#pragma once

#include "Bullet.h"

class World;

class Wall : public Object
{
	World *_world;
public:
	Wall(World *w, int x, int y);
	~Wall();

	virtual void draw(SDL_Surface *s);
	virtual void think();

	virtual int type();

	virtual bool hit_by(Bullet *b);
};
