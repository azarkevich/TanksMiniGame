#include "StdAfx.h"
#include "Tank.h"
#include "World.h"
#include "Bullet.h"

Tank::Tank(World *w, int x, int y)
{
	X = x;
	Y = y;
	_world = w;
	orients[0] = new Sprite();
	orients[0]->load(w->tiles, "resources/tank-a-up.sprite");
	orients[1] = new Sprite();
	orients[1]->load(w->tiles, "resources/tank-a-down.sprite");
	orients[2] = new Sprite();
	orients[2]->load(w->tiles, "resources/tank-a-left.sprite");
	orients[3] = new Sprite();
	orients[3]->load(w->tiles, "resources/tank-a-right.sprite");

	sprite = orients[ORIENT_UP];
	_orient = ORIENT_UP;
}

Tank::~Tank()
{
}
void Tank::fire()
{
	Bullet *b = new Bullet(_world, _orient);
	b->set_emission_point(X, Y);
	_world->add_bullet(b);
}
