#pragma once

#include "Object.h"

class Explode : public Object
{
public:
	Explode(int x, int y);

	virtual void draw(SDL_Surface *s)
	{
		sprite->draw(s, X, Y);
	}

	virtual void think()
	{
		if(sprite->get_state() == SPRITE_DONE)
			remove();
		sprite->think();
	}

	virtual int type()
	{
		return OBJ_EXPLODE;
	}
};
