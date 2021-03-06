#include "StdAfx.h"
#include "Explode.h"
#include "World.h"

Explode::Explode(int x, int y)
{
	X = x;
	Y = y;
	sprite = new Sprite();
	sprite->load(TilesCache::main, "resources/explode.sprite");
	
	for(unsigned int i=0;i<sprite->frames.size();i++)
		SDL_SetAlpha(sprite->frames[i], SDL_SRCALPHA, 200);

	sprite->play(false);
}
