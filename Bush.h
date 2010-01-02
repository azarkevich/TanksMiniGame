#pragma once

#include "Object.h"

class Bush : public Object
{
public:
	Bush(int x, int y);

	virtual void draw(SDL_Surface *s)
	{
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
