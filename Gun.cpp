#include "StdAfx.h"
#include "Gun.h"
#include "World.h"
#include "Bullet.h"

Gun::Gun(World *w, int x, int y, int orient, int armo, Uint32 period)
{
	Period = period;
	X = x;
	Y = y;
	world = w;
	Orient = orient;
	sprite = new Sprite();
	switch(Orient)
	{
		case ORIENT_UP:
			sprite->load(TilesCache::main, "resources/gun-up.sprite");
			break;
		case ORIENT_DOWN:
			sprite->load(TilesCache::main, "resources/gun-down.sprite");
			break;
		case ORIENT_LEFT:
			sprite->load(TilesCache::main, "resources/gun-up.sprite");
			break;
		case ORIENT_RIGHT:
			sprite->load(TilesCache::main, "resources/gun-up.sprite");
			break;
	}

	BBox = BounceBox(10, 4, 13, 20);
	InitialArmour = Armour = armo;
	next_fire = WorldTime::now + Period;
	sprite->begin();
	sprite->AutoReset = true;
}

void Gun::fire()
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

	Bullet *b = new Bullet(world, Orient, ex, ey, this);
	world->add_object(b);
	sprite->play(false);
}

void Gun::draw(SDL_Surface *s)
{
	sprite->draw(s, X, Y);

	if(!is_remove_pending())
	{
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
}

void Gun::think()
{
	sprite->think();
	if(next_fire <= WorldTime::now)
	{
		next_fire = WorldTime::now + Period;
		fire();
	}
}

int Gun::type()
{
	return OBJ_GUN;
}

bool Gun::hit_by(Bullet *b)
{
	if(Armour == 0)
	{
		remove();
		next_fire = numeric_limits<Uint32>::max();
		BounceBox b = BBox;
		b.x += X;
		b.y += Y;
		world->add_explode_area(b, 10);
		BBox = BounceBox();
	}
	else
	{
		Armour--;
		// stunned
		next_fire = WorldTime::now + Period * 3;
		world->add_explode(b->X, b->Y);
	}
	return true;
}
