#include "StdAfx.h"
#include "Bonus.h"
#include "Tank.h"

static const char *sprite_file[] = {
	"resources/bonus-super-bullet.sprite",
	"resources/bonus-heal.sprite",
	"resources/bonus-speed.sprite"
};

Bonus::Bonus(int x, int y, int bonus_type)
{
	this->bonus_type = bonus_type;
	X = x;
	Y = y;
	sprite = new Sprite();
	sprite->load(TilesCache::main, sprite_file[bonus_type]);
	sprite->play(true);
	BBox.x = 6;
	BBox.y = 6;
	BBox.w = 18;
	BBox.h = 18;
}

void Bonus::draw(SDL_Surface *s)
{
	sprite->draw(s, X, Y);
}

void Bonus::think()
{
	sprite->think();
}

int Bonus::type()
{
	return OBJ_BONUS;
}

void Bonus::action(Tank *t)
{
	if(bonus_type == BONUS_SUPER_BULLET)
	{
		t->BulletArmour++;
	}
	else if(bonus_type == BONUS_HEAL)
	{
		t->Armour = t->InitialArmour;
	}
	else if(bonus_type == BONUS_SPEED)
	{
		t->TankSpeed *= 2;
		if(t->move_info != NULL)
			t->move_to(t->Orient);
	}
	remove();
}