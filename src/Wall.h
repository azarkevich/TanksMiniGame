#pragma once

#include "Bullet.h"

class World;

#define WALL_A 0
#define WALL_B 1
#define WALL_CONCRETE_A 2
#define WALL_CONCRETE_B 3

class Wall : public Object
{
	World *_world;
	int wall_type;
public:
	Wall(World *w, int x, int y, int wall_type);
	~Wall();

	virtual void draw(SDL_Surface *s);
	virtual void think();

	virtual int type();

	virtual bool hit_by(Bullet *b);
};
