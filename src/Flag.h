#pragma once

#include "Object.h"

class World;

class Flag : public Object
{
	World *world;
public:
	bool Dead;
	Flag(World *w, int x, int y);
	void think()
	{
		sprite->think();
	}
	void draw(SDL_Surface *s)
	{
		sprite->draw(s, X, Y);
	}
	int type()
	{
		return OBJ_FLAG;
	}
	bool hit_by(Bullet *b);
};
