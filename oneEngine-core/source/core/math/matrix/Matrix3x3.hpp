#include <algorithm>
#include "CMatrix.h"
#include "string.h"

FORCE_INLINE Matrix3x3::Matrix3x3 ( void )
{
	pData[0] = 1.0F;
	pData[4] = 1.0F;
	pData[8] = 1.0F;

	pData[1] = 0.0F; pData[2] = 0.0F;
	pData[3] = 0.0F; pData[5] = 0.0F;
	pData[6] = 0.0F; pData[7] = 0.0F;
}
FORCE_INLINE Matrix3x3::Matrix3x3 ( const Matrix3x3& nSrc )
{
	memcpy( pData, nSrc.pData, 9*sizeof(Real) );
}
FORCE_INLINE Matrix3x3::Matrix3x3 ( const Real* nData )
{
	memcpy( pData, nData, 9*sizeof(Real) );
}

// Setting scale via FTYPE
FORCE_INLINE void Matrix3x3::setScale ( const Real x, const Real y, const Real z )
{
	pData[0] = x;
	pData[4] = y;
	pData[8] = z;
}
// Settin the scaling via Vector3D
FORCE_INLINE void Matrix3x3::setScale ( const Vector3d& vect )
{
	pData[0] = vect.x;
	pData[4] = vect.y;
	pData[8] = vect.z;
}

// Settin the rotation
FORCE_INLINE void Matrix3x3::setRotation ( const Real angle_x, const Real angle_y, const Real angle_z )
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
    pData[3]  =  -C * F;
    pData[6]  =   D;

    pData[1]  =  BD * E + A * F;
    pData[4]  = -BD * F + A * E;
    pData[7]  =  -B * C;

    pData[2]  = -AD * E + B * F;
    pData[5]  =  AD * F + B * E;
    pData[8]  =   A * C;
}
FORCE_INLINE void Matrix3x3::setRotationZYX ( const Real angle_x, const Real angle_y, const Real angle_z )
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
}


// Setting the rotation of a 3d matrix via 3d vector
FORCE_INLINE void Matrix3x3::setRotation ( const Vector3d& vect )
{
	return setRotation( vect.x, vect.y, vect.z );
}
// Setting rotation of a 3d matrix via a quaternion
FORCE_INLINE void Matrix3x3::setRotation ( const Quaternion& quat )
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
    pData[0]  = 1 - 2 * ( yy + zz );
    pData[1]  =     2 * ( xy - zw );
    pData[2]  =     2 * ( xz + yw );

    pData[3]  =     2 * ( xy + zw );
    pData[4]  = 1 - 2 * ( xx + zz );
    pData[5]  =     2 * ( yz - xw );

    pData[6]  =     2 * ( xz - yw );
    pData[7]  =     2 * ( yz + xw );
    pData[8]  = 1 - 2 * ( xx + yy );
	/*pData[0]  = 1 - 2 * ( yy + zz );
    pData[3]  =     2 * ( xy - zw );
    pData[6]  =     2 * ( xz + yw );

    pData[1]  =     2 * ( xy + zw );
    pData[4]  = 1 - 2 * ( xx + zz );
    pData[7]  =     2 * ( yz - xw );

    pData[2]  =     2 * ( xz - yw );
    pData[5]  =     2 * ( yz + xw );
    pData[8]  = 1 - 2 * ( xx + yy );*/
}
// Copy this shiet
FORCE_INLINE void Matrix3x3::setRotation ( const Matrix3x3& nMatx )
{
	memcpy( pData, nMatx.pData, 9*sizeof(Real) );
}

// Getters - Rotation
FORCE_INLINE Vector3d Matrix3x3::getEulerAngles ( void ) const
{
	float tr_x, tr_y, D,C;
	Vector3d angle;

	//angle.y = D =  asin( min<Real>(max<Real>(pData[2],-1.0f),1.0f) );        /* Calculate Y-axis angle */
 //   C           =  cos( angle.y );
 //   angle.y		=  (Real)radtodeg(angle.y);//*=  RADIANS;
 //   if ( fabs( C ) > 0.005 )             /* Gimball lock? */
 //   {
	//	tr_x      =  pData[8] / C;           /* No, so get X-axis angle */
	//	tr_y      = -pData[5] / C;
	//	angle.x  = (Real)radtodeg(atan2( tr_y, tr_x ));
	//	tr_x      =  pData[0] / C;            /* Get Z-axis angle */
	//	tr_y      = -pData[1] / C;
	//	angle.z  = (Real)radtodeg(atan2( tr_y, tr_x ));
 //   }
 //   else                                 /* Gimball lock has occurred */
 //   {
	//	angle.x  = 0;                      /* Set X-axis angle to zero */
	//	tr_x      =  pData[4];                 /* And calculate Z-axis angle */
	//	tr_y      =  pData[3];
	//	angle.z  = (Real)radtodeg(atan2( tr_y, tr_x ));
 //   }
	angle.y = D =  asin( std::min<Real>(std::max<Real>(pData[6],-1.0f),1.0f) );        /* Calculate Y-axis angle */
    C           =  cos( angle.y );
    angle.y		=  (Real)radtodeg(angle.y);//*=  RADIANS;
    if ( fabs( C ) > 0.005 )             /* Gimball lock? */
    {
		tr_x      =  pData[8] / C;           /* No, so get X-axis angle */
		tr_y      = -pData[7] / C;
		angle.x  = (Real)radtodeg(atan2( tr_y, tr_x ));
		tr_x      =  pData[0] / C;            /* Get Z-axis angle */
		tr_y      = -pData[3] / C;
		angle.z  = (Real)radtodeg(atan2( tr_y, tr_x ));
    }
    else                                 /* Gimball lock has occurred */
    {
		angle.x  = 0;                      /* Set X-axis angle to zero */
		tr_x      =  pData[4];                 /* And calculate Z-axis angle */
		tr_y      =  pData[1];
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
FORCE_INLINE Quaternion Matrix3x3::getQuaternion ( void ) const
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

// Getters - Scale
FORCE_INLINE Vector3d Matrix3x3::getScaling ( void ) const
{
	Vector3d result;
	/*result.x = Vector3d( pData[0],pData[1],pData[2] ).magnitude();
	result.y = Vector3d( pData[4],pData[5],pData[6] ).magnitude();
	result.z = Vector3d( pData[8],pData[9],pData[10] ).magnitude();*/
	result.x = Vector3d( pData[0],pData[3],pData[6] ).magnitude();
	result.y = Vector3d( pData[1],pData[4],pData[7] ).magnitude();
	result.z = Vector3d( pData[2],pData[5],pData[8] ).magnitude();

	/* check for invalid values */
	assert( result.x == result.x );
	assert( result.x <= FLT_MAX && result.x >= -FLT_MAX );
	assert( result.y == result.y );
	assert( result.y <= FLT_MAX && result.y >= -FLT_MAX );
	assert( result.z == result.z );
	assert( result.z <= FLT_MAX && result.z >= -FLT_MAX );

	return result;
}

// Transpose matrix
FORCE_INLINE Matrix3x3 Matrix3x3::transpose ( void ) const
{
	return !(*this);
}
FORCE_INLINE Matrix3x3 Matrix3x3::operator! ( void ) const
{
	Matrix3x3 result;
	result.pData[0] = this->pData[0];
	result.pData[1] = this->pData[3];
	result.pData[2] = this->pData[6];

	result.pData[3] = this->pData[1];
	result.pData[4] = this->pData[4];
	result.pData[5] = this->pData[7];

	result.pData[6] = this->pData[2];
	result.pData[7] = this->pData[5];
	result.pData[8] = this->pData[8];

	return result;
}

// Matrix inverse general
FORCE_INLINE Matrix3x3 Matrix3x3::inverse ( void ) const
{
	Real detVal = det();
	assert( detVal == detVal );
	assert( detVal <= FLT_MAX && detVal >= -FLT_MAX );
	if ( fabs( detVal ) <= 0.0005f )
	{
		return Matrix3x3();
	}
	else
	{
		Matrix3x3		mresult;
		Matrix2x2		mtemp;
		int     i, j, sign;
		for ( i = 0; i < 3; i++ )
		{
			for ( j = 0; j < 3; j++ )
			{
				sign = 1 - ( (i +j) % 2 ) * 2;
				mtemp = submatrix( i, j );
				mresult[j][i] = ( mtemp.det() * sign ) / detVal;
			}
		}
		return mresult;
	}
}
// Determinant
FORCE_INLINE Real Matrix3x3::det ( void ) const
{
	return ( (pData[0]*pData[4]*pData[8]) + (pData[1]*pData[5]*pData[6]) + (pData[2]*pData[3]*pData[7])
		- (pData[2]*pData[4]*pData[6]) - (pData[1]*pData[3]*pData[8]) - (pData[0]*pData[5]*pData[7]) );
}

// Retrieve submatrix
FORCE_INLINE Matrix2x2 Matrix3x3::submatrix ( int ex_x, int ex_y ) const
{
	Real newData [4];
	int di, dj, si, sj;
	for( di = 0; di < 2; ++di )
	{
		for( dj = 0; dj < 2; ++dj )
		{
			// map 3x3 element (destination) to 4x4 element (source)
			si = di + ( ( di >= ex_x ) ? 1 : 0 );
			sj = dj + ( ( dj >= ex_y ) ? 1 : 0 );
			// copy element
			newData[di*2 + dj] = pData[si*3 + sj];
	    }
	}

	// Create result
	Matrix2x2 result( newData );

	return result;
}

// Linear Interpolation
FORCE_INLINE void Matrix3x3::Lerp ( const Matrix3x3& right, const Real t )
{
	int x;
	for ( x = 0; x < 9; x += 1 )
	{
		pData[x] += ( right.pData[x] - pData[x] )*t;
	}
}
FORCE_INLINE Matrix3x3 Matrix3x3::LerpTo ( const Matrix3x3& right, const Real t ) const
{
	Matrix3x3 result;
	int x;
	for ( x = 0; x < 9; x += 1 )
	{
		result.pData[x] = pData[x] + ( right.pData[x] - pData[x] )*t;
	}
	return result;
}


//---Function Operators
// Multiplication
FORCE_INLINE Matrix3x3 Matrix3x3::operator* ( const Matrix3x3& right ) const
{
	Matrix3x3 result;
	/*int x, y, c;
	for ( x = 0; x < 3; x += 1 )
	{
		for ( y = 0; y < 3; y += 1 )
		{
			result[y][x] = 0.0;
			for ( c = 0; c < 3; c += 1 )
			{
				result[y][x] += ((*this)[y][c]) * (right[c][x]);
			}
		}
	}
	for ( x = 0; x < 3; x += 1 )
	{
		for ( y = 0; y < 3; y += 1 )
		{
			assert( result[y][x] == result[y][x] );
			assert( result[y][x] <= FLT_MAX && result[y][x] >= -FLT_MAX );
		}
	}*/
	int x,y,c;
	for ( x = 0; x < 3; ++x ) // column
	{
		for ( y = 0; y < 3; ++y ) // row
		{
			result.pData[x+y*3] = 0;
			for ( c = 0; c < 3; ++c )
			{
				result.pData[x+y*3] += pData[c+y*3] * right.pData[x+c*3];
			}
		}
	}

#ifdef _ENGINE_DEBUG
	if ( !result.isOk() ) {
		printf( " bad matrix3\n" );
	}
#endif

	return result;
}
FORCE_INLINE bool Matrix3x3::isOk ( void )
{
	int x, y;
	bool r = true;
	for ( x = 0; x < 3; x += 1 )
	{
		for ( y = 0; y < 3; y += 1 )
		{
			if (( (*this)[y][x] == (*this)[y][x] )&&( (*this)[y][x] <= FLT_MAX && (*this)[y][x] >= -FLT_MAX ))
			{
				// Nothing. This is correct.
			}
			else
			{
				r = false;
				(*this)[y][x] = (Real) ((y==x)?1:0);
			}
		}
	}
	return r;
}
FORCE_INLINE Matrix3x3 Matrix3x3::operator*=( Matrix3x3 const& right )
{
	(*this) = (*this) * right;
	return (*this);
}

// Multiply by a vector
FORCE_INLINE Vector3d Matrix3x3::operator* ( const Vector3d& vect ) const
{
	/*return Vector3d(
		pData[0]*vect.x+pData[1]*vect.y+pData[2]*vect.z+pData[3],
		pData[4]*vect.x+pData[5]*vect.y+pData[6]*vect.z+pData[7],
		pData[8]*vect.x+pData[9]*vect.y+pData[10]*vect.z+pData[11]
		);*/
	/*return Vector3d(
		pData[0]*vect.x+pData[4]*vect.y+pData[8]*vect.z+pData[3],
		pData[1]*vect.x+pData[5]*vect.y+pData[9]*vect.z+pData[7],
		pData[2]*vect.x+pData[6]*vect.y+pData[10]*vect.z+pData[11]
		);*/
	/*return Vector3d(
		pData[0]*vect.x+pData[3]*vect.y+pData[6]*vect.z,
		pData[1]*vect.x+pData[4]*vect.y+pData[7]*vect.z,
		pData[2]*vect.x+pData[5]*vect.y+pData[8]*vect.z
		);*/
	return Vector3d(
		pData[0]*vect.x+pData[1]*vect.y+pData[2]*vect.z,
		pData[3]*vect.x+pData[4]*vect.y+pData[5]*vect.z,
		pData[6]*vect.x+pData[7]*vect.y+pData[8]*vect.z
		);
	/*return Vector3d(
		pData[0]*vect.x+pData[4]*vect.y+pData[8]*vect.z+pData[12],
		pData[1]*vect.x+pData[5]*vect.y+pData[9]*vect.z+pData[13],
		pData[2]*vect.x+pData[6]*vect.y+pData[10]*vect.z+pData[14]
		);*/
}
// Scalar multiply
FORCE_INLINE Matrix3x3 Matrix3x3::operator* ( const Real right ) const
{
	Matrix3x3 result;
	for ( int x = 0; x < 9; x++ )
		result.pData[x] = pData[x] * right;
	return result;
}


// Add matrices
FORCE_INLINE Matrix3x3 Matrix3x3::operator+ ( const Matrix3x3& right ) const
{
	Matrix3x3 result;
	for ( int x = 0; x < 9; x++ )
		result.pData[x] = pData[x] + right.pData[x];
	return result;
}
FORCE_INLINE Matrix3x3 Matrix3x3::operator+= ( const Matrix3x3& right )
{
	(*this) = (*this) + right;
	return (*this);
}

// Public Accessor
FORCE_INLINE const Real* Matrix3x3::operator[] ( int index ) const
{
	return &( pData[index*3] );
}
// Private Accessor and Editor
FORCE_INLINE Real* Matrix3x3::operator[] ( int index )
{
	return &( pData[index*3] );
}

//Equal comparison overload
FORCE_INLINE bool Matrix3x3::operator== (Matrix3x3 const& right) const
{
	for (short i = 0; i < 9; i++)
		if (fabs(right.pData[i] - pData[i]) > FTYPE_PRECISION)
			return false;

	return true;
}

//Not equal comparison overload
FORCE_INLINE bool Matrix3x3::operator!= (Matrix3x3 const& right) const
{
	return !((*this) == right);
}
