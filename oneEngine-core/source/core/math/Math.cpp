
//#include "random_vector.h"

float saturate ( float f ) {
	f = ( (f>1) ? 1 : f );
	return ( (0>f) ? 0 : f );
}
double saturate ( double f ) {
	f = ( (f>1) ? 1 : f );
	return ( (0>f) ? 0 : f );
}

#include "Math.h"

CMath Math;

Real CMath::Smoothlerp ( Real t )
{
	t = Math.Clamp( t,0,1 );
	return ( t * t * (3 - 2 * t) );
}

Real CMath::Smoothlerp ( Real t, Real a, Real b )
{
	return Lerp( Smoothlerp(t), a,b );
}
Real CMath::Lerp ( Real t, Real a, Real b )
{
	if ( t < 0 )
		return a;
	else if ( t > 1 )
		return b;
	else
		return ( a+t*(b-a) );
}
// Performs an unlimited linear interpolation
Real CMath::lerp ( Real t, Real a, Real b )
{
	return ( a+t*(b-a) );
}
Real CMath::Clamp ( Real val, Real lower, Real upper )
{
	if ( upper < lower )
	{
		Real temp = upper;
		upper = lower;
		lower = temp;
	}
	if ( val > upper ) 
		return upper;
	else if ( val < lower )
		return lower;
	else
		return val;
}
Real CMath::Wrap ( Real val, Real lower, Real upper )
{
	if ( upper < lower )
	{
		Real temp = upper;
		upper = lower;
		lower = temp;
	}
	while ( val > upper )
	{
		val -= upper-lower;
	}
	while ( val < lower )
	{
		val += upper-lower;
	}
	return val;
}

// Gives ownership of the returned array away. Remember to delete it.
Vector3d* CMath::pointsOnSphere ( int N )
{
	Vector3d* pts = new Vector3d [N];

	/*const float inc = float(PI) * ( 3 - sqrt( 5.0f ) );
	const float off = 2 / float( N );*/

	/*float y, r, phi;
	for ( int k = 0; k < N; k += 1 )
	{
		y = k*off - 1.0f + ( off/2.0f );
		r = sqrt( 1-y*y );
		phi = k * inc;
		pts[k] = Vector3d( cosf( phi )*r, y, sinf( phi ) * r ).normal();
	}*/
	float y, r, phi = 0;
	const float s = 3.6f / sqrt( float(N) );
	
	for ( int k = 1; k < N - 1; k += 1 )
	{
		y = -1.0f + ( 2.0f * k ) / ( N-1 );
		r = sqrt( 1-y*y );
		phi = phi + s / r;
		pts[k] = Vector3d( cosf(phi)*r, sinf(phi)*r, y );
	}

	//pts[0] = Vector3d( 0,0,-1 );
	//pts[N-1] = Vector3d (0,0,1);

	return pts;
}