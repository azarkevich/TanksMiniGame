#include "StdAfx.h"
#include "Bush.h"
#include "World.h"

Bush::Bush(World *w, int x, int y)
{
	sprite = new Sprite();
	sprite->load(w->tiles, "resources/bush.sprite");
	sprite->play(false);
	X = x;
	Y = y;
}

Bush::~Bush()
{
}
