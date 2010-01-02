#include "StdAfx.h"
#include "Sprite.h"
#include "Object.h"
#include "Wall.h"
#include "World.h"
#include "Bullet.h"

Wall::Wall(World *w, int x, int y)
{
	_world = w;
	sprite = new Sprite();
	sprite->load(TilesCache::main, "resources/wall.sprite");
	BBox = BounceBox(0, 0, 32, 32);
	X = x;
	Y = y;
}

Wall::~Wall()
{
}

void Wall::draw(SDL_Surface *s)
{
	sprite->draw(s, X, Y);
}

bool Wall::hit_by(Bullet *b)
{
	_world->add_explode(b->X, b->Y);
	if(sprite->next() == false)
	{
		BounceBox b = BBox;
		b.x += X;
		b.y += Y;
		_world->add_explode_area(b, 5);
		BBox = BounceBox();
		remove_delay(1000);
	}

	return true;
}

int Wall::type()
{
	return OBJ_WALL;
}

void Wall::think()
{
	sprite->think();
}
