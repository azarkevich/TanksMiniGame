#include "StdAfx.h"
#include "Object.h"

Object::Object()
{
	sprite = NULL;
	move_info = NULL;
	Removed = false;
}

Object::~Object()
{
	if(sprite != NULL)
		delete sprite;
}
