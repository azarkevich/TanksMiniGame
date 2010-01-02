#include "StdAfx.h"
#include "Flag.h"
#include "World.h"

Flag::Flag(World *w, int x, int y)
{
	world = w;
	X = x;
	Y = y;
	sprite = new Sprite();
	sprite->load(TilesCache::main, "resources/flag.sprite");
	BBox = BounceBox(1, 3, 30, 26);
	Dead = false;
}

bool Flag::hit_by(Bullet *b)
{
	Dead = true;
	sprite->next();
	BounceBox bbox = BBox;
	bbox.x += X;
	bbox.y += Y;
	world->add_explode_area(bbox, 10);
	BBox = BounceBox();
	return true;
}
