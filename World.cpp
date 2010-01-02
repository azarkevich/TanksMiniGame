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
			_objs.push_back(new Wall(this, x, y));
			break;
		case 'b':
			_objs.push_back(new Bush(this, x, y));
			break;
		case 't':
			_objs.push_back(new Tank(this, x, y, 2));
			break;
		case 'T':
			_objs.push_back(new Tank(this, x, y, 3));
			break;
		case 'h':
			_objs.push_back(new Tank(this, x, y, 4));
			break;
		case 'H':
			_objs.push_back(new Tank(this, x, y, 5));
			break;
		case 'p':
			_player = new Tank(this, x, y, 5);
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
		if(ev.key.keysym.sym == SDLK_RETURN)
		{
			_show_bb = (_show_bb + 1) % 3;
		}
		if(ev.key.keysym.sym == SDLK_p)
		{
			pause(!_pause);
		}
		if(_pause)
			return;
		if(ev.key.keysym.sym == SDLK_SPACE)
		{
			_player->fire();
		}
		if(ev.key.keysym.sym == SDLK_UP || ev.key.keysym.sym == SDLK_DOWN || ev.key.keysym.sym == SDLK_LEFT || ev.key.keysym.sym == SDLK_RIGHT)
		{
			int old_orient = _player->Orient;
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

			if(is_possible_position(_player) == false)
			{
				_player->move_to(old_orient);
				_player->stop();
			}
		}
	}
	if(ev.type == SDL_KEYUP)
	{
		if(_pause)
			return;
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

bool World::is_possible_position(Object *self)
{
	for(unsigned int i=0;i<_objs.size();i++)
	{
		Object *o = _objs[i];
		// Not compare with self
		if(o == self)
			continue;
		// compare only wth: tanks, walls, TODO flag
		if(o->type() != OBJ_TANK && o->type() != OBJ_WALL)
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

template<typename T>
void World::handle_removed(vector<T *> &v)
{
	for(unsigned int i=0;i<v.size();i++)
	{
		if(v[i]->is_removed())
		{
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

	// add pending objects
	for(unsigned int i=0;i<add_queue.size();i++)
	{
		if(add_queue[i].first <= WorldTime::now)
		{
			add_object(add_queue[i].second);
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
		if(o->type() != OBJ_TANK)
			continue;

		try_move_tank((Tank *)o);
	}

	// Bullet collisions with tanks and walls
	for(unsigned int i=0;i<_objs.size();i++)
	{
		// select bullets
		if(_objs[i]->is_removed() || _objs[i]->type() != OBJ_BULLET)
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
			if(o->is_removed())
				continue;
			// check with: tanks, bullets, walls, TODO flag
			if(o->type() != OBJ_TANK && o->type() != OBJ_BULLET && o->type() != OBJ_WALL)
				continue;

			if(BounceBox::is_intersect(b, o))
			{
				b->hit(o);
				// if bullet removed by hit - skip other checks
				if(b->is_removed())
					break;
			}
		}
	}
	
	// think
	for(unsigned int i=0;i<_objs.size();i++)
	{
		_objs[i]->think();
	}
	
	// Remove pending objects
	for(unsigned int i=0;i<_objs.size();i++)
	{
		if(_objs[i]->is_removed())
		{
			delete _objs[i];
			_objs.erase(_objs.begin() + i);
			i--;
		}
	}
//	cout << _objs.size() << endl;
}

void World::add_explode(int x, int y)
{
	_objs.push_back(new Explode(this, x, y));
}

void World::add_explode_area(SDL_Rect box, int count)
{
	int delay = 0;
	for(int i=0;i<count; i++)
	{
		int x = box.x - 16 + (rand() % box.w);
		int y = box.y - 16 + rand() % box.h;
	
		add_object_delay(new Explode(this, x, y), delay);
		delay += 200 + rand() % 100;
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
		if(o->type() == OBJ_WALL || o->type() == OBJ_BULLET || o->type() == OBJ_TANK)
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
