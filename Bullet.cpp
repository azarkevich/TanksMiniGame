#include "StdAfx.h"
#include "Bullet.h"
#include "World.h"

Bullet::Bullet(World *w, int orient, int x, int y, Object *e)
{
	_world = w;
	// convert emit point
	X = x - 16;
	Y = y - 16;

	this->emiter = e;

	BBox = BounceBox(12, 12, 8, 8);

	sprite = new Sprite();
	if(orient == ORIENT_UP)
	{
		sprite->load(TilesCache::main, "resources/bullet-up.sprite");
	}
	else if(orient == ORIENT_DOWN)
	{
		sprite->load(TilesCache::main, "resources/bullet-down.sprite");
	}
	else if(orient == ORIENT_LEFT)
	{
		sprite->load(TilesCache::main, "resources/bullet-left.sprite");
	}
	else if(orient == ORIENT_RIGHT)
	{
		sprite->load(TilesCache::main, "resources/bullet-right.sprite");
	}

	sprite->play(true);

	move_info = new MoveInfo(128, orient);

	Armour = 0;
	hit_delay = 0;
}

void Bullet::set_armour(int armour)
{
	Armour = armour;
}

int Bullet::type()
{
	return OBJ_BULLET;
}

void Bullet::hit(Object *o)
{
	// Not interact with emiter. Problem in emission moment: bullet immediate hit self emitter. So need emit bullet wery carefully
	// but this is boring.
	if(o == emiter || hit_delay > 0)
		return;

	if(o->hit_by(this))
	{
		if(Armour == 0)
		{
			remove();
		}
		else
		{
			Armour--;
			hit_delay = 7;
		}
	}
}

bool Bullet::hit_by(Bullet *b)
{
	// Destroy self with explode
	_world->add_explode(X, Y);
	remove();

	// report about interaction
	return true;
}

void Bullet::draw(SDL_Surface *s)
{
	sprite->draw(s, X, Y);
}

void Bullet::think()
{
	sprite->think();
	if(hit_delay > 0)
		hit_delay--;
}
