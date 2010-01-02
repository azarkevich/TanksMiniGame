#pragma once

#include "Common.h"
#include "BounceBox.h"
#include "Sprite.h"

class Object
{
public:
	int X, Y;
	Sprite *sprite;
	MoveInfo *move_info;
	BounceBox BBox;
	bool Removed;

	Object();
	virtual ~Object();

	virtual void draw(SDL_Surface *s, int level) = 0;
	
	virtual void think()
	{
	};

	virtual int type() = 0;

	virtual bool wait_weapon_hit()
	{
		return false;
	}

	virtual void weapon_hit(Object *o)
	{
	}
};
