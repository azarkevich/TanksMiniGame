#pragma once

#define ORIENT_UP 0
#define ORIENT_DOWN 1
#define ORIENT_LEFT 2
#define ORIENT_RIGHT 3

#define OBJ_NONE -1
#define OBJ_BULLET 0
#define OBJ_WALL 1
#define OBJ_TANK 2
#define OBJ_EXPLODE 3
#define OBJ_BUSH 4
#define OBJ_FLAG 5
#define OBJ_GUN 6
#define OBJ_BONUS 7

class WorldTime
{
public:
	static Uint32 now;
};

class MoveInfo
{
public:
	MoveInfo(int speed, int orient)
	{
		Speed = speed;
		Orient = orient;
		NextMoveAt = WorldTime::now + Speed;
	}

	// frames per pixel
	int Speed;
	int Orient;
	Uint32 NextMoveAt;
};

class TilesCache
{
public:
	static vector<SDL_Surface *> main;
};
