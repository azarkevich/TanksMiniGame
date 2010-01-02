#include "StdAfx.h"
#include "Sprite.h"

Sprite::Sprite()
{
	_current_frame = 0;
	_next_time = 0;
	Duration = 0;
}

Sprite::~Sprite()
{
}

void Sprite::think()
{
	if(_state == SPRITE_STOP || _state == SPRITE_DONE || _state == SPRITE_PAUSE)
		return;

	if(WorldTime::now >= _next_time)
	{
		_current_frame++;

		if(_current_frame >= frames.size())
		{
			if(_state == SPRITE_PLAY_ONCE)
			{
				_current_frame = frames.size();
				_state = SPRITE_DONE;
				return;
			}
			else if(_state == SPRITE_PLAY_CYCLE)
			{
				_current_frame = 0;	
			}
		}
		_next_time = WorldTime::now + Duration/frames.size();
	}
}

void Sprite::draw(SDL_Surface *s, int x, int y)
{
	if(_current_frame >= frames.size())
		return;
	SDL_Rect dest;
	dest.x = x;
	dest.y = y;

	SDL_BlitSurface(frames[_current_frame], NULL, s, &dest);
}

void Sprite::load(const vector<SDL_Surface *> &v, const char *file)
{
	ifstream ifs(file);
	if(ifs.bad())
	{
		cerr << "Can't open file " << file << endl;
		exit(1);
	}
	int n;
	ifs >> n;
	for(int i=0;i<n;i++)
	{
		int sn;
		ifs >> sn;
		frames.push_back(v[sn]);
	}
	ifs >> Duration;
	set_state(SPRITE_STOP);
}

void Sprite::set_state(int state)
{
	_state = state;
	if(_state == SPRITE_STOP)
	{
		_current_frame = 0;
	}
	else if(_state == SPRITE_PLAY_ONCE || _state == SPRITE_PLAY_CYCLE)
	{
		_current_frame = 0;
		_next_time = WorldTime::now + Duration/frames.size();
	}
	else if(_state == SPRITE_DONE)
	{
		_current_frame = frames.size();
	}
}

int Sprite::get_state()
{
	return _state;
}
