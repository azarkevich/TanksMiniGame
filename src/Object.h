#pragma once

#include "Common.h"
#include "BounceBox.h"
#include "Sprite.h"

class Bullet;

class Object
{
	Uint32 remove_at;
public:
	int X, Y;
	Sprite *sprite;
	MoveInfo *move_info;
	BounceBox BBox;

	Object();
	virtual ~Object();

	virtual void draw(SDL_Surface *s) = 0;
	
	virtual void think() { };

	virtual int type() = 0;

	virtual bool hit_by(Bullet *o);

	void remove(Uint32 at = 0);
	void remove_delay(Uint32 delay);
	bool is_removed();
	bool is_remove_pending();
};
