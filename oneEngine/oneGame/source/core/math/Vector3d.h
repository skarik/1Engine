//	class Vector3d
// 3d vector math class

#ifndef VECTOR3D_H
#define VECTOR3D_H

#include "core/types/types.h"
#include "core/types/float.h"
#include <iostream>
#include <cmath>

class Matrix3x3;
class Matrix4x4;

class Vector2f;
class Vector4d;

class Vector3d 
{
	friend std::ostream& operator<< (std::ostream& out, Vector3d const& current);		//Out stream overload
	
public:
	//Default constructor
	FORCE_INLINE Vector3d ( void );
	//Appending constructor
	Vector3d (Vector2f const& old, Real const new_z=0);
	//Copy constructor
	FORCE_INLINE Vector3d (Vector3d const& old);
	//Truncating constructor
	Vector3d (Vector4d const& old);
	//Component input constructor
	FORCE_INLINE Vector3d (Real const& new_x, Real const& new_y, Real const& new_z);
	// Array input constructor Added: 8/30/12
	FORCE_INLINE Vector3d (const Real* arr);

	//Addition
	FORCE_INLINE Vector3d operator+ (Vector3d const& right) const;
	//Addition shorthand
	FORCE_INLINE void operator+= (Vector3d const& right);
	//Subtraction
	FORCE_INLINE Vector3d operator- (Vector3d const& right) const;
	//Subtraction shorthand
	FORCE_INLINE void operator-= (Vector3d const& right);
	//Scalar multiplication
	FORCE_INLINE Vector3d operator* (Real const& right) const;
	//Scalar multiplication shorthand
	FORCE_INLINE void operator*= (Real const& right);
	//Scalar division
	FORCE_INLINE Vector3d operator/ (Real const& right) const;
	//Scalar division shorthand
	FORCE_INLINE void operator/= (Real const& right);
	//Negation Added: 2/24/12
	FORCE_INLINE Vector3d operator- () const;
	//Dot product
	FORCE_INLINE Real dot (Vector3d const& right) const;	
	//Cross product
	FORCE_INLINE Vector3d cross (Vector3d const& right) const;
	//Linear interpolation Added: 9/8/12 at 9:12 am
	// Note: this doesn't edit the values
	FORCE_INLINE Vector3d lerp (Vector3d const& right, Real const& t) const;

	// Normal of a vector
	FORCE_INLINE Vector3d normal ( void ) const;
	// Normalize vector
	FORCE_INLINE void normalize ( void );
	//Returns the magnitude of the vector
	FORCE_INLINE Real magnitude (void) const;
	//Returns the squared magnitude of the vector
	FORCE_INLINE Real sqrMagnitude (void) const;

	// Index accessor
	FORCE_INLINE Real& operator[] ( const int );
	FORCE_INLINE const Real& operator[] ( const int ) const;

	//Direction to euler angle vector
	Vector3d toEulerAngles ( void ) const;

	// Matrix modify
	Vector3d rvrMultMatx ( Matrix4x4 const& right ) const;
	Vector3d rvrMultMatx ( Matrix3x3 const& right ) const;

	// Component multiply
	Vector3d mulComponents ( Vector3d const& right ) const;
	Vector3d divComponents ( Vector3d const& right ) const;

	//Equal comparison
	FORCE_INLINE bool operator== (Vector3d const& right) const;
	//Not equal comparison
	FORCE_INLINE bool operator!= (Vector3d const& right) const;	

public:
	Real x;
	Real y;
	Real z;

public:
	// A vector of value Vector3d( 0,0,0 )
	static const Vector3d zero;
	// A vector of value Vector3d::forward
	static const Vector3d forward;
	// A vector of value Vector3d( 0,0,1 )
	static const Vector3d up;
	// A vector of value Vector3d( 0,1,0 )
	static const Vector3d left;

};
typedef Vector3d Vector3f;

//===============================================================================================//
// Vector3d implementation
//===============================================================================================//

//Default constructor
FORCE_INLINE Vector3d::Vector3d()
	: x(0.0), y(0.0), z(0.0)
{
	;
}
//Copy constructor
FORCE_INLINE Vector3d::Vector3d (Vector3d const& old)
	: x(old.x), y(old.y), z(old.z)
{
	;
}
//Component input constructor
FORCE_INLINE Vector3d::Vector3d (Real const& new_x, Real const& new_y, Real const& new_z)
	: x(new_x), y(new_y), z(new_z)
{
	;
}
//Array input constructor
FORCE_INLINE Vector3d::Vector3d (const Real* arr) 
	: x(0), y(0), z(0)
{
	if (arr != NULL) {
		x = arr[0];
		y = arr[1];
		z = arr[2];
	}
}

//Addition overload
FORCE_INLINE Vector3d Vector3d::operator+ (Vector3d const& right) const
{
	return Vector3d( x+right.x, y+right.y, z+right.z );
}
//Addition shorthand overload
FORCE_INLINE void Vector3d::operator+= (Vector3d const& right)
{
	*this = *this + right; 
}
//Subtraction overload
FORCE_INLINE Vector3d Vector3d::operator- (Vector3d const& right) const
{
	return Vector3d( x-right.x, y-right.y, z-right.z );
}
//Subtraction shorthand overload
FORCE_INLINE void Vector3d::operator-= (Vector3d const& right)
{
	*this = *this - right;
}
//Scalar multiplication overload
FORCE_INLINE Vector3d Vector3d::operator* (Real const& right) const
{
	return Vector3d( x*right, y*right, z*right );
}

//Scalar multiplication shorthand overload
FORCE_INLINE void Vector3d::operator*= (Real const& right)
{
	*this = *this * right;
}
//Scalar division overload		Added: 5/3/2012
FORCE_INLINE Vector3d Vector3d::operator/ (Real const& right) const
{
	return Vector3d( x/right, y/right, z/right );
}
//Scalar division shorthand overload		Added: 5/3/2012
FORCE_INLINE void Vector3d::operator/= (Real const& right)
{
	*this = *this / right;
}
FORCE_INLINE Vector3d Vector3d::operator- ( void ) const
{
	return Vector3d( -x,-y,-z );
}
//Dot product    Added: 1/4/12
FORCE_INLINE Real Vector3d::dot (Vector3d const& right) const
{
 return x * right.x + y * right.y + z * right.z;
}
//Cross product    Added: 1/4/12
FORCE_INLINE Vector3d Vector3d::cross (Vector3d const& right) const
{
 return Vector3d ((y * right.z - z * right.y), (z * right.x - x * right.z), (x * right.y - y * right.x));
}
//Linear interpolation Added: 9/8/12 at 9:12
FORCE_INLINE Vector3d Vector3d::lerp (Vector3d const& right, Real const& t) const
{
	if(t <= 0)
	{
		return (*this);
	}
	else if(t >= 1)
	{
		return right;
	}
	else
	{
		return ((right - (*this))*t + (*this));
	}
}

FORCE_INLINE Vector3d Vector3d::normal ( void ) const
{
	Real invMagnitude = magnitude();
	if ( fabs(invMagnitude) <= FTYPE_PRECISION ) {
		return Vector3d::zero;
	}
	invMagnitude = 1.0f / invMagnitude;
	return (*this)*invMagnitude;
}
FORCE_INLINE void Vector3d::normalize ( void )
{
	(*this) = normal();
}
FORCE_INLINE Real Vector3d::magnitude (void) const
{
	return sqrt (x*x + y*y + z*z);
}
FORCE_INLINE Real Vector3d::sqrMagnitude (void) const
{
	return (x*x + y*y + z*z);
}

//Index Accessor Added: 5/3/12
FORCE_INLINE Real& Vector3d::operator[] ( const int a )
{
	return *(&x + a);
}
//Index Accessor Added: 5/3/12
FORCE_INLINE const Real& Vector3d::operator[] ( const int a ) const
{
	return *(&x + a);
}

//Equal comparison overload
FORCE_INLINE bool Vector3d::operator== (Vector3d const& right) const
{
	if (fabs(right.x - x) > FTYPE_PRECISION)
		return false;
	if (fabs(right.y - y) > FTYPE_PRECISION)
		return false;
	if (fabs(right.z - z) > FTYPE_PRECISION)
		return false;
	return true;
}
//Not equal comparison overload
FORCE_INLINE bool Vector3d::operator!= (Vector3d const& right) const
{
	return !((*this) == right);
}


#endif//VECTOR3D_H