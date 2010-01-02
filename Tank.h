#pragma once

#include "Object.h"

class World;

class Tank : public Object
{
	World *_world;
	Sprite *orients[4];
	int _orient;
public:
	Tank(World *w);
	~Tank();

	virtual void draw(SDL_Surface *s, int level)
	{
		if(level != 0)
			return;
		sprite->draw(s, X, Y);
	}

	virtual void think()
	{
		sprite->think();
	}

	virtual int type()
	{
		return OBJ_TANK;
	}

	void move_to(int orient)
	{
		if(move_info != NULL)
		{
			delete move_info;
			move_info = NULL;
		}
		move_info = new MoveInfo(32, orient);

		_orient = orient;

		sprite = orients[orient];
		sprite->play(true);
	}

	void stop()
	{
		sprite->set_state(SPRITE_STOP);
		if(move_info != NULL)
		{
			delete move_info;
			move_info = NULL;
		}
	}

	void fire();
};
