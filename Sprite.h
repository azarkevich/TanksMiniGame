#pragma once

#define SPRITE_STOP 0
#define SPRITE_PLAY_CYCLE 1
#define SPRITE_PLAY_ONCE 2
#define SPRITE_PAUSE 3
#define SPRITE_DONE 4

#include "Common.h"

class Sprite
{
	Uint32 _next_time;
	unsigned int _current_frame;
	int _state;
	bool _cycle;
public:
	vector<SDL_Surface *> frames;
	int Duration;

	Sprite();
	~Sprite();

	void think();

	void draw(SDL_Surface *s, int x, int y);

	void set_state(int state);
	int get_state();

	void play(bool cycle) { set_state(cycle ? SPRITE_PLAY_CYCLE : SPRITE_PLAY_ONCE); }

	void load(const vector<SDL_Surface *> &v, const char *file);
};
