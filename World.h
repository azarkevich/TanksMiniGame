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
	vector<Bullet *> _bullets;
	vector<Wall *> _walls;
	vector<Bush *> _bushes;
	vector<Explode *> _explodes;
	vector<Tank *> _tanks;
	vector<Object *> _objs;
	Tank *_player;

	void move_obj(Object *o);
	void show_bb(SDL_Surface *s);

	void try_move_tank(Tank *t);
	template<typename T>
		void handle_removed(vector<T *> &v);
public:
	SDL_Rect bounds;

	vector<SDL_Surface *> tiles;

	World(int w, int h);
	~World();

	void load_level(const char* level);

	void pause(bool set);
	void add_explode(int x, int y);
	
	void add_bullet(Bullet *b)
	{
		_bullets.push_back(b);
		_objs.push_back(b);
	}

	void handle_input(const SDL_Event &ev);
	void think();
	void draw(SDL_Surface *s);
};
