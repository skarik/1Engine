#ifndef _X_ROTATION_H_
#define _X_ROTATION_H_

#include <algorithm>
#include "Vector3d.h"
#include "core/types/float.h"
#include "Quaternion.h"
#include "core/math/Matrix/CMatrix.h"

class Rotator : public Matrix3x3
{
	friend std::ostream& operator<< (std::ostream& out, Rotator const& matx);		//Out stream overload

public: // Constructors
	FORCE_INLINE Rotator( void );
	FORCE_INLINE Rotator( const Matrix3x3& matx );
	FORCE_INLINE Rotator( Real const& eulerX, Real const& eulerY, Real const& eulerZ );
	FORCE_INLINE Rotator( const Vector3d& euler_angles );
	FORCE_INLINE Rotator( const Quaternion& quat );

public: // Setters
	// Sets the rotator to describing a rotation around an axis
	FORCE_INLINE void AxisAngle ( const Vector3d& axis, const Real angle );	
	// Sets the rotator to describing a rotation from vector a to b
	FORCE_INLINE void RotationTo ( const Vector3d& vfrom, const Vector3d& vto );
	// Sets the matrix from an array
	FORCE_INLINE void Set ( Real * data );

	// Resets matrix to identity
	FORCE_INLINE void SetIdentity ( void );
	
	// Switches the matrix bases
	FORCE_INLINE Rotator& SwitchBasis ( void );

public: // Inline Setters
	// Set Euler angles
	FORCE_INLINE void Euler ( const Vector3d& angles ) {
		setRotation( angles );
	}

public: // Operators
	// Assignment operator
	FORCE_INLINE Rotator& operator= ( const Matrix3x3& );

};

//===============================================================================================//
// Rotator implementation
//===============================================================================================//

// Constructors
FORCE_INLINE Rotator::Rotator ( void )
{
	SetIdentity();
}
FORCE_INLINE Rotator::Rotator ( const Matrix3x3& matx )
{
	pData[0] = matx.pData[0];
	pData[1] = matx.pData[1];
	pData[2] = matx.pData[2];
	pData[3] = matx.pData[3];
	pData[4] = matx.pData[4];
	pData[5] = matx.pData[5];
	pData[6] = matx.pData[6];
	pData[7] = matx.pData[7];
	pData[8] = matx.pData[8];
}
FORCE_INLINE Rotator::Rotator( Real const& eulerX, Real const& eulerY, Real const& eulerZ )
{
	setRotation( eulerX, eulerY, eulerZ );
}
FORCE_INLINE Rotator::Rotator( const Vector3d& euler_angles )
{
	setRotation( euler_angles );
}
FORCE_INLINE Rotator::Rotator( const Quaternion& quat )
{
	setRotation( quat );
}

// Sets the matrix to describing a rotation around an axis
FORCE_INLINE void Rotator::AxisAngle ( const Vector3d& axis, const Real angle )
{
	Quaternion temp;
	temp.AxisAngle( axis, angle );
	setRotation( temp );
}
// Sets the matrix to descrive a rotation from vector a to b
FORCE_INLINE void Rotator::RotationTo ( const Vector3d& vfrom, const Vector3d& vto )
{
	Vector3d axis = vfrom.cross(vto).normal();
	Real angle = acos( std::min<Real>( 1, std::max<Real>( -1, vfrom.normal().dot(vto.normal()) ) ) );
	AxisAngle( axis, (Real)radtodeg(angle) );
}
// Sets the matrix from an array
FORCE_INLINE void Rotator::Set ( Real * data )
{
	pData[0] = data[0];
	pData[1] = data[1];
	pData[2] = data[2];
	pData[3] = data[3];
	pData[4] = data[4];
	pData[5] = data[5];
	pData[6] = data[6];
	pData[7] = data[7];
	pData[8] = data[8];
}

// Resets matrix to identity
FORCE_INLINE void Rotator::SetIdentity ( void )
{
	pData[0] = 1;
	pData[1] = 0;
	pData[2] = 0;
	pData[3] = 0;
	pData[4] = 1;
	pData[5] = 0;
	pData[6] = 0;
	pData[7] = 0;
	pData[8] = 1;
}

// Switches the matrix bases (right handed-rot to left handed-rot)
FORCE_INLINE Rotator& Rotator::SwitchBasis ( void )
{
	/*Real arr [] = {-1,0,0, 0,1,0, 0,0,1};
	Matrix3x3 matx ( arr );
	*this *= matx;*/
	pData[0] = -pData[0];
	pData[3] = -pData[3];
	pData[6] = -pData[6];
	return *this;
}

// Assignment operator
FORCE_INLINE Rotator& Rotator::operator= ( const Matrix3x3& right )
{
	pData[0] = right.pData[0];
	pData[1] = right.pData[1];
	pData[2] = right.pData[2];
	pData[3] = right.pData[3];
	pData[4] = right.pData[4];
	pData[5] = right.pData[5];
	pData[6] = right.pData[6];
	pData[7] = right.pData[7];
	pData[8] = right.pData[8];
	return *this;
}

#endif//_X_ROTATION_H_