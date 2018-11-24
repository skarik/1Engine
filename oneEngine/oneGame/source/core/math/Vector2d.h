//===============================================================================================//
//
//		class Vector2[f/d]
//
// POD color class, 16 bytes.
// Represents a 4-component floating point color.
//
//===============================================================================================//
#ifndef CORE_MATH_VECTOR2_H_
#define CORE_MATH_VECTOR2_H_

#include "core/types/types.h"
#include "core/types/float.h"
#include <iostream>
#include <cmath>

class Vector3d;
class Matrix2x2;

// 2D Vector Math Class
class Vector2f 
{
	// Out stream overload
	friend std::ostream& operator<< (std::ostream& out, Vector2f const& current);
	
public:
	FORCE_INLINE			Vector2f (void);
	FORCE_INLINE			Vector2f (Vector2f const& old);
							Vector2f (Vector3d const& old);
	FORCE_INLINE			Vector2f (Real const& new_x, Real const& new_y);

	FORCE_INLINE Vector2f	operator+ (Vector2f const& right) const;
	FORCE_INLINE void		operator+= (Vector2f const& right);
	FORCE_INLINE Vector2f	operator- (Vector2f const& right) const;	
	FORCE_INLINE void		operator-= (Vector2f const& right);
	FORCE_INLINE Vector2f	operator* (Real const& right) const;
	FORCE_INLINE void		operator*= (Real const& right);
	FORCE_INLINE Vector2f	operator/ (Real const& right) const;
	FORCE_INLINE void		operator/= (Real const& right);
	FORCE_INLINE Vector2f	operator- () const;
	FORCE_INLINE Vector2f	lerp (Vector2f const& right, Real const& t) const;	
	FORCE_INLINE Real		dot (Vector2f const& right) const;
	FORCE_INLINE Real		cross (Vector2f const& right) const;

	FORCE_INLINE Real		magnitude (void) const;
	FORCE_INLINE Real		sqrMagnitude (void) const;

	FORCE_INLINE Vector2f	normal ( void ) const;
	FORCE_INLINE void		normalize ( void );

	FORCE_INLINE Real&		operator[] ( const int );
	FORCE_INLINE const Real&
							operator[] ( const int ) const;

	Vector2f				rvrMultMatx ( Matrix2x2 const& right ) const;
	Vector2f				mulComponents ( Vector2f const& right ) const;
	Vector2f				divComponents ( Vector2f const& right ) const;

	FORCE_INLINE bool		operator== (Vector2f const& right) const;
	FORCE_INLINE bool		operator!= (Vector2f const& right) const;

public:
	union
	{
		struct 
		{
			Real x;
			Real y;
		};
		struct
		{
			Real raw [2];
		};
	};
};
typedef Vector2f Vector2f;


//===============================================================================================//
// Vector2f implementation
//===============================================================================================//

//Default constructor
FORCE_INLINE Vector2f::Vector2f()
	: x(0), y(0)
{
	;
}
//Copy constructor
FORCE_INLINE Vector2f::Vector2f (Vector2f const& old)
	: x(old.x), y(old.y)
{
	;
}
//Conversion constructor
FORCE_INLINE Vector2f::Vector2f (Real const& new_x, Real const& new_y)
	: x(new_x), y(new_y)
{
	;
}

//Addition
FORCE_INLINE Vector2f Vector2f::operator+ (Vector2f const& right) const
{
	return Vector2f (x + right.x, y + right.y);
}
//Addition shorthand 
FORCE_INLINE void Vector2f::operator+= (Vector2f const& right)
{
	*this = *this + right; 
}
//Subtraction
FORCE_INLINE Vector2f Vector2f::operator- (Vector2f const& right) const
{
	return Vector2f (x - right.x, y - right.y);
}
//Subtraction shorthand
FORCE_INLINE void Vector2f::operator-= (Vector2f const& right)
{
	*this = *this - right;
}
//Scalar multiplication overload
FORCE_INLINE Vector2f Vector2f::operator* (Real const& right) const
{
	return Vector2f (x * right, y * right);
}
//Scalar multiplication shorthand overload
FORCE_INLINE void Vector2f::operator*= (Real const& right)
{
	*this = *this * right;
}
//Scalar multiplication overload
FORCE_INLINE Vector2f Vector2f::operator/ (Real const& right) const
{
	return Vector2f (x / right, y / right);
}
//Scalar multiplication shorthand overload
FORCE_INLINE void Vector2f::operator/= (Real const& right)
{
	*this = *this / right;
}
// Negation
FORCE_INLINE Vector2f Vector2f::operator- () const						//Negation Added: 3/15/14
{
	return Vector2f(-x,-y);
}
//Dot product				
FORCE_INLINE Real Vector2f::dot (Vector2f const& right) const
{
	return x * right.x + y * right.y;
}
// Cross product
FORCE_INLINE Real Vector2f::cross (Vector2f const& right) const
{
	return x*right.y - y*right.x; 
}
//Index Accessor
FORCE_INLINE Real& Vector2f::operator[] ( const int a )
{
	return *(&x + a);
}
FORCE_INLINE const Real& Vector2f::operator[] ( const int a ) const	
{
	return *(&x + a);
}



FORCE_INLINE Real Vector2f::magnitude (void) const
{
	return (Real) sqrt (x*x + y*y);
}

FORCE_INLINE Real Vector2f::sqrMagnitude (void) const
{
	return (x*x + y*y);
}

FORCE_INLINE Vector2f Vector2f::normal ( void ) const
{
	Real invMagnitude = magnitude();
	if ( fabs(invMagnitude) <= 1.0e-7 ) {
		return Vector2f(0,0);
	}
	invMagnitude = 1.0f / invMagnitude;
	return (*this)*invMagnitude;
}
FORCE_INLINE void Vector2f::normalize ( void )
{
	(*this) = normal();
}

// Note this doesn't edit the values
FORCE_INLINE Vector2f Vector2f::lerp (Vector2f const& right, Real const& t) const
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
FORCE_INLINE bool Vector2f::operator== (Vector2f const& right) const
{
	if (fabs(right.x - x) > FTYPE_PRECISION)
		return false;
	if (fabs(right.y - y) > FTYPE_PRECISION)
		return false;
	return true;
}

//Not equal comparison overload
FORCE_INLINE bool Vector2f::operator!= (Vector2f const& right) const
{
	return !((*this) == right);
}



#endif//CORE_MATH_VECTOR2_H_