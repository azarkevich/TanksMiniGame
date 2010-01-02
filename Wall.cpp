#include "StdAfx.h"
#include "Sprite.h"
#include "Object.h"
#include "Wall.h"
#include "World.h"

Wall::Wall(World *w, int x, int y)
{
	_world = w;
	sprite = new Sprite();
	sprite->load(w->tiles, "resources/wall.sprite");
	BBox = BounceBox(0, 0, 32, 32);
	X = x;
	Y = y;
}

Wall::~Wall()
{
}

void Wall::draw(SDL_Surface *s, int level)
{
	if(level != 0)
		return;
	sprite->draw(s, X, Y);
}

void Wall::weapon_hit(Object *o)
{
	_world->add_explode(o->X, o->Y);
	if(sprite->get_state() == SPRITE_STOP)
		sprite->play(false);
}

int Wall::type()
{
	return OBJ_WALL;
}

bool Wall::wait_weapon_hit()
{
	return true;
}

void Wall::think()
{
	if(sprite->get_state() == SPRITE_DONE)
		Removed = true;
	sprite->think();
}
