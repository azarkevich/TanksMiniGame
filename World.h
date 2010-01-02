#pragma once

#include "Object.h"
#include "Tank.h"
#include "Bullet.h"
#include "Bush.h"
#include "Wall.h"
#include "Explode.h"

class World
{
	bool _pause;
	Uint32 _world_time_diff;
	int _show_bb;
	vector< pair<Uint32, Object *> > add_queue;
	vector<Object *> _objs;
	Tank *_player;

	void move_obj(Object *o);
	void show_bb(SDL_Surface *s);

	void try_move_tank(Tank *t);
	template<typename T>
		void handle_removed(vector<T *> &v);
	bool is_possible_position(Object *o);
public:
	SDL_Rect bounds;

	World(int w, int h);
	~World();

	void load_level(const char* level);

	void pause(bool set);
	void add_explode(int x, int y);
	
	void add_object(Object *o)
	{
		_objs.push_back(o);
	}
	void add_object_delay(Object *o, Uint32 delay)
	{
		add_queue.push_back(make_pair(WorldTime::now + delay, o));
	}
	void add_explode_area(SDL_Rect box, int count);
	void handle_input(const SDL_Event &ev);
	void think();
	void draw(SDL_Surface *s);
};
