#pragma once

#include "Object.h"

class World;

class Explode : public Object
{
public:
	Explode(World *w, int x, int y);
	~Explode();

	virtual void draw(SDL_Surface *s, int level)
	{
		if(level != 1)
			return;
		sprite->draw(s, X, Y);
	}

	virtual void think()
	{
		if(sprite->get_state() == SPRITE_DONE)
			Removed = true;
		sprite->think();
	}

	virtual int type()
	{
		return OBJ_EXPLODE;
	}
};
