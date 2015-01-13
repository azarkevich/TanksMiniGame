#pragma once

#include "Object.h"
#include "Bullet.h"

class World;

class Gun : public Object
{
	World *world;
	Uint32 next_fire;
public:
	int InitialArmour;
	int Armour;
	int Orient;
	Uint32 Period;
	Gun(World *w, int x, int y, int orient, int armo, Uint32 period);

	virtual void draw(SDL_Surface *s);
	virtual void think();
	virtual int type();
	virtual bool hit_by(Bullet *b);
	void fire();
};
