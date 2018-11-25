

#ifndef _STRUCT_LINE_H_
#define _STRUCT_LINE_H_

// Includes
#include "Vector3.h"
#include "Ray.h"

// Struct definition
struct Line
{
	Vector3f start;
	Vector3f end;
public:
	Line ( void ) {
		start	= Vector3f( 0,0,0 );
		end		= Vector3f( 0,0,1 );
	}
	Line ( const Line& l ) {
		*this = l;
	}
	Line ( const Vector3f& startpoint, const Vector3f& endpoint )
	{
		start	= startpoint;
		end		= endpoint;
	}
	Line ( const Ray& ray )
	{
		start	= ray.pos;
		end		= ray.pos + ray.dir;
	}
	Vector3f midpoint ( void ) const
	{
		return ( (start+end)*0.5f );
	}
};

#endif