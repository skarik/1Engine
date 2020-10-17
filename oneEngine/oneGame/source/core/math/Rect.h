#ifndef CORE_MATH_RECT_H_
#define CORE_MATH_RECT_H_

#include "core/types/types.h"
#include "Vector2.h"

class Rect
{
public:
	Vector2f pos;
	Vector2f size;

public:
	Rect ( const Vector2f& n_pos, const Vector2f& n_size )
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
	FORCE_INLINE bool Contains ( const Vector2f& point )
	{
		return ((point.x > pos.x) && (point.y > pos.y) && (point.x < pos.x+size.x) && (point.y < pos.y+size.y));
	}

	//		Expand ( point )
	// Expands rect to contain point
	FORCE_INLINE void Expand ( const Vector2f& point )
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

public:

	//		FromPosition ( pos1, pos2 )
	// Creates a Rect from the given positions
	FORCE_INLINE static Rect FromPosition ( Vector2f min_pos, Vector2f max_pos )
	{
		if ( min_pos.x > max_pos.x ) {
			Real temp = min_pos.x;
			min_pos.x  = max_pos.x;
			max_pos.x  = temp;
		}
		if ( min_pos.y > max_pos.y ) {
			Real temp = min_pos.y;
			min_pos.y  = max_pos.y;
			max_pos.y  = temp;
		}
		return Rect( min_pos, (max_pos-min_pos) );
	}

};

#endif//CORE_MATH_RECT_H_