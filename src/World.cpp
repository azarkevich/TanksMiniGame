#include "StdAfx.h"
#include "Object.h"
#include "World.h"
#include "Wall.h"
#include "Explode.h"
#include "Bush.h"
#include "Bullet.h"
#include "Tank.h"
#include "Flag.h"
#include "Gun.h"
#include "Bonus.h"

Uint32 WorldTime::now;

World::World(int w, int h)
	: _levelIndex(0)
{
	struct stat s;

	heart.load(TilesCache::main, "resources/heart.sprite");
	game_mode = GAME_MODE_START;
	_show_bb = 0;
	bounds.x=0;
	bounds.y=0;
	bounds.w = w;
	bounds.h = h;
	_player = NULL;
	_remote_player = NULL;
	player_start_x = 0;
	player_start_y = 0;
	remote_player_start_x = 0;
	remote_player_start_y = 0;
	mode_image[0] = NULL;
	mode_image[1] = NULL;
	mode_image[2] = SDL_LoadBMP("resources/game-pause.bmp");
	mode_image[3] = SDL_LoadBMP("resources/game-over.bmp");
	mode_image[4] = SDL_LoadBMP("resources/game-win.bmp");
	for(int i=0;i<5;i++)
	{
		if(mode_image[i] != NULL)
		{
			SDL_SetColorKey(mode_image[i], SDL_SRCCOLORKEY, SDL_MapRGB(mode_image[i]->format, 0, 0, 0));
			SDL_SetAlpha(mode_image[i], SDL_SRCALPHA, 240);
		}
	}
}

World::~World()
{
	delete_objects();
	for(int i=0;i<5;i++)
	{
		if(mode_image[i] != NULL)
			SDL_FreeSurface(mode_image[i]);
	}
}

void World::load_current_level()
{
	int width = 0;
	int height = 0;

	int x = 1;
	int y = 1;
	
	char level[256];
	sprintf(level, "resources/%03d.level", _levelIndex);

	ifstream ifs(level);
	if(ifs.bad())
	{
		cout << "Can't open level file " << level << endl;
		exit(1);
	}
	
	EnvMap.clear();
	
	while(ifs.eof() == false)
	{
		char l[256];
		ifs.getline(l, 256);
		
		x = 0;

		vector<Object *> map;
		for(unsigned int i=0;i<strlen(l);i++)
		{
			Object *map_element = NULL;
			switch(l[i])
			{
			case ' ':
				break;
			case 'w':
				map_element = new Wall(this, x, y, WALL_A);
				_objs.push_back(map_element);
				break;
			case 'W':
				map_element = new Wall(this, x, y, WALL_B);
				_objs.push_back(map_element);
				break;
			case 'c':
				map_element = new Wall(this, x, y, WALL_CONCRETE_A);
				_objs.push_back(map_element);
				break;
			case 'C':
				map_element = new Wall(this, x, y, WALL_CONCRETE_B);
				_objs.push_back(map_element);
				break;
			case 'g':
				map_element = new Gun(this, x, y, ORIENT_UP, 5, 100);
				_objs.push_back(map_element);
				break;
			case 'G':
				map_element = new Gun(this, x, y, ORIENT_DOWN, 5, 100);
				_objs.push_back(map_element);
				break;
			case 'b':
				_objs.push_back(new Bush(x, y));
				break;
			case 't':
				_objs.push_back(new Tank(this, TANK_ENIMY, x, y, 2));
				break;
			case 'T':
				_objs.push_back(new Tank(this, TANK_ENIMY, x, y, 3));
				break;
			case 'h':
				_objs.push_back(new Tank(this, TANK_ENIMY, x, y, 4));
				break;
			case 'H':
				_objs.push_back(new Tank(this, TANK_ENIMY, x, y, 5));
				break;
			case 's':
				map_element = new Bonus(x, y, BONUS_SUPER_BULLET);
				_objs.push_back(map_element);
				break;
			case '>':
				map_element = new Bonus(x, y, BONUS_SPEED);
				_objs.push_back(map_element);
				break;
			case '+':
				map_element = new Bonus(x, y, BONUS_HEAL);
				_objs.push_back(map_element);
				break;
			case 'p':
				player_start_x = x;
				player_start_y = y;
				break;
			case 'P':
				remote_player_start_x = x;
				remote_player_start_y = y;
				break;
			case 'F':
				map_element = enimy_flag = new Flag(this, x, y);
				_objs.push_back(enimy_flag);
				break;
			case 'f':
				map_element = player_flag = new Flag(this, x, y);
				_objs.push_back(player_flag);
				break;
			}
			
			map.push_back(map_element);
			
			x += 32;
		}
		
		EnvMap.push_back(map);

		y += 32;
		width = std::max(width, x);
	}
	height = y;
	bounds.w = width;
	bounds.h = height;
	
	for(unsigned int i=0;i<EnvMap.size();i++)
	{
		EnvMap[i].resize(width/32);
	}
}

void World::handle_input(bool remote_player, int key)
{
	if(key == SDLK_b)
	{
		_show_bb = (_show_bb + 1) % 3;
		return;
	}

	// Lose/win mode. Any key - begin new game
	if(game_mode == GAME_MODE_LOSE || game_mode == GAME_MODE_WIN)
	{
		if(key == SDLK_SPACE)
		{
			if (game_mode == GAME_MODE_WIN)
			{
				// load next layer
				_levelIndex++;
			}
			else if (game_mode == GAME_MODE_LOSE)
			{
				// load first layer
				_levelIndex = 0;
			}

			game_mode = GAME_MODE_START;
		}

		// retry game
		if (key == SDLK_r)
		{
			game_mode = GAME_MODE_START;
		}
		return;
	}
	
	if(game_mode == GAME_MODE_PLAY || game_mode == GAME_MODE_PAUSE)
	{
		if(key == SDLK_p)
		{
			if(game_mode == GAME_MODE_PLAY)
			{
				game_mode = GAME_MODE_PAUSE;
			}
			else if(game_mode == GAME_MODE_PAUSE)
			{
				game_mode = GAME_MODE_PLAY;
			}
			return;
		}
	}
	
	Tank *p = remote_player ? _remote_player : _player;
	
	if(p != NULL && game_mode == GAME_MODE_PLAY)
	{
		if(key == SDLK_SPACE)
		{
			p->fire();
		}
		if(key == SDLK_UP || key == SDLK_DOWN || key == SDLK_LEFT || key == SDLK_RIGHT)
		{
			int old_orient = p->Orient;
			if(key == SDLK_UP)
			{
				p->move_to(ORIENT_UP);
			}
			if(key == SDLK_DOWN)
			{
				p->move_to(ORIENT_DOWN);
			}
			if(key == SDLK_LEFT)
			{
				p->move_to(ORIENT_LEFT);
			}
			if(key == SDLK_RIGHT)
			{
				p->move_to(ORIENT_RIGHT);
			}

			if(is_possible_position(p) == false)
			{
				p->move_to(old_orient);
				p->stop();
			}
		}
	}
}

void World::move_obj(Object *o)
{
	if(o->move_info == NULL || o->move_info->NextMoveAt > WorldTime::now)
		return;

	if(o->move_info->Orient == ORIENT_UP)
	{
		o->Y--;
	}
	else if(o->move_info->Orient == ORIENT_DOWN)
	{
		o->Y++;
	}
	else if(o->move_info->Orient == ORIENT_LEFT)
	{
		o->X--;
	}
	else if(o->move_info->Orient == ORIENT_RIGHT)
	{
		o->X++;
	}
	o->move_info->NextMoveAt = WorldTime::now + o->move_info->Speed;
}

bool World::is_possible_position(Object *self)
{
	for(unsigned int i=0;i<_objs.size();i++)
	{
		Object *o = _objs[i];
		// Not compare with self
		if(o == self)
			continue;
		// compare only wth: tanks, walls, flag, guns
		int type = o->type();
		if(type != OBJ_TANK && type != OBJ_WALL && type != OBJ_FLAG && type != OBJ_GUN)
			continue;
		if(BounceBox::is_intersect(o, self))
		{
			return false;
		}
	}
	return true;
}

void World::try_move_tank(Tank *t)
{
	int old_x = t->X;
	int old_y = t->Y;
	move_obj(t);

	if(is_possible_position(t) == false)
	{
		t->X = old_x;
		t->Y = old_y;
		t->stop();
	}

	// not allow going out of screen:
	BounceBox bb = t->BBox;
	bb.x += t->X;
	bb.y += t->Y;
	// left
	if(bb.x < bounds.x)
	{
		t->X += (bounds.x - bb.x);
		t->stop();
	}
	// top
	if(bb.y < bounds.y)
	{
		t->Y += (bounds.y - bb.y);
		t->stop();
	}
	// right
	int screen_right = bounds.w - bounds.x;
	int bb_right = bb.x + bb.w;
	if(bb_right > screen_right)
	{
		int diff = bb_right - screen_right;
		t->X -= diff;
		bb_right -= diff;
		t->stop();
	}
	// down
	int screen_down = bounds.h - bounds.y;
	int bb_down = bb.y + bb.h;
	if(bb_down > screen_down)
	{
		int diff = bb_down - screen_down;
		t->Y -= diff;
		bb_down -= diff;
		t->stop();
	}
}

void World::delete_objects()
{
	for(unsigned int i = 0;i<_objs.size();i++)
	{
		delete _objs[i];
	}
	for (unsigned int i = 0; i<add_queue.size(); i++)
	{
		delete add_queue[i].second;
	}
	for (unsigned int i = 0; i<removed_objs.size(); i++)
	{
		delete removed_objs[i];
	}
	_objs.clear();
	removed_objs.clear();
	add_queue.clear();
}

void World::think()
{
	if(game_mode == GAME_MODE_PAUSE)
		return;

	if(game_mode == GAME_MODE_START)
	{
		delete_objects();
		player_flag = NULL;
		enimy_flag = NULL;
		_player = NULL;
		_remote_player = NULL;
		lives = 3;
		respawn_player_at = WorldTime::now + 10;
		if(network_game)
		{
			respawn_remote_player_at = WorldTime::now + 10;
			remote_lives = 3;
		}
		else
		{
			// never respawn
			respawn_remote_player_at = numeric_limits<Uint32>::max();
			remote_lives = 0;
		}
		load_current_level();
		game_mode = GAME_MODE_PLAY;
		EnvMapChanged = true;
		return;
	}

	if(respawn_player_at <= WorldTime::now)
	{
		_player = new Tank(this, TANK_PLAYER, player_start_x, player_start_y, 5);
		_player->init();
		_objs.push_back(_player);
		respawn_player_at = numeric_limits<Uint32>::max();
	}

	if(respawn_remote_player_at <= WorldTime::now)
	{
		_remote_player = new Tank(this, TANK_PLAYER, remote_player_start_x, remote_player_start_y, 5);
		_remote_player->init();
		_objs.push_back(_remote_player);
		respawn_remote_player_at = numeric_limits<Uint32>::max();
	}

	// add pending objects
	for(unsigned int i=0;i<add_queue.size();i++)
	{
		if(add_queue[i].first <= WorldTime::now)
		{
			Object *o = add_queue[i].second;
			if(o->move_info != NULL)
				o->move_info->NextMoveAt = WorldTime::now + o->move_info->Speed;
			add_object(o);
			add_queue.erase(add_queue.begin() + i);
			i--;
		}
	}
	// Move bullets
	for(unsigned int i=0;i<_objs.size();i++)
	{
		Object *o = _objs[i];
		if(o->type() != OBJ_BULLET)
			continue;

		move_obj(o);
		// remove out of screen bullets
		if(BounceBox::is_inside_rect(o, &bounds)==false)
		{
			o->remove();
		}
	}
	// try move tanks
	for(unsigned int i=0;i<_objs.size();i++)
	{
		Object *o = _objs[i];
		if(o->is_remove_pending() || o->type() != OBJ_TANK)
			continue;

		try_move_tank((Tank *)o);
	}

	// Bullet collisions with tanks and walls
	for(unsigned int i=0;i<_objs.size();i++)
	{
		// select bullets
		if(_objs[i]->is_remove_pending() || _objs[i]->type() != OBJ_BULLET)
			continue;
		Bullet *b = (Bullet *)_objs[i];

		// compare with other objects
		for(unsigned int j=0;j<_objs.size();j++)
		{
			// self to self deny
			if(i == j)
				continue;

			Object *o = _objs[j];
			// skip removed
			if(o->is_remove_pending())
				continue;
			// check with: tanks, bullets, walls, flag
			int type = o->type();
			if(type != OBJ_TANK && type != OBJ_BULLET && type != OBJ_WALL && type != OBJ_FLAG && type != OBJ_GUN)
				continue;

			if(BounceBox::is_intersect(b, o))
			{
				b->hit(o);
				// if bullet removed by hit - skip other checks
				if(b->is_remove_pending())
					break;
			}
		}
	}
	
	// Tank collisions with bonuses
	for(unsigned int i=0;i<_objs.size();i++)
	{
		// select bonuses
		if(_objs[i]->is_remove_pending() || _objs[i]->type() != OBJ_BONUS)
			continue;
		Bonus *b = (Bonus *)_objs[i];

		// compare with other objects
		for(unsigned int j=0;j<_objs.size();j++)
		{
			// self to self deny
			if(i == j)
				continue;

			if(_objs[j]->type() != OBJ_TANK || _objs[j]->is_remove_pending())
				continue;

			Tank *t = (Tank *)_objs[j];

			if(BounceBox::is_intersect(b, t))
			{
				b->action(t);
			}
		}
	}

	// think
	for(unsigned int i=0;i<_objs.size();i++)
	{
		_objs[i]->think();
	}
	
	// strategy calculated.
	EnvMapChanged = false;
	
	// Check game state
	if(game_mode == GAME_MODE_PLAY)
	{
		if(_player != NULL && _player->is_remove_pending())
		{
			_player = NULL;
			lives--;

			if(_player == NULL && lives > 0)
			{
				// respawn after 10 frames
				respawn_player_at = WorldTime::now + 10;
			}
		}
		
		if(_remote_player != NULL && _remote_player->is_remove_pending())
		{
			_remote_player = NULL;
			remote_lives--;

			if(_remote_player == NULL && remote_lives > 0)
			{
				// respawn after 10 frames
				respawn_remote_player_at = WorldTime::now + 10;
			}
		}
		
		if(lives <= 0 && remote_lives <= 0)
		{
			game_mode = GAME_MODE_LOSE;
		}

		if(player_flag != NULL && player_flag->Dead)
		{
			game_mode = GAME_MODE_LOSE;
		}

		if(enimy_flag != NULL && enimy_flag->Dead)
		{
			game_mode = GAME_MODE_WIN;
		}

		if(game_mode != GAME_MODE_PLAY)
		{
			if(_player != NULL)
				_player->stop();
			if(_remote_player != NULL)
				_remote_player->stop();
		}
	}
	
	// Remove pending objects from map
	for(unsigned int i=0;i<EnvMap.size();i++)
	{
		vector<Object *> &v = EnvMap[i];
		for(unsigned int j=0;j<v.size();j++)
		{
			if(v[j] != NULL && v[j]->is_remove_pending())
			{
				v[j] = NULL;
				EnvMapChanged = true;
			}
		}
	}

	// Remove removed objects
	for(unsigned int i=0;i<_objs.size();i++)
	{
		if(_objs[i]->is_removed())
		{
			removed_objs.push_back(_objs[i]);
			_objs.erase(_objs.begin() + i);
			i--;
		}
	}
}

void World::add_explode(int x, int y)
{
	_objs.push_back(new Explode(x, y));
}

void World::add_explode_area(SDL_Rect box, int count)
{
	int delay = 0;
	for(int i=0;i<count; i++)
	{
		int x = box.x - 16 + (rand() % box.w);
		int y = box.y - 16 + rand() % box.h;
	
		add_object_delay(new Explode(x, y), delay);
		delay += 20 + rand() % 10;
	}
}

void World::show_bb(SDL_Surface *s)
{
	// Draw BBoxes
	for(unsigned int i = 0;i<_objs.size();i++)
	{
		Object *o = _objs[i];

		BounceBox bb = o->BBox;
		bb.x += o->X;
		bb.y += o->Y;
		SDL_FillRect(s, &bb, SDL_MapRGB(s->format, 255, 0, 0));
	}
}

void World::draw(SDL_Surface *s)
{
	if(_show_bb == 1)
		show_bb(s);

	for(unsigned int i = 0;i<_objs.size();i++)
	{
		Object *o = _objs[i];
		int type = o->type();
		if(type != OBJ_EXPLODE && type != OBJ_BUSH && type != OBJ_TANK)
			o->draw(s);
	}

	for(unsigned int i = 0;i<_objs.size();i++)
	{
		Object *o = _objs[i];
		if(o->type() == OBJ_TANK)
			o->draw(s);
	}

	for(unsigned int i = 0;i<_objs.size();i++)
	{
		Object *o = _objs[i];
		if(o->type() == OBJ_EXPLODE)
			o->draw(s);
	}

	for(unsigned int i = 0;i<_objs.size();i++)
	{
		Object *o = _objs[i];
		if(o->type() == OBJ_BUSH)
			o->draw(s);
	}
	
	// draw hearts
	int x = 20;
	int y = bounds.h + 20;
	for(int i=0;i<lives;i++)
	{
		heart.draw(s, x + i*32, y);
	}

	x = 300;
	for(int i=0;i<remote_lives;i++)
	{
		heart.draw(s, x + i*32, y);
	}

	if(_show_bb == 2)
		show_bb(s);

	if(mode_image[game_mode] != NULL)
	{
		SDL_Rect dest;
		
		dest.x = (s->w - mode_image[game_mode]->w)/2;
		dest.y = (s->h - mode_image[game_mode]->h)/2;

		SDL_BlitSurface(mode_image[game_mode], NULL, s, &dest);
	}
}
