#pragma once

#include "Object.h"

class World;

class Bush : public Object
{
public:
	Bush(World *w, int x, int y);
	~Bush();

	virtual void draw(SDL_Surface *s, int level)
	{
		if(level != 2)
			return;
		sprite->draw(s, X, Y);
	}

	virtual void think()
	{
	}

	virtual int type()
	{
		return OBJ_BUSH;
	}
};
