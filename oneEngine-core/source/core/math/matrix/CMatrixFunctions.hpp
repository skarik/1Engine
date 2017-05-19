
#ifndef _C_MATRIX_FUNCTIONS_
#define _C_MATRIX_FUNCTIONS_

// Includes
#include "CMatrix.h"

//---Constructors and destructors
// Constructor
template <int W, int H>
CMatrix<W,H>::CMatrix ( void )
{
	int i;
	for ( i = W*H-1; i >= 0; i -= 1 )
	{
		pData[i] = 0.0f;
	}
	for ( i = min(W,H)-1; i >= 0; i -= 1 )
	{
		pData[i+i*W] = 1.0f;
	}
}

template <int W, int H>
CMatrix<W,H>::CMatrix ( Real * pInData )
{
	for ( int i = W*H-1; i >= 0; i -= 1 )
	{
		pData[i] = pInData[i];
	}
}

//---Function definition
// Settin the translation
template<> inline bool Matrix2x2::setTranslation ( Real const x, Real const y, Real const z )
{
	pData[1] = x;
	pData[3] = y;
	return true;
}
template<> inline bool Matrix4x4::setTranslation ( Real const x, Real const y, Real const z )
{
	pData[3] = x;
	pData[7] = y;
	pData[11]= z;
	return true;
}

// Settin the translation via array of ftypes
template <int W, int H>
bool CMatrix<W,H>::setTranslation ( Real * pInVector )
{
	int i;
	for ( i = min(W,H); i > 0; i -= 1 )
	{
		pData[(i*W)-1] = pInVector[i-1];
	}
	return true;
}

// Settin the translation via Vector2D
template<> inline bool Matrix2x2::setTranslation ( Vector2d const& v )
{
	pData[1] = v.x;
	pData[3] = v.y;
	return true;
}

// Settin the translation via Vector3D
template<> inline bool Matrix4x4::setTranslation ( Vector3d const& vect )
{
	pData[3] = vect.x;
	pData[7] = vect.y;
	pData[11]= vect.z;
	return true;
}

// Settin the scaling
template<> inline bool Matrix2x2::setScale ( Real const x, Real const y, Real const z )
{
	pData[0] = x;
	pData[3] = y;
	return true;
}
template<> inline bool Matrix4x4::setScale ( Real const x, Real const y, Real const z )
{
	pData[0] = x;
	pData[5] = y;
	pData[10]= z;
	return true;
}

// Settin the scaling via Vector3D
template<> inline bool Matrix4x4::setScale ( Vector3d const& vect )
{
	pData[0] = vect.x;
	pData[5] = vect.y;
	pData[10]= vect.z;
	return true;
}

// Settin the scaling via array of ftypes
template <int W, int H>
bool CMatrix<W,H>::setScale ( Real * pInVector )
{
	int i;
	for ( i = min(W,H); i > 0; i -= 1 )
	{
		pData[(i-1)*W+i] = pInVector[i-1];
	}
	return true;
}

// Settin the rotation of a 2D matrix
template<> inline bool Matrix2x2::setRotation ( Real const angle )
{
	/*Matrix2x2 rotationResult;
	Real trueAngle = angle * 3.14159265f / 180.0f;
	rotationResult.pData[0] = cos( trueAngle );
	rotationResult.pData[3] = rotationResult.pData[0];
	rotationResult.pData[2] = sin( trueAngle );
	rotationResult.pData[2] = -rotationResult.pData[1];

	(*this) *= rotationResult;*/
	Real trueAngle = angle * 3.14159265f / 180.0f;
	pData[0] = cos( trueAngle );
	pData[3] = pData[0];
	pData[2] = sin( trueAngle );
	pData[2] = -pData[1];

	return true;
}

// Settin the rotation
template<> inline bool Matrix4x4::setRotation ( Real const angle_x, Real const angle_y, Real const angle_z )
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

	/*pData[0]  =   C * E;
    pData[4]  =  -C * F;
    pData[8]  =   D;

    pData[1]  =  BD * E + A * F;
    pData[5]  = -BD * F + A * E;
    pData[9]  =  -B * C;
    pData[2]  = -AD * E + B * F;
    pData[6]  =  AD * F + B * E;
    pData[10] =   A * C;
    pData[12]  =  pData[13] = pData[14] = pData[3] = pData[7] = pData[11] = 0;
    pData[15] =  1;*/

	return true;
}

// Setting the rotation of a 3d matrix via 3d vector
template<> inline bool Matrix4x4::setRotation ( Vector3d const& vect )
{
	/* check for invalid values */
	assert( vect.x == vect.x );
	assert( vect.x <= FLT_MAX && vect.x >= -FLT_MAX );
	assert( vect.y == vect.y );
	assert( vect.y <= FLT_MAX && vect.y >= -FLT_MAX );
	assert( vect.z == vect.z );
	assert( vect.z <= FLT_MAX && vect.z >= -FLT_MAX );

	Real ax = (Real)degtorad(vect.x);
	Real ay = (Real)degtorad(vect.y);
	Real az = (Real)degtorad(vect.z);
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

	/*pData[0]  =   C * E;
    pData[4]  =  -C * F;
    pData[8]  =   D;

    pData[1]  =  BD * E + A * F;
    pData[5]  = -BD * F + A * E;
    pData[9]  =  -B * C;
    pData[2]  = -AD * E + B * F;
    pData[6]  =  AD * F + B * E;
    pData[10] =   A * C;
    pData[12]  =  pData[13] = pData[14] = pData[3] = pData[7] = pData[11] = 0;
    pData[15] =  1;*/

	return true;
}
// Setting rotation of a 3d matrix via a quaternion
template<> inline bool Matrix4x4::setRotation ( Quaternion const& quat )
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

    pData[4]  =     2 * ( xy + zw );
    pData[5]  = 1 - 2 * ( xx + zz );
    pData[6]  =     2 * ( yz - xw );

	pData[8]  =     2 * ( xz - yw );
    pData[9]  =     2 * ( yz + xw );
    pData[10] = 1 - 2 * ( xx + yy );

	pData[3]  = pData[7] = pData[11] = pData[12] = pData[13] = pData[14] = 0;
    pData[15] = 1;*/
	pData[0]  = 1 - 2 * ( yy + zz );
    pData[4]  =     2 * ( xy - zw );
    pData[8]  =     2 * ( xz + yw );

    pData[1]  =     2 * ( xy + zw );
    pData[5]  = 1 - 2 * ( xx + zz );
    pData[9]  =     2 * ( yz - xw );

	pData[2]  =     2 * ( xz - yw );
    pData[6]  =     2 * ( yz + xw );
    pData[10] = 1 - 2 * ( xx + yy );

	pData[12] = pData[13] = pData[14] = pData[3] = pData[7] = pData[11] = 0;
    pData[15] = 1;

	return true;
}
// Setting rotation of a 3d 4x4 matrix via copy from a 3x3 matrix
template<> inline bool Matrix4x4::setRotation ( Matrix3x3 const& matx )
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

// Getters - Translation
template<> inline Vector3d Matrix4x4::getTranslation ( void ) const
{
	Vector3d position;

	position.x = pData[3];
	position.y = pData[7];
	position.z = pData[11];

	return position;
}

// Getters - Rotation
template<> inline Vector3d Matrix4x4::getEulerAngles ( void ) const
{
	float tr_x, tr_y, D,C;
	Vector3d angle;
	//angle.y = D =  asin( min( max( pData[2], -1.0f ), 1.0f ) );        /* Calculate Y-axis angle */

	// Check for invalid results
	/*assert( _finite( angle.y ) );
	assert( !_isnan( angle.y ) );
	assert( angle.y == angle.y );
	assert( angle.y <= FLT_MAX && angle.y >= -FLT_MAX );
	assert( errno != EDOM );*/
	// End check

	angle.y = D =  asin( min<Real>(max<Real>(pData[2],-1.0f),1.0f) );        /* Calculate Y-axis angle */
    C           =  cos( angle.y );
    angle.y		=  (Real)radtodeg(angle.y);//*=  RADIANS;
    if ( fabs( C ) > 0.005 )             /* Gimball lock? */
    {
      tr_x      =  pData[10] / C;           /* No, so get X-axis angle */
      tr_y      = -pData[6]  / C;
      angle.x  = (Real)radtodeg(atan2( tr_y, tr_x ));
      tr_x      =  pData[0] / C;            /* Get Z-axis angle */
      tr_y      = -pData[1] / C;
      angle.z  = (Real)radtodeg(atan2( tr_y, tr_x ));
    }
    else                                 /* Gimball lock has occurred */
    {
      angle.x  = 0;                      /* Set X-axis angle to zero */
      tr_x      =  pData[5];                 /* And calculate Z-axis angle */
      tr_y      =  pData[4];
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
template<> inline Quaternion Matrix4x4::getQuaternion ( void ) const
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
// Getters - Scale
template<> inline Vector3d Matrix4x4::getScaling ( void )
{
	Vector3d result;
	/*result.x = Vector3d( pData[0],pData[1],pData[2] ).magnitude();
	result.y = Vector3d( pData[4],pData[5],pData[6] ).magnitude();
	result.z = Vector3d( pData[8],pData[9],pData[10] ).magnitude();*/
	result.x = Vector3d( pData[0],pData[4],pData[8] ).magnitude();
	result.y = Vector3d( pData[1],pData[5],pData[9] ).magnitude();
	result.z = Vector3d( pData[2],pData[6],pData[10] ).magnitude();

	/* check for invalid values */
	assert( result.x == result.x );
	assert( result.x <= FLT_MAX && result.x >= -FLT_MAX );
	assert( result.y == result.y );
	assert( result.y <= FLT_MAX && result.y >= -FLT_MAX );
	assert( result.z == result.z );
	assert( result.z <= FLT_MAX && result.z >= -FLT_MAX );

	return result;
}

// Modders - Translation
template<> inline void Matrix4x4::translate ( Vector3d const& v )
{
	pData[3] += v.x;
	pData[7] += v.y;
	pData[11] += v.z;
}

//---Function definition
// Matrix transposition
template <int W, int H>
CMatrix<H,W> CMatrix<W,H>::transpose ( void )
{
	return !(*this);
}

// Matrix inverse 2x2
template<> inline CMatrix<2,2> CMatrix<2,2>::inverse ( void )
{
	Real detVal = det();
	if ( fabs( detVal ) <= 0.0005f )
	{
		return CMatrix<2,2>();
	}
	else
	{
		CMatrix<2,2> mresult;
		mresult.pData[0] = pData[3] / detVal;
		mresult.pData[3] = pData[0] / detVal;
		mresult.pData[1] = -pData[2] / detVal;
		mresult.pData[2] = -pData[1] / detVal;

		return mresult;
	}
}
// Matrix inverse general
template <int W, int H>
inline CMatrix<W,H> CMatrix<W,H>::inverse ( void )
{
	if ( W == H )
	{
		Real detVal = det();
		assert( detVal == detVal );
		assert( detVal <= FLT_MAX && detVal >= -FLT_MAX );
		if ( fabs( detVal ) <= 0.0005f )
		{
			return CMatrix<W,H>();
		}
		else
		{
			CMatrix<W,H>		mresult;
			CMatrix<W-1,H-1>	mtemp;
			int     i, j, sign;
			for ( i = 0; i < W; i++ )
			{
				for ( j = 0; j < H; j++ )
				{
					sign = 1 - ( (i +j) % 2 ) * 2;
					mtemp = submatrix( i, j );
					mresult[j][i] = ( mtemp.det() * sign ) / detVal;
				}
			}
			return mresult;
		}
	}

	return CMatrix<W,H>();
}

// Matrix determinant
template<> inline Real CMatrix<2,2>::det ( void )
{
	return ( (pData[0]*pData[3]) - (pData[1]*pData[2]) );
}
template<> inline Real CMatrix<3,3>::det ( void )
{
	return ( (pData[0]*pData[4]*pData[8]) + (pData[1]*pData[5]*pData[6]) + (pData[2]*pData[3]*pData[7])
		- (pData[2]*pData[4]*pData[6]) - (pData[1]*pData[3]*pData[8]) - (pData[0]*pData[5]*pData[7]) );
}
// Matrix determinant of arbitrary size (TODO)
template <int W, int H>
inline Real CMatrix<W,H>::det ( void )
{
	Real   det, result = 0, i = 1;
	CMatrix<W-1,H-1> msub;
	int     n;
	for ( n = 0; n < W; n++, i *= -1 )
	{
		msub	= submatrix( 0, n );
		det     = msub.det();
		result += pData[n] * det * i;
	}
	return( result );
}

// Retrieve submatrix
template <int W, int H>
CMatrix<W-1,H-1> CMatrix<W,H>::submatrix ( int ex_x, int ex_y ) const
{
	Real newData [(W-1)*(H-1)];
	static int di, dj, si, sj;
	for( di = 0; di < W-1; ++di )
	{
		for( dj = 0; dj < H-1; ++dj )
		{
			// map 3x3 element (destination) to 4x4 element (source)
			si = di + ( ( di >= ex_x ) ? 1 : 0 );
			sj = dj + ( ( dj >= ex_y ) ? 1 : 0 );
			// copy element
			newData[di * (W-1) + dj] = pData[si * W + sj];
	    }
	}

	// Create result
	CMatrix<W-1,H-1> result( newData );

	return result;
}

// Linear Interpolation
template <int W, int H>
void CMatrix<W,H>::Lerp ( CMatrix<W,H> const& right, Real const t )
{
	int x;
	for ( x = 0; x < W*H; x += 1 )
	{
		pData[x] += ( right.pData[x] - pData[x] )*t;
	}
}
template <int W, int H>
CMatrix<W,H> CMatrix<W,H>::lerp ( CMatrix<W,H> const& right, Real const t ) const
{
	CMatrix <W,H> result;
	int x;
	for ( x = 0; x < W*H; x += 1 )
	{
		result.pData[x] = pData[x] + ( right.pData[x] - pData[x] )*t;
	}
	return result;
}

//---Function Operators
// Multiplication
template <int W, int H>
template <int C>
CMatrix<C,H> CMatrix<W,H>::operator* ( CMatrix<C,W> const& right ) const
{
	CMatrix <C,H> result;
	int x, y, c;
	for ( x = 0; x < C; x += 1 )
	{
		for ( y = 0; y < H; y += 1 )
		{
			result[y][x] = 0.0;
			for ( c = 0; c < W; c += 1 )
			{
				result[y][x] += ((*this)[y][c]) * (right[c][x]);
			}
		}
	}
	for ( x = 0; x < C; x += 1 )
	{
		for ( y = 0; y < H; y += 1 )
		{
			/*if ( result[y][x] != result[y][x] )
				cout << "NAN!" << endl;
			else if (result[y][x] <= FLT_MAX && result[y][x] >= -FLT_MAX)
				cout << "INF!" << endl;*/
			assert( result[y][x] == result[y][x] );
			assert( result[y][x] <= FLT_MAX && result[y][x] >= -FLT_MAX );
		}
	}

	return result;
}
template <int W, int H>
CMatrix<W,H> CMatrix<W,H>::operator*=( CMatrix<W,H> const& right )
{
	(*this) = (*this) * right;
	return (*this);
}
/*
inline Matrix4x4 Matrix4x4::operator * ( Matrix4x4 const& right )
{
	Matrix4x4 result;
	int x, y, c;
	for ( x = 0; x < 4; x += 1 )
	{
		for ( y = 0; y < 4; y += 1 )
		{
			result[y][x] = 0.0;
			for ( c = 0; c < 4; c += 1 )
			{
				result[y][x] += ((*this)[y][c]) * (right[c][x]);
			}
		}
	}
	for ( x = 0; x < 4; x += 1 )
	{
		for ( y = 0; y < 4; y += 1 )
		{
			assert( result[y][x] == result[y][x] );
			assert( result[y][x] <= FLT_MAX && result[y][x] >= -FLT_MAX );
		}
	}

	return result;
}*/

// Multiply by a vector
inline Vector3d Matrix4x4::operator* ( Vector3d const& vect ) const
{
	return Vector3d(
		pData[0]*vect.x+pData[1]*vect.y+pData[2]*vect.z+pData[3],
		pData[4]*vect.x+pData[5]*vect.y+pData[6]*vect.z+pData[7],
		pData[8]*vect.x+pData[9]*vect.y+pData[10]*vect.z+pData[11]
		);
	/*return Vector3d(
		pData[0]*vect.x+pData[4]*vect.y+pData[8]*vect.z+pData[3],
		pData[1]*vect.x+pData[5]*vect.y+pData[9]*vect.z+pData[7],
		pData[2]*vect.x+pData[6]*vect.y+pData[10]*vect.z+pData[11]
		);*/
	/*return Vector3d(
		pData[0]*vect.x+pData[4]*vect.y+pData[8]*vect.z+pData[12],
		pData[1]*vect.x+pData[5]*vect.y+pData[9]*vect.z+pData[13],
		pData[2]*vect.x+pData[6]*vect.y+pData[10]*vect.z+pData[14]
		);*/
}

inline Vector4d Matrix4x4::operator* ( Vector4d const& vect ) const
{
	return Vector4d(
		pData[0]*vect.x+pData[1]*vect.y+pData[2]*vect.z+pData[3]*vect.w,
		pData[4]*vect.x+pData[5]*vect.y+pData[6]*vect.z+pData[7]*vect.w,
		pData[8]*vect.x+pData[9]*vect.y+pData[10]*vect.z+pData[11]*vect.w,
		pData[12]*vect.x+pData[13]*vect.y+pData[14]*vect.z+pData[15]*vect.w
		);
	/*return Vector4d(
		pData[0]*vect.x+pData[4]*vect.y+pData[8]*vect.z+pData[12]*vect.w,
		pData[1]*vect.x+pData[5]*vect.y+pData[9]*vect.z+pData[13]*vect.w,
		pData[2]*vect.x+pData[6]*vect.y+pData[10]*vect.z+pData[14]*vect.w,
		pData[3]*vect.x+pData[7]*vect.y+pData[11]*vect.z+pData[15]*vect.w
		);*/
}

// Multiply scalar
template <int W, int H>
CMatrix<W,H> CMatrix<W,H>::operator* ( Real const right ) const
{
	CMatrix <W,H> result;
	int x, y;
	for ( x = 0; x < W; x += 1 )
	{
		for ( y = 0; y < H; y += 1 )
		{
			result[y][x] = ((*this)[y][x]) * right;
		}
	}
	return result;
}

inline Matrix4x4 Matrix4x4::operator* ( Real const right ) const
{
	Matrix4x4 result;
	for ( int x = 0; x < 16; x++ )
		result.pData[x] = pData[x] * right;
	return result;
}

// Add matrices
template <int W, int H>
CMatrix<W,H> CMatrix<W,H>::operator+ ( CMatrix<W,H> const& right ) const
{
	CMatrix <W,H> result;
	int x, y;
	for ( x = 0; x < W; x += 1 )
	{
		for ( y = 0; y < H; y += 1 )
		{
			result[y][x] = ((*this)[y][x]) + (right[y][x]);
		}
	}
	return result;
}
inline Matrix4x4 Matrix4x4::operator+ ( Matrix4x4 const& right ) const
{
	CMatrix <W,H> result;
	for ( int x = 0; x < 16; x++ )
		result.pData[x] = pData[x] + right.pData[x];
	return result;
}

template <int W, int H>
CMatrix<W,H> CMatrix<W,H>::operator+= ( CMatrix<W,H> const& right )
{
	(*this) = (*this) + right;
	return (*this);
}

// Transpose Matrix
inline Matrix2x2 Matrix2x2::operator! ( void ) const
{
	Matrix2x2 result;
	result.pData[0] = this->pData[0];
	result.pData[1] = this->pData[2];
	result.pData[2] = this->pData[1];
	result.pData[3] = this->pData[3];
	return result;
}
inline Matrix3x3 Matrix3x3::operator! ( void ) const
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
inline Matrix4x4 Matrix4x4::operator! ( void ) const
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
// general transpose
template <int W, int H>
CMatrix<H,W> CMatrix<W,H>::operator! ( void ) const
{
	CMatrix <H,W> result;
	int x,y;
	for ( x = 0; x < W; x += 1 )
	{
		for ( y = 0; y < H; y += 1 )
		{
			result[x][y] = (*this)[y][x];
		}
	}
	return result;
}

// Public Accessor
template <int W, int H>
const Real* CMatrix<W,H>::operator[] ( int index ) const
{
	return &( pData[index*W] );
}

// Private Accessor and Editor
template <int W, int H>
Real* CMatrix<W,H>::operator[] ( int index )
{
	return &( pData[index*W] );
}

#endif
