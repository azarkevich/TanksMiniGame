#include "StdAfx.h"
#include "Tank.h"
#include "World.h"
#include "Bullet.h"

Tank::Tank(World *w, int x, int y, int armo)
{
	X = x;
	Y = y;
	_world = w;
	_orients[ORIENT_UP] = new Sprite();
	_orients[ORIENT_UP]->load(w->tiles, "resources/tank-a-up.sprite");
	_orients[ORIENT_DOWN] = new Sprite();
	_orients[ORIENT_DOWN]->load(w->tiles, "resources/tank-a-down.sprite");
	_orients[ORIENT_LEFT] = new Sprite();
	_orients[ORIENT_LEFT]->load(w->tiles, "resources/tank-a-left.sprite");
	_orients[ORIENT_RIGHT] = new Sprite();
	_orients[ORIENT_RIGHT]->load(w->tiles, "resources/tank-a-right.sprite");

	_orients_bb[ORIENT_UP] = BounceBox(4, 2, 24, 28);
	_orients_bb[ORIENT_DOWN] = BounceBox(4, 2, 24, 28);
	_orients_bb[ORIENT_LEFT] = BounceBox(2, 4, 28, 24);
	_orients_bb[ORIENT_RIGHT] = BounceBox(2, 4, 28, 24);

	Orient = ORIENT_UP;
	sprite = _orients[Orient];
	BBox = _orients_bb[Orient];
	InitialArmour = Armour = armo;
}

void Tank::fire()
{
	int ex = X, ey = Y;
	if(Orient == ORIENT_UP || Orient == ORIENT_DOWN)
		ex = X + BBox.x + BBox.w/2;

	if(Orient == ORIENT_LEFT || Orient == ORIENT_RIGHT)
		ey = Y + BBox.y + BBox.h/2;

	if(Orient == ORIENT_DOWN)
		ey += BBox.h;

	if(Orient == ORIENT_RIGHT)
		ex += BBox.w;

	Bullet *b = new Bullet(_world, Orient, ex, ey, this);
	_world->add_object(b);
}

void Tank::draw(SDL_Surface *s)
{
	sprite->draw(s, X, Y);

	SDL_Rect r;
	r.x = X - 5;
	r.y = Y - 5;
	r.w = 5 + (InitialArmour * 5);
	r.h = 4;
	SDL_FillRect(s, &r, SDL_MapRGB(s->format, 0, 0, 0));
	r.x++;
	r.y++;
	r.h-=2;
	r.w-=2;
	SDL_FillRect(s, &r, SDL_MapRGB(s->format, 255, 0, 0));
	if(Armour > 0 && InitialArmour > 0)
	{
		r.w = r.w * (1.0 * Armour / InitialArmour);
		SDL_FillRect(s, &r, SDL_MapRGB(s->format, 0, 255, 0));
	}
}

void Tank::think()
{
	sprite->think();
}

int Tank::type()
{
	return OBJ_TANK;
}

void Tank::move_to(int orient)
{
	if(move_info != NULL && ((Orient == ORIENT_UP && orient == ORIENT_DOWN) ||
		(Orient == ORIENT_DOWN && orient == ORIENT_UP) ||
		(Orient == ORIENT_LEFT && orient == ORIENT_RIGHT) ||
		(Orient == ORIENT_RIGHT && orient == ORIENT_LEFT)))
	{
		// stop on opposite movement
		stop();
		return;
	}

	Orient = orient;

	if(move_info != NULL)
	{
		delete move_info;
		move_info = NULL;
	}
	move_info = new MoveInfo(32, Orient);

	sprite = _orients[Orient];
	sprite->play(true);

	BBox = _orients_bb[Orient];
}

void Tank::stop()
{
	sprite->set_state(SPRITE_STOP);
	if(move_info != NULL)
	{
		delete move_info;
		move_info = NULL;
	}
}

bool Tank::hit_by(Bullet *b)
{
	if(Armour == 0)
	{
		remove_delay(2000);
		BounceBox b = BBox;
		b.x += X;
		b.y += Y;
		_world->add_explode_area(b, 10);
		BBox = BounceBox();
	}
	else
	{
		Armour--;
		_world->add_explode(b->X, b->Y);
	}
	return true;
}
