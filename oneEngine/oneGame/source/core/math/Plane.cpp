#include "Plane.h"

using namespace core::math;

// Constructors
Plane::Plane ( void )
{
	n = Vector3f( 0,0,1 );
	d = 0;
}

Plane::Plane ( Vector3f * pPoint )
{
	n = (pPoint[1]-pPoint[0]).cross(pPoint[2]-pPoint[0]);
	d = -pPoint[1].x*n.x-pPoint[1].y*n.y-pPoint[1].z*n.z;
}

void Plane::ConstructFromPoints ( Vector3f * pPoint )
{
	n = (pPoint[1]-pPoint[0]).cross(pPoint[2]-pPoint[0]);
	d = -pPoint[1].x*n.x-pPoint[1].y*n.y-pPoint[1].z*n.z;
}

void Plane::ConstructFromPoints ( Vector3f const& point0, Vector3f const& point1, Vector3f const& point2 )
{
	n = (point1-point0).cross(point2-point0);
	d = -point1.x*n.x-point1.y*n.y-point1.z*n.z;
}
