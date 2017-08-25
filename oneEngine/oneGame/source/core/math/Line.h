

#ifndef _STRUCT_LINE_H_
#define _STRUCT_LINE_H_

// Includes
#include "Vector3d.h"
#include "Ray.h"

// Struct definition
struct Line
{
	Vector3d start;
	Vector3d end;
public:
	Line ( void ) {
		start	= Vector3d( 0,0,0 );
		end		= Vector3d( 0,0,1 );
	}
	Line ( const Line& l ) {
		*this = l;
	}
	Line ( const Vector3d& startpoint, const Vector3d& endpoint )
	{
		start	= startpoint;
		end		= endpoint;
	}
	Line ( const Ray& ray )
	{
		start	= ray.pos;
		end		= ray.pos + ray.dir;
	}
	Vector3d midpoint ( void ) const
	{
		return ( (start+end)*0.5f );
	}
};

#endif