#include "StdAfx.h"
#include "Object.h"

Object::Object()
{
	sprite = NULL;
	move_info = NULL;
	remove_at = numeric_limits<Uint32>::max();
}

Object::~Object()
{
	if(sprite != NULL)
		delete sprite;
}

bool Object::hit_by(Bullet *b)
{
	return false;
}

void Object::remove(Uint32 at)
{
	remove_at = at;
}

void Object::remove_delay(Uint32 delay)
{
	remove_at = WorldTime::now + delay;
}

bool Object::is_removed()
{
	return (WorldTime::now >= remove_at);
}

bool Object::is_remove_pending()
{
	return (remove_at != numeric_limits<Uint32>::max());
}
