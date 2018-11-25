
#ifndef _C_MATRIX_FUNCTIONS_2_
#define _C_MATRIX_FUNCTIONS_2_

// Includes
#include "CMatrix.h"

inline Matrix3x3 Matrix4x4::getRotator ( void ) const
{
	Matrix3x3 result;

	result.pData[0] = this->pData[0];
	result.pData[1] = this->pData[1];
	result.pData[2] = this->pData[2];

	result.pData[3] = this->pData[4];
	result.pData[4] = this->pData[5];
	result.pData[5] = this->pData[6];

	result.pData[6] = this->pData[8];
	result.pData[7] = this->pData[9];
	result.pData[8] = this->pData[10];

	return result;
}

// Settin the rotation
inline bool Matrix3x3::setRotation ( Real const angle_x, Real const angle_y, Real const angle_z )
{
	Real ax = (Real)degtorad(angle_x);
	Real ay = (Real)degtorad(angle_y);
	Real az = (Real)degtorad(angle_z);
	Real A       = cos(ax);
    Real B       = sin(ax);
    Real C       = cos(ay);
    Real D       = sin(ay);
    Real E       = cos(az);
    Real F       = sin(az);
    Real AD      =   A * D;
    Real BD      =   B * D;

    pData[0]  =   C * E;
    pData[1]  =  -C * F;
    pData[2]  =   D;

    pData[3]  =  BD * E + A * F;
    pData[4]  = -BD * F + A * E;
    pData[5]  =  -B * C;

    pData[6]  = -AD * E + B * F;
    pData[7]  =  AD * F + B * E;
    pData[8]  =   A * C;

	return true;
}

// Setting the rotation of a 3d matrix via 3d vector
inline bool Matrix3x3::setRotation ( Vector3f const& vect )
{
	return setRotation( vect.x, vect.y, vect.z );
}
// Setting rotation of a 3d matrix via a quaternion
inline bool Matrix3x3::setRotation ( Quaternion const& quat )
{
	Real xx      = quat.x * quat.x;
    Real xy      = quat.x * quat.y;
    Real xz      = quat.x * quat.z;
    Real xw      = quat.x * quat.w;
    Real yy      = quat.y * quat.y;
    Real yz      = quat.y * quat.z;
    Real yw      = quat.y * quat.w;
    Real zz      = quat.z * quat.z;
    Real zw      = quat.z * quat.w;
    /*pData[0]  = 1 - 2 * ( yy + zz );
    pData[1]  =     2 * ( xy - zw );
    pData[2]  =     2 * ( xz + yw );

    pData[3]  =     2 * ( xy + zw );
    pData[4]  = 1 - 2 * ( xx + zz );
    pData[5]  =     2 * ( yz - xw );

    pData[6]  =     2 * ( xz - yw );
    pData[7]  =     2 * ( yz + xw );
    pData[8]  = 1 - 2 * ( xx + yy );*/
	pData[0]  = 1 - 2 * ( yy + zz );
    pData[3]  =     2 * ( xy - zw );
    pData[6]  =     2 * ( xz + yw );

    pData[1]  =     2 * ( xy + zw );
    pData[4]  = 1 - 2 * ( xx + zz );
    pData[7]  =     2 * ( yz - xw );

    pData[2]  =     2 * ( xz - yw );
    pData[5]  =     2 * ( yz + xw );
    pData[8]  = 1 - 2 * ( xx + yy );

	return true;
}
// Getters - Rotation
inline Vector3f Matrix3x3::getEulerAngles ( void ) const
{
	float tr_x, tr_y, D,C;
	Vector3f angle;

	angle.y = D =  asin( min<Real>(max<Real>(pData[2],-1.0f),1.0f) );        /* Calculate Y-axis angle */
    C           =  cos( angle.y );
    angle.y		=  (Real)radtodeg(angle.y);//*=  RADIANS;
    if ( fabs( C ) > 0.005 )             /* Gimball lock? */
    {
		tr_x      =  pData[8] / C;           /* No, so get X-axis angle */
		tr_y      = -pData[5] / C;
		angle.x  = (Real)radtodeg(atan2( tr_y, tr_x ));
		tr_x      =  pData[0] / C;            /* Get Z-axis angle */
		tr_y      = -pData[1] / C;
		angle.z  = (Real)radtodeg(atan2( tr_y, tr_x ));
    }
    else                                 /* Gimball lock has occurred */
    {
		angle.x  = 0;                      /* Set X-axis angle to zero */
		tr_x      =  pData[4];                 /* And calculate Z-axis angle */
		tr_y      =  pData[3];
		angle.z  = (Real)radtodeg(atan2( tr_y, tr_x ));
    }

    /* return only positive angles in [0,360] */
    /*if (angle.x < 0) angle.x += 360;
    if (angle.y < 0) angle.y += 360;
    if (angle.z < 0) angle.z += 360;*/

	/* check for invalid values */
	assert( angle.x == angle.x );
	assert( angle.x <= FLT_MAX && angle.x >= -FLT_MAX );
	assert( angle.y == angle.y );
	assert( angle.y <= FLT_MAX && angle.y >= -FLT_MAX );
	assert( angle.z == angle.z );
	assert( angle.z <= FLT_MAX && angle.z >= -FLT_MAX );

	return angle;
}

// Convert the rotation into a quaternion
inline Quaternion Matrix3x3::getQuaternion ( void ) const
{
	Real S, X, Y, Z, W;

	Real tr = pData[0] + pData[4] + pData[8];
	if ( tr > 0 ) { 
		S = sqrt(tr+1.0f) * 2; // S=4*qw 
		W = 0.25f * S;
		X = (pData[7] - pData[5]) / S;
		Y = (pData[2] - pData[6]) / S; 
		Z = (pData[3] - pData[1]) / S; 
	}
	else if ( pData[0] > pData[4] && pData[0] > pData[8] ) 
	{	// Column 0: 
        S  = (Real)sqrt( 1.0f + pData[0] - pData[4] - pData[8] ) * 2;
        X = 0.25f * S;
        Y = (pData[3] + pData[1]) / S;
        Z = (pData[2] + pData[6]) / S;
        W = (pData[7] - pData[5]) / S;
    }
	else if ( pData[4] > pData[8] )
	{	// Column 1: 
        S  = (Real)sqrt( 1.0f + pData[4] - pData[0] - pData[8] ) * 2;
        X = (pData[3] + pData[1]) / S;
        Y = 0.25f * S;
        Z = (pData[7] + pData[5]) / S;
        W = (pData[2] - pData[6]) / S;
    }
	else
	{	// Column 2:
        S  = (Real)sqrt( 1.0f + pData[8] - pData[0] - pData[4] ) * 2;
        X = (pData[2] + pData[6]) / S;
        Y = (pData[7] + pData[5]) / S;
        Z = 0.25f * S;
        W = (pData[3] - pData[1]) / S;
    }
	return Quaternion( X,Y,Z,W );
}

// Multiply by a vector
inline Vector3f Matrix3x3::operator* ( Vector3f const& vect ) const
{
	/*return Vector3f(
		pData[0]*vect.x+pData[1]*vect.y+pData[2]*vect.z+pData[3],
		pData[4]*vect.x+pData[5]*vect.y+pData[6]*vect.z+pData[7],
		pData[8]*vect.x+pData[9]*vect.y+pData[10]*vect.z+pData[11]
		);*/
	/*return Vector3f(
		pData[0]*vect.x+pData[4]*vect.y+pData[8]*vect.z+pData[3],
		pData[1]*vect.x+pData[5]*vect.y+pData[9]*vect.z+pData[7],
		pData[2]*vect.x+pData[6]*vect.y+pData[10]*vect.z+pData[11]
		);*/
	return Vector3f(
		pData[0]*vect.x+pData[3]*vect.y+pData[6]*vect.z,
		pData[1]*vect.x+pData[4]*vect.y+pData[7]*vect.z,
		pData[2]*vect.x+pData[5]*vect.y+pData[8]*vect.z
		);
	/*return Vector3f(
		pData[0]*vect.x+pData[4]*vect.y+pData[8]*vect.z+pData[12],
		pData[1]*vect.x+pData[5]*vect.y+pData[9]*vect.z+pData[13],
		pData[2]*vect.x+pData[6]*vect.y+pData[10]*vect.z+pData[14]
		);*/
}

#endif