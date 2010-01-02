#pragma once

#include "Object.h"
#include "Tank.h"

class World
{
	bool _pause;
	Uint32 _world_time_diff;
	int _show_bb;
public:
	SDL_Rect bounds;

	vector<SDL_Surface *> tiles;
	vector<Object *> env;
	Tank *tank;

	World(int w, int h);
	~World();

	void load_level(const char* level);

	void move_obj(Object *o);
	void show_bb(SDL_Surface *s);
	void pause(bool set);
	void add_explode(int x, int y);
	void add_object(Object *b)
	{
		env.push_back(b);
	}

	void handle_input(const SDL_Event &ev);
	void think();
	void draw(SDL_Surface *s);
};
