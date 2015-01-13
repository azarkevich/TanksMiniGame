#include "StdAfx.h"
#include "Sprite.h"
#include "Object.h"
#include "Wall.h"
#include "World.h"
#include "Bullet.h"

static const char *wall_types[] = {
	"resources/wall-a.sprite",
	"resources/wall-b.sprite",
	"resources/wall-c.sprite",
	"resources/wall-d.sprite"
};

static bool destroyable[] = {
	true,
	true,
	false,
	false,
};

Wall::Wall(World *w, int x, int y, int wall_type)
{
	_world = w;
	this->wall_type = wall_type;
	sprite = new Sprite();
	sprite->load(TilesCache::main, wall_types[wall_type]);
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
	if(destroyable[wall_type])
	{
		if(sprite->next() == false)
		{
			BounceBox b = BBox;
			b.x += X;
			b.y += Y;
			_world->add_explode_area(b, 5);
			BBox = BounceBox();
			remove();
		}
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
