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
	sprite->load(w->tiles, "resources/wall.sprite");
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
	_world->add_explode(b->X + rand() % 2 - 1, b->Y + rand() % 2 - 1);
	if(sprite->get_state() == SPRITE_STOP)
		sprite->play(false);

	return true;
}

int Wall::type()
{
	return OBJ_WALL;
}

void Wall::think()
{
	if(sprite->get_state() == SPRITE_DONE)
		Removed = true;
	sprite->think();
}
