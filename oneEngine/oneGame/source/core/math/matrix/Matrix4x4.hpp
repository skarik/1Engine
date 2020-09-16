#include "CMatrix.h"
#include "string.h"
#include <algorithm>

FORCE_INLINE Matrix4x4::Matrix4x4 ( void )
{
	pData[0] = 1.0F;
	pData[5] = 1.0F;
	pData[10] = 1.0F;
	pData[15] = 1.0F;

	pData[1] = 0.0F; pData[2] = 0.0F; pData[3] = 0.0F;
	pData[4] = 0.0F; pData[6] = 0.0F; pData[7] = 0.0F;
	pData[8] = 0.0F; pData[9] = 0.0F; pData[11] = 0.0F;
	pData[12] = 0.0F; pData[13] = 0.0F; pData[14] = 0.0F;
}
FORCE_INLINE Matrix4x4::Matrix4x4 ( const Matrix4x4& nSrc )
{
	std::copy(nSrc.pData, nSrc.pData + 16, pData);
}
FORCE_INLINE Matrix4x4::Matrix4x4 ( const Real* nData )
{
	std::copy(nData, nData + 16, pData);
}
FORCE_INLINE Matrix4x4::Matrix4x4 ( const Vector4f& row_x, const Vector4f& row_y, const Vector4f& row_z, const Vector4f& row_w )
{
	pData[0] = row_x[0];
	pData[1] = row_x[1];
	pData[2] = row_x[2];
	pData[3] = row_x[3];

	pData[4] = row_y[0];
	pData[5] = row_y[1];
	pData[6] = row_y[2];
	pData[7] = row_y[3];

	pData[8] = row_z[0];
	pData[9] = row_z[1];
	pData[10] = row_z[2];
	pData[11] = row_z[3];

	pData[12] = row_w[0];
	pData[13] = row_w[1];
	pData[14] = row_w[2];
	pData[15] = row_w[3];
}


// Set translation with FTYPE
FORCE_INLINE bool Matrix4x4::setTranslation ( Real const x, Real const y, Real const z )
{
	pData[3] = x;
	pData[7] = y;
	pData[11]= z;
	return true;
}
// Settin the translation via Vector3D
FORCE_INLINE bool Matrix4x4::setTranslation ( Vector3f const& vect )
{
	pData[3] = vect.x;
	pData[7] = vect.y;
	pData[11]= vect.z;
	return true;
}
// Set scaling with FTYPE
FORCE_INLINE bool Matrix4x4::setScale ( Real const x, Real const y, Real const z )
{
	pData[0] = x;
	pData[5] = y;
	pData[10]= z;
	return true;
}
// Settin the scaling via Vector3D
FORCE_INLINE bool Matrix4x4::setScale ( Vector3f const& vect )
{
	pData[0] = vect.x;
	pData[5] = vect.y;
	pData[10]= vect.z;
	return true;
}

// Settin the rotation
FORCE_INLINE bool Matrix4x4::setRotation ( const Real angle_x, const Real angle_y, const Real angle_z )
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
    pData[4]  =  -C * F;
    pData[8]  =   D;

    pData[1]  =  BD * E + A * F;
    pData[5]  = -BD * F + A * E;
    pData[9]  =  -B * C;

    pData[2]  = -AD * E + B * F;
    pData[6]  =  AD * F + B * E;
    pData[10] =   A * C;

    pData[3]  =  pData[7] = pData[11] = pData[12] = pData[13] = pData[14] = 0;
    pData[15] =  1;

	return true;
}
FORCE_INLINE bool Matrix4x4::setRotationZYX ( const Real angle_x, const Real angle_y, const Real angle_z )
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

    pData[4]  =  BD * E + A * F;
    pData[5]  = -BD * F + A * E;
    pData[6]  =  -B * C;

    pData[8]  = -AD * E + B * F;
    pData[9]  =  AD * F + B * E;
    pData[10] =   A * C;

    pData[3]  =  pData[7] = pData[11] = pData[12] = pData[13] = pData[14] = 0;
    pData[15] =  1;

	return true;
}

// Setting the rotation of a 3d matrix via 3d vector
FORCE_INLINE bool Matrix4x4::setRotation ( const Vector3f& vect )
{
	return setRotation(vect.x,vect.y,vect.z);
}
// Setting rotation of a 3d matrix via a quaternion
FORCE_INLINE bool Matrix4x4::setRotation ( const Quaternion& quat )
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

    pData[4]  =     2 * ( xy + zw );
    pData[5]  = 1 - 2 * ( xx + zz );
    pData[6]  =     2 * ( yz - xw );

	pData[8]  =     2 * ( xz - yw );
    pData[9]  =     2 * ( yz + xw );
    pData[10] = 1 - 2 * ( xx + yy );

	pData[3]  = pData[7] = pData[11] = pData[12] = pData[13] = pData[14] = 0;
    pData[15] = 1;
	/*pData[0]  = 1 - 2 * ( yy + zz );
    pData[4]  =     2 * ( xy - zw );
    pData[8]  =     2 * ( xz + yw );

    pData[1]  =     2 * ( xy + zw );
    pData[5]  = 1 - 2 * ( xx + zz );
    pData[9]  =     2 * ( yz - xw );

	pData[2]  =     2 * ( xz - yw );
    pData[6]  =     2 * ( yz + xw );
    pData[10] = 1 - 2 * ( xx + yy );

	pData[12] = pData[13] = pData[14] = pData[3] = pData[7] = pData[11] = 0;
    pData[15] = 1;*/

	return true;
}
// Setting rotation of a 3d 4x4 matrix via copy from a 3x3 matrix
FORCE_INLINE bool Matrix4x4::setRotation ( const Matrix3x3& matx )
{
	pData[0] = matx.pData[0];
	pData[1] = matx.pData[1];
	pData[2] = matx.pData[2];

	pData[4] = matx.pData[3];
	pData[5] = matx.pData[4];
	pData[6] = matx.pData[5];

	pData[8] = matx.pData[6];
	pData[9] = matx.pData[7];
	pData[10] = matx.pData[8];

	pData[3]  = pData[7] = pData[11] = pData[12] = pData[13] = pData[14] = 0;
    pData[15] = 1;
	return true;
}

// Modders - Translation
FORCE_INLINE void Matrix4x4::translate ( const Vector3f& v )
{
	pData[3] += v.x;
	pData[7] += v.y;
	pData[11] += v.z;
}

// Getters - Translation
FORCE_INLINE Vector3f Matrix4x4::getTranslation ( void ) const
{
	Vector3f position;

	position.x = pData[3];
	position.y = pData[7];
	position.z = pData[11];

	return position;
}

// Getters - Rotation
FORCE_INLINE Vector3f Matrix4x4::getEulerAngles ( void ) const
{
	float tr_x, tr_y, D,C;
	Vector3f angle;
	//angle.y = D =  asin( min( max( pData[2], -1.0f ), 1.0f ) );        /* Calculate Y-axis angle */

	// Check for invalid results
	/*assert( _finite( angle.y ) );
	assert( !_isnan( angle.y ) );
	assert( angle.y == angle.y );
	assert( angle.y <= FLT_MAX && angle.y >= -FLT_MAX );
	assert( errno != EDOM );*/
	// End check

	//angle.y = D =  asin( min<Real>(max<Real>(pData[2],-1.0f),1.0f) );        /* Calculate Y-axis angle */
 //   C           =  cos( angle.y );
 //   angle.y		=  (Real)radtodeg(angle.y);//*=  RADIANS;
 //   if ( fabs( C ) > 0.005 )             /* Gimball lock? */
 //   {
 //     tr_x      =  pData[10] / C;           /* No, so get X-axis angle */
 //     tr_y      = -pData[6]  / C;
 //     angle.x  = (Real)radtodeg(atan2( tr_y, tr_x ));
 //     tr_x      =  pData[0] / C;            /* Get Z-axis angle */
 //     tr_y      = -pData[1] / C;
 //     angle.z  = (Real)radtodeg(atan2( tr_y, tr_x ));
 //   }
 //   else                                 /* Gimball lock has occurred */
 //   {
 //     angle.x  = 0;                      /* Set X-axis angle to zero */
 //     tr_x      =  pData[5];                 /* And calculate Z-axis angle */
 //     tr_y      =  pData[4];
 //     angle.z  = (Real)radtodeg(atan2( tr_y, tr_x ));
 //   }

	angle.y = D =  asin( std::min<Real>(std::max<Real>(pData[8],-1.0f),1.0f) );        /* Calculate Y-axis angle */
    C           =  cos( angle.y );
    angle.y		=  (Real)radtodeg(angle.y);//*=  RADIANS;
    if ( fabs( C ) > 0.005 )             /* Gimball lock? */
    {
      tr_x      =  pData[10] / C;           /* No, so get X-axis angle */
      tr_y      = -pData[9]  / C;
      angle.x  = (Real)radtodeg(atan2( tr_y, tr_x ));
      tr_x      =  pData[0] / C;            /* Get Z-axis angle */
      tr_y      = -pData[4] / C;
      angle.z  = (Real)radtodeg(atan2( tr_y, tr_x ));
    }
    else                                 /* Gimball lock has occurred */
    {
      angle.x  = 0;                      /* Set X-axis angle to zero */
      tr_x      =  pData[5];                 /* And calculate Z-axis angle */
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
FORCE_INLINE Quaternion Matrix4x4::getQuaternion ( void ) const
{
	Real S, X, Y, Z, W;

	Real tr = pData[0] + pData[5] + pData[10];
	if ( tr > 0 ) {
		S = sqrt(tr+1.0f) * 2; // S=4*qw
		W = 0.25f * S;
		X = (pData[9] - pData[6]) / S;
		Y = (pData[2] - pData[8]) / S;
		Z = (pData[4] - pData[1]) / S;
	}
	else if ( pData[0] > pData[5] && pData[0] > pData[10] )
	{	// Column 0:
        S  = (Real)sqrt( 1.0f + pData[0] - pData[5] - pData[10] ) * 2;
        X = 0.25f * S;
        Y = (pData[4] + pData[1]) / S;
        Z = (pData[2] + pData[8]) / S;
        W = (pData[9] - pData[6]) / S;
    }
	else if ( pData[5] > pData[10] )
	{	// Column 1:
        S  = (Real)sqrt( 1.0f + pData[5] - pData[0] - pData[10] ) * 2;
        X = (pData[4] + pData[1]) / S;
        Y = 0.25f * S;
        Z = (pData[9] + pData[6]) / S;
        W = (pData[2] - pData[8]) / S;
    }
	else
	{	// Column 2:
        S  = (Real)sqrt( 1.0f + pData[10] - pData[0] - pData[5] ) * 2;
        X = (pData[2] + pData[8]) / S;
        Y = (pData[9] + pData[6]) / S;
        Z = 0.25f * S;
        W = (pData[4] - pData[1]) / S;
    }
	return Quaternion( X,Y,Z,W );
}
// Get Rotator
FORCE_INLINE Matrix3x3 Matrix4x4::getRotator ( void ) const
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
// Getters - Scale
FORCE_INLINE Vector3f Matrix4x4::getScaling ( void ) const
{
	Vector3f result;
	/*result.x = Vector3f( pData[0],pData[1],pData[2] ).magnitude();
	result.y = Vector3f( pData[4],pData[5],pData[6] ).magnitude();
	result.z = Vector3f( pData[8],pData[9],pData[10] ).magnitude();*/
	result.x = Vector3f( pData[0],pData[4],pData[8] ).magnitude();
	result.y = Vector3f( pData[1],pData[5],pData[9] ).magnitude();
	result.z = Vector3f( pData[2],pData[6],pData[10] ).magnitude();

	/* check for invalid values */
	assert( result.x == result.x );
	assert( result.x <= FLT_MAX && result.x >= -FLT_MAX );
	assert( result.y == result.y );
	assert( result.y <= FLT_MAX && result.y >= -FLT_MAX );
	assert( result.z == result.z );
	assert( result.z <= FLT_MAX && result.z >= -FLT_MAX );

	return result;
}

//---Function definition
// Matrix transposition
FORCE_INLINE Matrix4x4 Matrix4x4::transpose ( void ) const
{
	return !(*this);
}

// Matrix inverse general
FORCE_INLINE Matrix4x4 Matrix4x4::inverse ( void ) const
{
	Real detVal = det();
	assert( detVal == detVal );
	assert( detVal <= FLT_MAX && detVal >= -FLT_MAX );
	if ( fabs( detVal ) <= 0.0005f )
	{
		return Matrix4x4();
	}
	else
	{
		Matrix4x4		mresult;
		Matrix3x3		mtemp;
		int     i, j, sign;
		for ( i = 0; i < 4; i++ )
		{
			for ( j = 0; j < 4; j++ )
			{
				sign = 1 - ( (i +j) % 2 ) * 2;
				mtemp = submatrix( i, j );
				mresult[j][i] = ( mtemp.det() * sign ) / detVal;
			}
		}
		return mresult;
	}
}

// Matrix determinant of arbitrary size (TODO)
FORCE_INLINE Real Matrix4x4::det ( void ) const
{
	Real   det, result = 0, i = 1;
	Matrix3x3 msub;
	int     n;
	for ( n = 0; n < 4; n++, i *= -1 )
	{
		msub	= submatrix( 0, n );
		det     = msub.det();
		result += pData[n] * det * i;
	}
	return( result );
}

// Retrieve submatrix
FORCE_INLINE Matrix3x3 Matrix4x4::submatrix ( int ex_x, int ex_y ) const
{
	Real newData [9];
	int di, dj, si, sj;
	for( di = 0; di < 3; ++di )
	{
		for( dj = 0; dj < 3; ++dj )
		{
			// map 3x3 element (destination) to 4x4 element (source)
			si = di + ( ( di >= ex_x ) ? 1 : 0 );
			sj = dj + ( ( dj >= ex_y ) ? 1 : 0 );
			// copy element
			newData[di*3 + dj] = pData[si*4 + sj];
	    }
	}

	// Create result
	Matrix3x3 result( newData );

	return result;
}


// Linear Interpolation
FORCE_INLINE void Matrix4x4::Lerp ( const Matrix4x4& right, const Real t )
{
	int x;
	for ( x = 0; x < 16; x += 1 )
	{
		pData[x] += ( right.pData[x] - pData[x] )*t;
	}
}
FORCE_INLINE Matrix4x4 Matrix4x4::LerpTo ( const Matrix4x4& right, const Real t ) const
{
	Matrix4x4 result;
	int x;
	for ( x = 0; x < 16; x += 1 )
	{
		result.pData[x] = pData[x] + ( right.pData[x] - pData[x] )*t;
	}
	return result;
}


//---Function Operators
// Multiplication
FORCE_INLINE Matrix4x4 Matrix4x4::operator* ( const Matrix4x4& right ) const
{
//#ifdef _WIN32
#ifdef FUCKING_PEN0RS
	Matrix4x4 result;
	const float* A = pData;
	const float* B = right.pData;
	float* C = result.pData;
	__m128 row1 = _mm_load_ps(&B[0]);
    __m128 row2 = _mm_load_ps(&B[4]);
    __m128 row3 = _mm_load_ps(&B[8]);
    __m128 row4 = _mm_load_ps(&B[12]);
    for(int i=0; i<4; i++)
	{
        __m128 brod1 = _mm_set1_ps(A[4*i + 0]);
        __m128 brod2 = _mm_set1_ps(A[4*i + 1]);
        __m128 brod3 = _mm_set1_ps(A[4*i + 2]);
        __m128 brod4 = _mm_set1_ps(A[4*i + 3]);
        __m128 row = _mm_add_ps(
                    _mm_add_ps(
                        _mm_mul_ps(brod1, row1),
                        _mm_mul_ps(brod2, row2)),
                    _mm_add_ps(
                        _mm_mul_ps(brod3, row3),
                        _mm_mul_ps(brod4, row4)));
        _mm_store_ps(&C[4*i], row);
    }
	return result;
#else
	/*Matrix4x4 result;
	int x,y,c;
	for ( x = 0; x < 4; ++x ) // column
	{
		for ( y = 0; y < 4; ++y ) // row
		{
			result.pData[x+y*4] = 0;
			for ( c = 0; c < 4; ++c )
			{
				result.pData[x+y*4] += pData[c+y*4] * right.pData[x+c*4];
			}
		}
	}*/
	Matrix4x4 result;
	for (unsigned int i = 0; i < 16; i += 4)
        for (unsigned int j = 0; j < 4; ++j)
            result.pData[i + j] = (pData[i + 0] * right.pData[j +  0])
								+ (pData[i + 1] * right.pData[j +  4])
								+ (pData[i + 2] * right.pData[j +  8])
								+ (pData[i + 3] * right.pData[j + 12]);
#endif

#ifdef _ENGINE_DEBUG
	if ( !result.isOk() ) {
		printf( " bad matrix4\n" );
	}
#endif

	return result;
}
FORCE_INLINE bool Matrix4x4::isOk ( void )
{
	bool r = true;
	/*int x, y;
	for ( x = 0; x < 4; x += 1 )
	{
		for ( y = 0; y < 4; y += 1 )
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
	}*/
	for (unsigned int i = 0; i < 16; ++i)
	{
		if (pData[i] == pData[i] && pData[i] <= FLT_MAX && pData[i] >= -FLT_MAX)
		{
			// Nothing, this is correct.
		}
		else
		{
			r = false;
			pData[0] = (Real)((i%5)?0:1);
		}
	}
	return r;
}
FORCE_INLINE Matrix4x4 Matrix4x4::operator*=( const Matrix4x4& right )
{
	(*this) = (*this) * right;
	return (*this);
}

// Multiply by a vector
FORCE_INLINE Vector3f Matrix4x4::operator* ( const Vector3f& vect ) const
{
	return Vector3f(
		pData[0]*vect.x+pData[1]*vect.y+pData[2]*vect.z+pData[3],
		pData[4]*vect.x+pData[5]*vect.y+pData[6]*vect.z+pData[7],
		pData[8]*vect.x+pData[9]*vect.y+pData[10]*vect.z+pData[11]
		);
	/*return Vector3f(
		pData[0]*vect.x+pData[4]*vect.y+pData[8]*vect.z+pData[3],
		pData[1]*vect.x+pData[5]*vect.y+pData[9]*vect.z+pData[7],
		pData[2]*vect.x+pData[6]*vect.y+pData[10]*vect.z+pData[11]
		);*/
	/*return Vector3f(
		pData[0]*vect.x+pData[4]*vect.y+pData[8]*vect.z+pData[12],
		pData[1]*vect.x+pData[5]*vect.y+pData[9]*vect.z+pData[13],
		pData[2]*vect.x+pData[6]*vect.y+pData[10]*vect.z+pData[14]
		);*/
}

FORCE_INLINE Vector4f Matrix4x4::operator* ( const Vector4f& vect ) const
{
	return Vector4f(
		pData[0]*vect.x+pData[1]*vect.y+pData[2]*vect.z+pData[3]*vect.w,
		pData[4]*vect.x+pData[5]*vect.y+pData[6]*vect.z+pData[7]*vect.w,
		pData[8]*vect.x+pData[9]*vect.y+pData[10]*vect.z+pData[11]*vect.w,
		pData[12]*vect.x+pData[13]*vect.y+pData[14]*vect.z+pData[15]*vect.w
		);
	/*return Vector4f(
		pData[0]*vect.x+pData[4]*vect.y+pData[8]*vect.z+pData[12]*vect.w,
		pData[1]*vect.x+pData[5]*vect.y+pData[9]*vect.z+pData[13]*vect.w,
		pData[2]*vect.x+pData[6]*vect.y+pData[10]*vect.z+pData[14]*vect.w,
		pData[3]*vect.x+pData[7]*vect.y+pData[11]*vect.z+pData[15]*vect.w
		);*/
}

// Multiply scalar
FORCE_INLINE Matrix4x4 Matrix4x4::operator* ( const Real right ) const
{
	Matrix4x4 result;
	for ( int x = 0; x < 16; x++ )
		result.pData[x] = pData[x] * right;
	return result;
}

// Add matrices
FORCE_INLINE Matrix4x4 Matrix4x4::operator+ ( const Matrix4x4& right ) const
{
	Matrix4x4 result;
	for ( int x = 0; x < 16; x++ )
		result.pData[x] = pData[x] + right.pData[x];
	return result;
}

FORCE_INLINE Matrix4x4 Matrix4x4::operator+= ( const Matrix4x4& right )
{
	(*this) = (*this) + right;
	return (*this);
}

// Transpose
FORCE_INLINE Matrix4x4 Matrix4x4::operator! ( void ) const
{
	Matrix4x4 result;
	result.pData[0] = this->pData[0];
	result.pData[1] = this->pData[4];
	result.pData[2] = this->pData[8];
	result.pData[3] = this->pData[12];

	result.pData[4] = this->pData[1];
	result.pData[5] = this->pData[5];
	result.pData[6] = this->pData[9];
	result.pData[7] = this->pData[13];

	result.pData[8] = this->pData[2];
	result.pData[9] = this->pData[6];
	result.pData[10] = this->pData[10];
	result.pData[11] = this->pData[14];

	result.pData[12] = this->pData[3];
	result.pData[13] = this->pData[7];
	result.pData[14] = this->pData[11];
	result.pData[15] = this->pData[15];
	return result;
}

// Public Accessor
FORCE_INLINE const Real* Matrix4x4::operator[] ( int index ) const
{
	return &( pData[index*4] );
}
// Private Accessor and Editor
FORCE_INLINE Real* Matrix4x4::operator[] ( int index )
{
	return &( pData[index*4] );
}

//Equal comparison overload
FORCE_INLINE bool Matrix4x4::operator== (Matrix4x4 const& right) const
{
	for (short i = 0; i < 16; i++)
		if (fabs(right.pData[i] - pData[i]) > FTYPE_PRECISION)
			return false;

	return true;
}

//Not equal comparison overload
FORCE_INLINE bool Matrix4x4::operator!= (Matrix4x4 const& right) const
{
	return !((*this) == right);
}