#pragma once

#include "Object.h"
#include "Common.h"

class World;

class Bullet : public Object
{
	World *_world;
	Object *emiter;
	int hit_delay;
public:
	// armoured bullet not destroy until Armour == 0
	int Armour;

	Bullet(World *w, int orient, int x, int y, Object *emiter);
	
	void set_armour(int armour);

	virtual void draw(SDL_Surface *s);
	virtual void think();
	virtual int type();
	virtual bool hit_by(Bullet *b);
	void hit(Object *o);
};
