#include "StdAfx.h"
#include "Object.h"
#include "World.h"
#include "Wall.h"
#include "Explode.h"
#include "Bush.h"
#include "Bullet.h"
#include "Tank.h"

Uint32 WorldTime::now;

World::World(int w, int h)
{
	_pause = false;
	_world_time_diff = 0;
	_show_bb = 0;
	bounds.x=0;
	bounds.y=0;
	bounds.w = w;
	bounds.h = h;
	_player = NULL;
}

World::~World()
{
	for(unsigned int i = 0;i<_objs.size();i++)
	{
		delete _objs[i];
	}

	for_each(tiles.begin(), tiles.end(), SDL_FreeSurface);
}

void World::load_level(const char* level)
{
	int width = 0;
	int height = 0;

	int x = 1;
	int y = 1;

	for(unsigned int i=0;i<strlen(level);i++)
	{
		if(level[i] == '\r')
			continue;
		if(level[i] == '\n')
		{
			x = 0;
			y += 33;
			height = std::max(height, y);
			continue;
		}

		switch(level[i])
		{
		case ' ':
			break;
		case 'w':
			_walls.push_back(new Wall(this, x, y));
			_objs.push_back(_walls.back());
			break;
		case 'b':
			_bushes.push_back(new Bush(this, x, y));
			_objs.push_back(_bushes.back());
			break;
		case 't':
			_tanks.push_back(new Tank(this, x, y));
			_objs.push_back(_tanks.back());
			break;
		case 'p':
			_player = new Tank(this, x, y);
			_objs.push_back(_player);
			break;
		}
		x += 33;
		width = std::max(width, x);
	}
}

void World::handle_input(const SDL_Event &ev)
{
	if(ev.type == SDL_KEYDOWN)
	{
		if(ev.key.keysym.sym == SDLK_SPACE)
		{
			_player->fire();
		}
		if(ev.key.keysym.sym == SDLK_RETURN)
		{
			_show_bb = (_show_bb + 1) % 3;
		}
		if(ev.key.keysym.sym == SDLK_p)
		{
			pause(!_pause);
		}
		if(ev.key.keysym.sym == SDLK_UP)
		{
			_player->move_to(ORIENT_UP);
		}
		if(ev.key.keysym.sym == SDLK_DOWN)
		{
			_player->move_to(ORIENT_DOWN);
		}
		if(ev.key.keysym.sym == SDLK_LEFT)
		{
			_player->move_to(ORIENT_LEFT);
		}
		if(ev.key.keysym.sym == SDLK_RIGHT)
		{
			_player->move_to(ORIENT_RIGHT);
		}
	}
	if(ev.type == SDL_KEYUP)
	{
		if(ev.key.keysym.sym == SDLK_UP || ev.key.keysym.sym == SDLK_DOWN || ev.key.keysym.sym == SDLK_LEFT || ev.key.keysym.sym == SDLK_RIGHT)
		{
			//tank->stop();
		}
	}
}

void World::move_obj(Object *o)
{
	if(o->move_info == NULL)
		return;

	// move in pixels = time span / velocity
	double time_span_sec = (WorldTime::now - o->move_info->_last_calc)/1000.0;

	double delta = o->move_info->Velocity * time_span_sec;
	if(o->move_info->Orient == ORIENT_UP || o->move_info->Orient == ORIENT_LEFT)
	{
		o->move_info->delta -= delta;
	}
	else if(o->move_info->Orient == ORIENT_DOWN || o->move_info->Orient == ORIENT_RIGHT)
	{
		o->move_info->delta += delta;
	}

	o->move_info->_last_calc = WorldTime::now;

	if(o->move_info->delta > 1 || o->move_info->delta < -1)
	{
		int idelta = (int)o->move_info->delta;
		o->move_info->delta -= idelta;

		if(o->move_info->Orient == ORIENT_UP || o->move_info->Orient == ORIENT_DOWN)
		{
			o->Y += idelta;
		}
		else if(o->move_info->Orient == ORIENT_LEFT || o->move_info->Orient == ORIENT_RIGHT)
		{
			o->X += idelta;
		}
	}
}

void World::try_move_tank(Tank *t)
{
	//int x = t->X;
	//int y = t->Y;
	move_obj(t);
	// not allow going out of screen:
	BounceBox bb = t->BBox;
	bb.x += t->X;
	bb.y += t->Y;
	// left
	if(bb.x < bounds.x)
		t->X += (bounds.x - bb.x);
	// top
	if(bb.y < bounds.y)
		t->Y += (bounds.y - bb.y);
	// right
	int screen_right = bounds.w - bounds.x;
	if((bb.x + bb.w) > screen_right)
		t->X -= (screen_right - (bb.x + bb.w));
	// down
	int screen_down = bounds.h - bounds.y;
	if((bb.y + bb.h) > screen_down)
		t->Y -= (screen_down - (bb.y + bb.h));
}

template<typename T>
void World::handle_removed(vector<T *> &v)
{
	for(unsigned int i=0;i<v.size();i++)
	{
		if(v[i]->Removed)
		{
			delete v[i];
			v.erase(v.begin() + i);
			i--;
		}
	}
}

void World::think()
{
	if(_pause)
		return;

	WorldTime::now = SDL_GetTicks() - _world_time_diff;

	// Move bullets
	for(unsigned int i=0;i<_bullets.size();i++)
	{
		move_obj(_bullets[i]);
		// remove out of screen bullets
		if(BounceBox::is_inside_rect(_bullets[i], &bounds)==false)
		{
			_bullets[i]->Removed = true;
		}
	}
	//  try move tanks
	for(unsigned int i=0;i<_tanks.size();i++)
	{
		try_move_tank(_tanks[i]);
	}
	// and player
	try_move_tank(_player);

	// Bullet collisions with tanks and walls
	for(unsigned int i=0;i<_bullets.size();i++)
	{
		Bullet *b = _bullets[i];
		if(b->Removed)
			continue;
/*
		for(unsigned int j=0;j<env.size();j++)
		{
			if(i == j)
				continue;

			Object *o2 = env[j];
			if(o2->wait_weapon_hit() == false)
				continue;

			if(BounceBox::is_intersect(o1, o2))
			{
				o2->weapon_hit(o1);

				delete o1;
				env.erase(env.begin() + i);
				i--;
				break;
			}
		}
*/
	}
	
	// think
	for(unsigned int i=0;i<_objs.size();i++)
	{
		_objs[i]->think();
	}
	
	// Remove pending objects
	handle_removed(_bullets);
	handle_removed(_tanks);
	handle_removed(_explodes);
	handle_removed(_walls);
	handle_removed(_bushes);
	for(unsigned int i=0;i<_objs.size();i++)
	{
		if(_objs[i]->Removed)
		{
			_objs.erase(_objs.begin() + i);
			i--;
		}
	}
}

void World::add_explode(int x, int y)
{
	_explodes.push_back(new Explode(this, x, y));
	_objs.push_back(_explodes.back());
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

//	for_each(_walls.begin(), _walls.end(),
//			mem_fun_ptr(Object::draw));

	for(unsigned int i = 0;i<_walls.size();i++)
	{
		_walls[i]->draw(s, 0);
	}

	for(unsigned int i = 0;i<_bullets.size();i++)
	{
		_bullets[i]->draw(s, 0);
	}

	for(unsigned int i = 0;i<_tanks.size();i++)
	{
		_tanks[i]->draw(s, 0);
	}
	_player->draw(s, 0);

	for(unsigned int i = 0;i<_explodes.size();i++)
	{
		_explodes[i]->draw(s, 0);
	}

	for(unsigned int i = 0;i<_bushes.size();i++)
	{
		_bushes[i]->draw(s, 0);
	}

	if(_show_bb == 2)
		show_bb(s);
}

void World::pause(bool set)
{
	if(!_pause && set)
	{
		_pause = true;
	}
	else if(_pause && !set)
	{
		_pause = false;
		_world_time_diff = SDL_GetTicks() - WorldTime::now;
	}
}
