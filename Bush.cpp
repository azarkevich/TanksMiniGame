#include "StdAfx.h"
#include "Bush.h"
#include "World.h"

Bush::Bush(World *w)
{
	sprite = new Sprite();
	sprite->load(w->tiles, "resources/bush.sprite");
	sprite->play(false);
}

Bush::~Bush()
{
}
