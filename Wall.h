#pragma once

class World;

class Wall : public Object
{
	World *_world;
public:
	Wall(World *w, int x, int y);
	~Wall();

	virtual void draw(SDL_Surface *s, int level);
	virtual void think();

	virtual int type();
	virtual bool wait_weapon_hit();

	virtual void weapon_hit(Object *o);
};
