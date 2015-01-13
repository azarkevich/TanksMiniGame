#include "StdAfx.h"
#include "Bush.h"
#include "World.h"

Bush::Bush(int x, int y)
{
	sprite = new Sprite();
	sprite->load(TilesCache::main, "resources/bush.sprite");
	sprite->play(false);
	X = x;
	Y = y;
}
