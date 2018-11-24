#ifndef _RAY_2D_H_
#define _RAY_2D_H_

#include "Vector2f.h"

class Ray2d
{
public:
	Ray2d ( const Vector2f& npos, const Vector2f& ndir ) : pos(npos), dir(ndir) {}
	Ray2d ( void ) : pos(0,0), dir(0,0) {}

	static Ray2d FromSegment ( const Vector2f& source, const Vector2f& target )
	{
		return Ray2d( source, target-source );
	}

	// return parametric value of collision of this ray
	Real CollideRay ( const Ray2d& other ) const
	{
		//Real dx = bs.x - as.x
		//Real dy = bs.y - as.y
		Vector2f delta = other.pos - pos;
		//det = bd.x * ad.y - bd.y * ad.x
		Real det = other.dir.cross(dir);
		//u = (dy * bd.x - dx * bd.y) / det
		//v = (dy * ad.x - dx * ad.y) / det
		return (delta.y * other.dir.x - delta.x * other.dir.y) / det;
	}

public:
	Vector2f pos;
	Vector2f dir;
};


#endif//_RAY_2D_H_