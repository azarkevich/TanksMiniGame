#include "StdAfx.h"
#include "Explode.h"
#include "World.h"

Explode::Explode(int x, int y)
{
	X = x;
	Y = y;
	sprite = new Sprite();
	sprite->load(TilesCache::main, "resources/explode.sprite");
	sprite->play(false);
}
