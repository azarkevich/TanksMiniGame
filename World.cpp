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
}

World::~World()
{
	for(unsigned int i = 0;i<env.size();i++)
	{
		delete env[i];
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
			{
				Wall *w = new Wall(this);
				w->X = x;
				w->Y = y;
				env.push_back(w);
			}
			break;
		case 'b':
			{
				Object *o = new Bush(this);
				o->X = x;
				o->Y = y;
				env.push_back(o);
			}
			break;
		case 't':
			{
				tank = new Tank(this);
				tank->X = x;
				tank->Y = y;
				env.push_back(tank);
			}
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
			tank->fire();
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
			tank->move_to(ORIENT_UP);
		}
		if(ev.key.keysym.sym == SDLK_DOWN)
		{
			tank->move_to(ORIENT_DOWN);
		}
		if(ev.key.keysym.sym == SDLK_LEFT)
		{
			tank->move_to(ORIENT_LEFT);
		}
		if(ev.key.keysym.sym == SDLK_RIGHT)
		{
			tank->move_to(ORIENT_RIGHT);
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

void World::think()
{
	if(_pause)
		return;

	WorldTime::now = SDL_GetTicks() - _world_time_diff;

	// Move all movabe objects
	for(unsigned int i=0;i<env.size();i++)
	{
		if(env[i]->move_info != NULL)
		{
			move_obj(env[i]);
		}
	}

	// Remove out of screen bullets
	for(unsigned int i=0;i<env.size();i++)
	{
		Object *o = env[i];
		if(o->type() == OBJ_BULLET)
		{
			if(BounceBox::is_inside_rect(o, &bounds)==false)
			{
				delete env[i];
				env.erase(env.begin() + i);
				i--;

				break;
			}
		}
	}

	// Bullet collisions with others
	for(unsigned int i=0;i<env.size();i++)
	{
		Object *o1 = env[i];
		if(o1->type() == OBJ_BULLET)
		{
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
		}
	}
	
	// think
	for(unsigned int i=0;i<env.size();i++)
	{
		env[i]->think();
	}
	
	// Remove pending objects
	for(unsigned int i=0;i<env.size();i++)
	{
		if(env[i]->Removed)
		{
			delete env[i];
			env.erase(env.begin() + i);
			i--;
		}
	}

	// Add random bullet
	if(0 && rand() % 2 == 1)
	{
		Bullet *b = new Bullet(this, rand() % 4);
		b->X = rand() % 800;
		b->Y = rand() % 600;
		env.push_back(b);
	}
}

void World::add_explode(int x, int y)
{
	Explode *e = new Explode(this);
	e->X = x;
	e->Y = y;
	env.push_back(e);
}

void World::show_bb(SDL_Surface *s)
{
	// Draw BBoxes
	for(unsigned int i = 0;i<env.size();i++)
	{
		Object *o = env[i];

		BounceBox bb = o->BBox;
		bb.x += o->X;
		bb.y += o->Y;
		SDL_FillRect(s, &bb, SDL_MapRGB(s->format, 200, 0, 0));
	}
}

void World::draw(SDL_Surface *s)
{
	if(_show_bb == 1)
		show_bb(s);

	for(unsigned int i = 0;i<env.size();i++)
	{
		env[i]->draw(s, 0);
	}
	
	for(unsigned int i = 0;i<env.size();i++)
	{
		env[i]->draw(s, 1);
	}

	for(unsigned int i = 0;i<env.size();i++)
	{
		env[i]->draw(s, 2);
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
