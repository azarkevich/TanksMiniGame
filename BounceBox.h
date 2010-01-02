#pragma once

class Object;

class BounceBox : public SDL_Rect
{
public:
	BounceBox()
	{
		this->x = 0;
		this->y = 0;
		this->w = 0;
		this->h = 0;
	}
	//int x, y, w, h;
	BounceBox(int x, int y, int w, int h)
	{
		this->x = x;
		this->y = y;
		this->w = w;
		this->h = h;
	}

	static bool is_inside_rect(Object *o, SDL_Rect *r);
	static bool is_intersect(Object *o1, Object *o2);
};
