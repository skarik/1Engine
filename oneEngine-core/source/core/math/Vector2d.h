//	class Vector2d
// 2D vector math class

#ifndef _CORE_VECTOR2D_H_
#define _CORE_VECTOR2D_H_

#include "core/types/types.h"
#include "core/types/float.h"
#include <iostream>
#include <cmath>

class Vector3d;
class Matrix2x2;

class Vector2d 
{
	friend std::ostream& operator<< (std::ostream& out, Vector2d const& current);		//Out stream overload
	
public:
	//Default constructor
	FORCE_INLINE Vector2d (void);
	//Copy constructor
	FORCE_INLINE Vector2d (Vector2d const& old);
	//Truncating constructor
	Vector2d (Vector3d const& old);
	//Input conversion constructor
	FORCE_INLINE Vector2d (Real const& new_x, Real const& new_y);

	//Addition
	FORCE_INLINE Vector2d operator+ (Vector2d const& right) const;
	//Addition shorthand	
	FORCE_INLINE void operator+= (Vector2d const& right);
	//Subtraction
	FORCE_INLINE Vector2d operator- (Vector2d const& right) const;	
	//Subtraction shorthand
	FORCE_INLINE void operator-= (Vector2d const& right);
	//Scalar multiplication
	FORCE_INLINE Vector2d operator* (Real const& right) const;
	//Scalar multiplication shorthand
	FORCE_INLINE void operator*= (Real const& right);
	//Scalar division
	FORCE_INLINE Vector2d operator/ (Real const& right) const;
	//Scalar division shorthand
	FORCE_INLINE void operator/= (Real const& right);
	// Vector negation
	FORCE_INLINE Vector2d operator- () const;
	// Linear interpolation
	// Note this doesn't edit the values
	FORCE_INLINE Vector2d lerp (Vector2d const& right, Real const& t) const;	
	//Dot product
	FORCE_INLINE Real dot (Vector2d const& right) const;
	// Cross product 10/17/13
	FORCE_INLINE Real cross (Vector2d const& right) const;

	//Returns the magnitude of the vector
	FORCE_INLINE Real magnitude (void) const;
	//Returns squared magnitude for fast comparisons
	FORCE_INLINE Real sqrMagnitude (void) const;

	//Normal of vector
	FORCE_INLINE Vector2d normal ( void ) const;
	//Normalize vector
	FORCE_INLINE void normalize ( void );

	// Index accessor
	FORCE_INLINE Real& operator[] ( const int );
	FORCE_INLINE const Real& operator[] ( const int ) const;

	// Matrix modify
	Vector2d rvrMultMatx ( Matrix2x2 const& right ) const;
	// Component multiply
	Vector2d mulComponents ( Vector2d const& right ) const;

	//Equal comparison
	FORCE_INLINE bool operator== (Vector2d const& right) const;
	//Not equal comparison
	FORCE_INLINE bool operator!= (Vector2d const& right) const;

public:
	Real x;
	Real y;
};


//===============================================================================================//
// Vector2d implementation
//===============================================================================================//

//Default constructor
FORCE_INLINE Vector2d::Vector2d()
	: x(0), y(0)
{
	;
}
//Copy constructor
FORCE_INLINE Vector2d::Vector2d (Vector2d const& old)
	: x(old.x), y(old.y)
{
	;
}
//Conversion constructor
FORCE_INLINE Vector2d::Vector2d (Real const& new_x, Real const& new_y)
	: x(new_x), y(new_y)
{
	;
}

//Addition
FORCE_INLINE Vector2d Vector2d::operator+ (Vector2d const& right) const
{
	return Vector2d (x + right.x, y + right.y);
}
//Addition shorthand 
FORCE_INLINE void Vector2d::operator+= (Vector2d const& right)
{
	*this = *this + right; 
}
//Subtraction
FORCE_INLINE Vector2d Vector2d::operator- (Vector2d const& right) const
{
	return Vector2d (x - right.x, y - right.y);
}
//Subtraction shorthand
FORCE_INLINE void Vector2d::operator-= (Vector2d const& right)
{
	*this = *this - right;
}
//Scalar multiplication overload
FORCE_INLINE Vector2d Vector2d::operator* (Real const& right) const
{
	return Vector2d (x * right, y * right);
}
//Scalar multiplication shorthand overload
FORCE_INLINE void Vector2d::operator*= (Real const& right)
{
	*this = *this * right;
}
//Scalar multiplication overload
FORCE_INLINE Vector2d Vector2d::operator/ (Real const& right) const
{
	return Vector2d (x / right, y / right);
}
//Scalar multiplication shorthand overload
FORCE_INLINE void Vector2d::operator/= (Real const& right)
{
	*this = *this / right;
}
// Negation
FORCE_INLINE Vector2d Vector2d::operator- () const						//Negation Added: 3/15/14
{
	return Vector2d(-x,-y);
}
//Dot product				
FORCE_INLINE Real Vector2d::dot (Vector2d const& right) const
{
	return x * right.x + y * right.y;
}
// Cross product
FORCE_INLINE Real Vector2d::cross (Vector2d const& right) const
{
	return x*right.y - y*right.x; 
}
//Index Accessor
FORCE_INLINE Real& Vector2d::operator[] ( const int a )
{
	return *(&x + a);
}
FORCE_INLINE const Real& Vector2d::operator[] ( const int a ) const	
{
	return *(&x + a);
}



FORCE_INLINE Real Vector2d::magnitude (void) const
{
	return (Real) sqrt (x*x + y*y);
}

FORCE_INLINE Real Vector2d::sqrMagnitude (void) const
{
	return (x*x + y*y);
}

FORCE_INLINE Vector2d Vector2d::normal ( void ) const
{
	Real invMagnitude = magnitude();
	if ( fabs(invMagnitude) <= 1.0e-7 ) {
		return Vector2d(0,0);
	}
	invMagnitude = 1.0f / invMagnitude;
	return (*this)*invMagnitude;
}
FORCE_INLINE void Vector2d::normalize ( void )
{
	(*this) = normal();
}

// Note this doesn't edit the values
FORCE_INLINE Vector2d Vector2d::lerp (Vector2d const& right, Real const& t) const
{
	if (t <= 0) {
		return (*this);
	}
	else if (t >= 1) {
		return right;
	}
	else {
		return ((right - (*this))*t + (*this));
	}
}


//Equal comparison overload
FORCE_INLINE bool Vector2d::operator== (Vector2d const& right) const
{
	if (fabs(right.x - x) > FTYPE_PRECISION)
		return false;
	if (fabs(right.y - y) > FTYPE_PRECISION)
		return false;
	return true;
}

//Not equal comparison overload
FORCE_INLINE bool Vector2d::operator!= (Vector2d const& right) const
{
	return !((*this) == right);
}



#endif//_CORE_VECTOR2D_H_