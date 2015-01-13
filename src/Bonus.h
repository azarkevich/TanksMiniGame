#pragma once

#include "Object.h"

//class World;
class Tank;

#define BONUS_SUPER_BULLET 0
#define BONUS_HEAL 1
#define BONUS_SPEED 2

class Bonus : public Object
{
	//World *world;
	int bonus_type;
public:
	Bonus(int x, int y, int bonus_type);

	virtual void draw(SDL_Surface *s);
	virtual void think();
	virtual int type();
	
	void action(Tank *t);
};
