#include "StdAfx.h"
#include "BounceBox.h"
#include "Object.h"

bool BounceBox::is_inside_rect(Object *o, SDL_Rect *r)
{
	const BounceBox &bb = o->BBox;
	if((o->X + bb.x) < r->x || (o->Y + bb.y) < r->y)
		return false;
	if((o->X + bb.x + bb.w) > (r->x + r->w) || (o->Y + bb.y + bb.h) > (r->y + r->h))
		return false;

	return true;
}

bool BounceBox::is_intersect(Object *o1, Object *o2)
{
	BounceBox b1 = o1->BBox;
	b1.x += o1->X;
	b1.y += o1->Y;

	BounceBox b2 = o2->BBox;
	b2.x += o2->X;
	b2.y += o2->Y;

	bool x_intersect = (b1.x >= b2.x && b1.x < (b2.x + b2.w)) || (b2.x >= b1.x && b2.x < (b1.x + b1.w));
	if(x_intersect == false)
		return false;

	bool y_intersect = (b1.y >= b2.y && b1.y < (b2.y + b2.h)) || (b2.y >= b1.y && b2.y < (b1.y + b1.h));
	if(y_intersect == false)
		return false;

	return true;
}
