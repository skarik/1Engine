
//#include "random_vector.h"
#include "Math.h"

// Gives ownership of the returned array away. Remember to delete it.
Vector3d* Math::points_on_sphere ( int N )
{
	Vector3d* pts = new Vector3d [N];

	float y, r, phi = 0;
	const float s = 3.6f / sqrt( float(N) );
	
	for ( int k = 1; k < N - 1; k += 1 )
	{
		y = -1.0f + ( 2.0f * k ) / ( N-1 );
		r = sqrt( 1-y*y );
		phi = phi + s / r;
		pts[k] = Vector3d( cosf(phi)*r, sinf(phi)*r, y );
	}

	return pts;
}