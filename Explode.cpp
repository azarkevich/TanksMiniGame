#include "StdAfx.h"
#include "Explode.h"
#include "World.h"

Explode::Explode(World *w, int x, int y)
{
	X = x;
	Y = y;
	sprite = new Sprite();
	sprite->load(w->tiles, "resources/explode.sprite");
	sprite->play(false);
}

Explode::~Explode()
{
}
