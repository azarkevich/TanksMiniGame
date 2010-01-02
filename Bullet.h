#pragma once

#include "Object.h"
#include "Common.h"

class World;

class Bullet : public Object
{
	World *_world;
public:
	Bullet(World *w, int orient);

	virtual void draw(SDL_Surface *s, int level)
	{
		if(level != 0)
			return;
		sprite->draw(s, X, Y);
	}

	virtual void think()
	{
		sprite->think();
	}

	virtual int type()
	{
		return OBJ_BULLET;
	}

	virtual bool wait_weapon_hit()
	{
		return true;
	}

	virtual void weapon_hit(Object *o);
};
