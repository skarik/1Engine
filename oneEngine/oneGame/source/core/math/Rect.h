
#ifndef _C_RECT_
#define _C_RECT_

// Includes
#include "core/types/types.h"
#include "Vector2d.h"

// Class
class Rect
{
public:
	Vector2d pos;
	Vector2d size;

public:
	Rect ( const Vector2d& n_pos, const Vector2d& n_size )
	{
		pos		= n_pos;
		size	= n_size;
	}
	Rect ( const Real x, const Real y, const Real w, const Real h )
	{
		pos.x	= x;
		pos.y	= y;
		size.x	= w;
		size.y	= h;
	}
	Rect ( void )
	{
		pos.x	= 0;
		pos.y	= 0;
		size.x	= 0;
		size.y	= 0;
	}

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

	//		Expand ( point )
	// Expands rect to contain point
	FORCE_INLINE void Expand ( const Vector2d& point )
	{
		for ( uint i = 0; i < 2; ++i )
		{
			if ( point[i] < pos[i] )
			{
				size[i] += pos[i] - point[i];
				pos[i] = point[i];
			}
			else if( point[i] > pos[i] + size[i] )
			{
				size[i] += point[i] - (pos[i] + size[i]);
			}
		}
	}
	//		Expand ( rect )
	// Expands rect to contain rect
	FORCE_INLINE void Expand ( const Rect& rect )
	{
		Expand( rect.pos );
		Expand( rect.pos + rect.size );
	}

};

#endif