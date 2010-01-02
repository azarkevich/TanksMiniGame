#pragma once

#define ORIENT_UP 0
#define ORIENT_DOWN 1
#define ORIENT_LEFT 2
#define ORIENT_RIGHT 3

#define OBJ_BULLET 0
#define OBJ_WALL 1
#define OBJ_TANK 2
#define OBJ_EXPLODE 3
#define OBJ_BUSH 4
#define OBJ_FLAG 5
#define OBJ_GUN 6

class WorldTime
{
public:
	static Uint32 now;
};

class MoveInfo
{
public:
	MoveInfo(double vel, int orient)
	{
		_last_calc = WorldTime::now;
		delta = 0;

		Velocity = vel;
		Orient = orient;
	}

	// pixels per second
	double Velocity;			
	int Orient;
	Uint32 _last_calc;
	double delta;
};

class TilesCache
{
public:
	static vector<SDL_Surface *> main;
};
