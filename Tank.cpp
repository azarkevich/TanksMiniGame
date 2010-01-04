#include "StdAfx.h"
#include "Tank.h"
#include "World.h"
#include "Bullet.h"

Tank::Tank(World *w, int tank_type, int x, int y, int armo)
{
	next_fire_at = 0;
	X = x;
	Y = y;
	_world = w;
	_orients[ORIENT_UP] = new Sprite();
	_orients[ORIENT_DOWN] = new Sprite();
	_orients[ORIENT_LEFT] = new Sprite();
	_orients[ORIENT_RIGHT] = new Sprite();
	if(tank_type == TANK_PLAYER)
	{
		_orients[ORIENT_UP]->load(TilesCache::main, "resources/tank-a-up.sprite");
		_orients[ORIENT_DOWN]->load(TilesCache::main, "resources/tank-a-down.sprite");
		_orients[ORIENT_LEFT]->load(TilesCache::main, "resources/tank-a-left.sprite");
		_orients[ORIENT_RIGHT]->load(TilesCache::main, "resources/tank-a-right.sprite");
	}
	else if(tank_type == TANK_ENIMY)
	{
		_orients[ORIENT_UP]->load(TilesCache::main, "resources/tank-b-up.sprite");
		_orients[ORIENT_DOWN]->load(TilesCache::main, "resources/tank-b-down.sprite");
		_orients[ORIENT_LEFT]->load(TilesCache::main, "resources/tank-b-left.sprite");
		_orients[ORIENT_RIGHT]->load(TilesCache::main, "resources/tank-b-right.sprite");
	}

	_orients_bb[ORIENT_UP] = BounceBox(4, 2, 24, 28);
	_orients_bb[ORIENT_DOWN] = BounceBox(4, 2, 24, 28);
	_orients_bb[ORIENT_LEFT] = BounceBox(2, 4, 28, 24);
	_orients_bb[ORIENT_RIGHT] = BounceBox(2, 4, 28, 24);

	Orient = ORIENT_UP;
	sprite = _orients[Orient];
	BBox = _orients_bb[Orient];
	InitialArmour = Armour = armo;
	
	BulletArmour = 0;
	
	TankSpeed = 32;
	
	WeaponReloadTime = 500;
}

void Tank::fire()
{
	if(next_fire_at > WorldTime::now)
		return;
	
	next_fire_at = WorldTime::now + WeaponReloadTime;
	
	int ex = X, ey = Y;
	if(Orient == ORIENT_UP || Orient == ORIENT_DOWN)
		ex = X + BBox.x + BBox.w/2;

	if(Orient == ORIENT_LEFT || Orient == ORIENT_RIGHT)
		ey = Y + BBox.y + BBox.h/2;

	if(Orient == ORIENT_DOWN)
		ey += BBox.h;

	if(Orient == ORIENT_RIGHT)
		ex += BBox.w;

	Bullet *b = new Bullet(_world, Orient, ex, ey, this);
	b->set_armour(BulletArmour);
	_world->add_object(b);
}

void Tank::draw(SDL_Surface *s)
{
	sprite->draw(s, X, Y);

	if(!is_remove_pending())
	{
		SDL_Rect r;
		r.x = X - 5;
		r.y = Y - 5;
		if(r.x > 0 && r.y > 0)
		{
			r.w = 5 + (InitialArmour * 5);
			r.h = 4;
			SDL_FillRect(s, &r, SDL_MapRGB(s->format, 0, 0, 0));
			r.x++;
			r.y++;
			r.h-=2;
			r.w-=2;
			SDL_FillRect(s, &r, SDL_MapRGB(s->format, 255, 0, 0));
			if(Armour > 0 && InitialArmour > 0)
			{
				r.w = r.w * (1.0 * Armour / InitialArmour);
				SDL_FillRect(s, &r, SDL_MapRGB(s->format, 0, 255, 0));
			}
		}
	}
}

void Tank::think_fire()
{
	// want fire if ahead: player, flag
	// not want if ahead: enimy_flag
	
	int drow = 0;
	int dcol = 0;
	unsigned int col = X/32;
	unsigned int row = Y/32;
	
	if(Orient == ORIENT_UP)
		drow = -1;
	else if(Orient == ORIENT_DOWN)
		drow = 1;
	else if(Orient == ORIENT_LEFT)
		dcol = -1;
	else if(Orient == ORIENT_RIGHT)
		dcol = 1;
	
	while(col >= 0 && row >= 0 && row < (_world->EnvMap.size()-1) && col < (_world->EnvMap[0].size()-1))
	{
		row += drow;
		col += dcol;
		Object *o = _world->EnvMap[row][col];
		if(_world->enimy_flag == o)
			return;
	}
	
	fire();
}

void Tank::think()
{
	sprite->think();

	if(this == _world->_player || this == _world->_remote_player)
		return;

	// enimy AI

	// auto fire for enimies
	if(next_fire_at < WorldTime::now && rand() % 50 == 0)
		think_fire();
	
	if(_world->EnvMapChanged || path_to_target.size() == 0)
	{
		think_strategey();
	}
	
	if(path_to_target.size() == 0)
		return;
	
	// we still not reach next cell
	if(Y == path_to_target[0].first*32 && X == path_to_target[0].second*32)
	{
		path_to_target.pop_front();
	}
	
	if(path_to_target.size() == 0)
	{
		// we are here !
		stop();
		return;
	}

	if(Y != path_to_target[0].first*32 || X != path_to_target[0].second*32)
	{
		// which direction ?
		int orient = -1;
		if(Y > path_to_target[0].first*32)
			orient = ORIENT_UP;
		else if(Y < path_to_target[0].first*32)
			orient = ORIENT_DOWN;
		else if(X > path_to_target[0].second*32)
			orient = ORIENT_LEFT;
		else if(X < path_to_target[0].second*32)
			orient = ORIENT_RIGHT;

		if(move_info != NULL && move_info->Orient != orient)
		{
			delete move_info;
			move_info = NULL;
		}

		// begin movint to target
		if(move_info == NULL)
		{
			move_to(orient);
		}
	}
}

bool Tank::is_can_goto(int row, int col)
{
	int type = OBJ_NONE;
	if(_world->EnvMap[row][col] != NULL)
		type = _world->EnvMap[row][col]->type();
	
	if(type == OBJ_WALL || type == OBJ_TANK || type == OBJ_FLAG || type == OBJ_GUN)
		return false;
	
	return true;
}

void Tank::think_strategey()
{
	calc_paths();

	// flag
	int flag_row = _world->player_flag->Y/32;
	int flag_col = _world->player_flag->X/32;
	
	// find path to flag
	path_to_target.clear();
	// TODO: find free point about flag! see -1
	if(get_path_to(flag_row, flag_col - 1, path_to_target) == false)
	{
		
	}
}

void Tank::calc_paths()
{
	paths = vector< vector<int > >(_world->EnvMap.size(), vector<int>(_world->EnvMap[0].size(), -1));
	// my cell:
	int col = X/32;
	int row = Y/32;
	paths[row][col] = 0;
	int step = 0;
	bool path_added = true;
	while(path_added)
	{
		path_added = false;
//		show_wave(step, paths);
		for(unsigned int i=0;i<paths.size();i++)
		{
			for(unsigned int j=0;j<paths[i].size();j++)
			{
				if(paths[i][j] != step)
					continue;
				
				// can't go from this point. skip
				if(is_can_goto(i, j) == false)
					continue;
				
				// free to step up
				if(i > 0 && is_can_goto(i-1, j) && paths[i-1][j] == -1)
				{
					paths[i-1][j] = step+1;
				}
				// free to step down
				if(i < (paths.size()-1) && is_can_goto(i+1, j) && paths[i+1][j] == -1)
				{
					paths[i+1][j] = step+1;
				}
				// free to step left
				if(j > 0 && is_can_goto(i, j-1) && paths[i][j-1] == -1)
				{
					paths[i][j-1] = step+1;
				}
				// free to step right
				if(j < (paths[i].size() - 1) && is_can_goto(i, j+1) && paths[i][j+1] == -1)
				{
					paths[i][j+1] = step+1;
				}
				path_added = true;
			}
		}
		step++;
	}
}

// previous step
pair<int, int> Tank::get_prev_step(unsigned int row, unsigned int col)
{
	int prev = paths[row][col] - 1;
	
	if(row > 0 && paths[row - 1][col] == prev)
		return make_pair(row - 1, col);
	
	if(row < (paths.size() - 1) && paths[row + 1][col] == prev)
		return make_pair(row + 1, col);

	if(col > 0 && paths[row][col - 1] == prev)
		return make_pair(row, col - 1);
	
	if(col < (paths[0].size() - 1) && paths[row][col + 1] == prev)
		return make_pair(row, col + 1);
	
	return make_pair(-1, -1);
}

bool Tank::get_path_to(int row, int col, deque< pair<int, int> > &v)
{
	if(paths.size() == 0)
		calc_paths();
	
	if(paths[row][col] == 0)
		return true;
	
	v.push_front(make_pair(row, col));

	pair<int,int> prev_pt = get_prev_step(row, col);
	if(prev_pt.first == -1)
		return false;
	
//	cout << "Row=" << prev_pt.first << " Col=" << prev_pt.second << endl;

	return get_path_to(prev_pt.first, prev_pt.second, v);
}

void Tank::show_wave(int step, vector< vector<int > > &paths)
{
	cout << "Step " << step << endl;
	for(unsigned int i=0;i<paths.size();i++)
	{
		for(unsigned int j=0;j<paths[i].size();j++)
		{
			cout << setw(3) << paths[i][j] << " ";
		}
		cout << endl;
	}
	cout << endl;
	
	cout << "Press ENTER" << endl;
	char t[10];
	cin.getline(t, 10);
}

int Tank::type()
{
	return OBJ_TANK;
}

void Tank::move_to(int orient)
{
	if(move_info != NULL && ((Orient == ORIENT_UP && orient == ORIENT_DOWN) ||
		(Orient == ORIENT_DOWN && orient == ORIENT_UP) ||
		(Orient == ORIENT_LEFT && orient == ORIENT_RIGHT) ||
		(Orient == ORIENT_RIGHT && orient == ORIENT_LEFT)))
	{
		// stop on opposite movement
		stop();
		return;
	}

	Orient = orient;

	if(move_info != NULL)
	{
		delete move_info;
		move_info = NULL;
	}
	move_info = new MoveInfo(TankSpeed, Orient);

	sprite = _orients[Orient];
	sprite->play(true);

	BBox = _orients_bb[Orient];
}

void Tank::stop()
{
	sprite->set_state(SPRITE_STOP);
	if(move_info != NULL)
	{
		delete move_info;
		move_info = NULL;
	}
}

bool Tank::hit_by(Bullet *b)
{
	if(Armour == 0)
	{
		remove_delay(2000);
		BounceBox b = BBox;
		b.x += X;
		b.y += Y;
		_world->add_explode_area(b, 10);
		BBox = BounceBox();
	}
	else
	{
		Armour--;
		_world->add_explode(b->X, b->Y);
	}
	return true;
}
