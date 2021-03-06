#pragma once

#include "Object.h"
#include "Bullet.h"

class World;

#define TANK_PLAYER 0
#define TANK_ENIMY 1

class Tank : public Object
{
	World *_world;
	Sprite *_orients[4];
	BounceBox _orients_bb[4];
	Object *target;
	void think_strategey();
	bool is_can_goto(int row, int col);
	void show_wave(int step, vector< vector<int > > &paths);
	vector< vector<int > > paths;
	bool get_path_to(int row, int col, deque< pair<int, int> > &v);
	pair<int, int> get_prev_step(unsigned int row, unsigned int col);
	void calc_paths();
	deque< pair<int,int> > path_to_target;
	Uint32 next_fire_at;
	void think_fire();

public:
	Uint32 WeaponReloadTime;
	int BulletArmour;
	int InitialArmour;
	int Armour;
	int Orient;
	int TankSpeed;
	Tank(World *w, int tank_type, int x, int y, int armo);
	~Tank();

	void init();

	virtual void draw(SDL_Surface *s);
	virtual void think();
	virtual int type();
	virtual bool hit_by(Bullet *b);
	void move_to(int orient);
	void stop();
	void fire();
};
