
#ifndef _C_RECT_
#define _C_RECT_

// Includes
#include "core/types/types.h"
#include "Vector2d.h"

// Class
class Rect
{
public:
	Rect ( Real x, Real y, Real w, Real h )
	{
		pos.x = x;
		pos.y = y;
		size.x = w;
		size.y = h;
	}
	Rect ( void )
	{
		pos.x = 0;
		pos.y = 0;
		size.x = 0;
		size.y = 0;
	}

	Vector2d pos;
	Vector2d size;

	//		Fix ()
	// Changes the rect bounds and position so that size is positive on both components
	FORCE_INLINE void Fix ( void )
	{
		if ( size.x < 0 ) {
			pos.x += size.x;
			size.x = -size.x;
		}	
		if ( size.y < 0 ) {
			pos.y += size.y;
			size.y = -size.y;
		}	
	}

	//		Contains ( point )
	// Checks if point is inside the given rect
	FORCE_INLINE bool Contains ( const Vector2d& point )
	{
		return ((point.x > pos.x) && (point.y > pos.y) && (point.x < pos.x+size.x) && (point.y < pos.y+size.y));
	}
};

#endif