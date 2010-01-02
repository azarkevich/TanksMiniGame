#pragma once

#include "Object.h"
#include "Bullet.h"

class World;

#define TANK_PLAYER 0
#define TANK_ENIMY 1

class Tank : public Object
{
	World *_world;
	Sprite *_orients[4];
	BounceBox _orients_bb[4];
public:
	int BulletArmour;
	int InitialArmour;
	int Armour;
	int Orient;
	Tank(World *w, int tank_type, int x, int y, int armo);

	virtual void draw(SDL_Surface *s);
	virtual void think();
	virtual int type();
	virtual bool hit_by(Bullet *b);
	void move_to(int orient);
	void stop();
	void fire();
};
