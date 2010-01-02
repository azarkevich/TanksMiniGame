#include "StdAfx.h"
#include "Bullet.h"
#include "World.h"

Bullet::Bullet(World *w, int orient)
{
	_world = w;
	BBox = BounceBox(12, 12, 8, 8);

	sprite = new Sprite();
	if(orient == ORIENT_UP)
	{
		sprite->load(w->tiles, "resources/bullet-up.sprite");
	}
	else if(orient == ORIENT_DOWN)
	{
		sprite->load(w->tiles, "resources/bullet-down.sprite");
	}
	else if(orient == ORIENT_LEFT)
	{
		sprite->load(w->tiles, "resources/bullet-left.sprite");
	}
	else if(orient == ORIENT_RIGHT)
	{
		sprite->load(w->tiles, "resources/bullet-right.sprite");
	}

	sprite->play(true);

	move_info = new MoveInfo(128, orient);
}

void Bullet::weapon_hit(Object *o)
{
	_world->add_explode(o->X, o->Y);
	_world->add_explode(X, Y);
	Removed = true;
}

