
//#include "random_vector.h"
#include "Math.h"

// Gives ownership of the returned array away. Remember to delete it.
Vector3f* math::points_on_sphere ( int N )
{
	Vector3f* pts = new Vector3f [N];

	float y, r, phi = 0;
	const float s = 3.6f / sqrt( float(N) );
	
	for ( int k = 1; k < N - 1; k += 1 )
	{
		y = -1.0f + ( 2.0f * k ) / ( N-1 );
		r = sqrt( 1-y*y );
		phi = phi + s / r;
		pts[k] = Vector3f( cosf(phi)*r, sinf(phi)*r, y );
	}

	return pts;
}