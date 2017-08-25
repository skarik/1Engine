
#include "Random.h"
#include "core/math/Vector2d.h"
#include "core/math/Vector3d.h"
#include <cmath>

CRandom Random;

Real CRandom::Range ( Real min, Real max )
{
	return min + (Random.Next() / (Real)Random.Max()) * ( max - min );
}

Vector3d CRandom::PointOnUnitSphere ( void )
{
	Real angle1 = Range( 0, (Real) (2*PI) );
	Real angle2 = Range( (Real) -PI/2,(Real) PI/2 );

	Real fcosz	= cos( angle2 );

	Vector3d result;
	result.z = sin( angle2 );
	result.x = sin( angle1 ) * fcosz;
	result.y = cos( angle1 ) * fcosz;

	return result;
}

Vector3d CRandom::PointInUnitSphere ( void )
{
	return ( PointOnUnitSphere() * Range(0,1) );
}

Vector2d CRandom::PointOnUnitCircle ( void )
{
	Real angle = Range( 0, (Real) (2*PI) );
	
	Vector2d result;
	result.x = sin( angle );
	result.y = cos( angle );

	return result;
}

bool CRandom::Chance ( Real test_value )
{
	return (Range(0,1) < test_value);
}