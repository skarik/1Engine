//Implementation Copyright 2012 by John Choi
// Modified and improved 2013 by Joshua Boren

#include "vect2d_template.h"

namespace core
{

	//Default constructor
	template <typename REAL>
	vect2d_template<REAL>::vect2d_template()
		: x(0), y(0)
	{
		;
	}
	//Copy constructor
	template <typename REAL>
	vect2d_template<REAL>::vect2d_template (vect2d_template<REAL> const& old)
		: x(old.x), y(old.y)
	{
		;
	}
	//Conversion constructor
	template <typename REAL>
	vect2d_template<REAL>::vect2d_template (REAL const& new_x, REAL const& new_y)
		: x(new_x), y(new_y)
	{
		;
	}
	//Addition overload
	template <typename REAL>
	vect2d_template<REAL> vect2d_template<REAL>::operator+ (vect2d_template<REAL> const& right) const
	{
		return vect2d_template<REAL> (x + right.x, y + right.y);
	}

	//Addition shorthand overload
	template <typename REAL>
	void vect2d_template<REAL>::operator+= (vect2d_template<REAL> const& right)
	{
		*this = *this + right; 
	}

	//Subtraction overload
	template <typename REAL>
	vect2d_template<REAL> vect2d_template<REAL>::operator- (vect2d_template<REAL> const& right) const
	{
		return vect2d_template<REAL> (x - right.x, y - right.y);
	}

	//Subtraction shorthand overload
	template <typename REAL>
	void vect2d_template<REAL>::operator-= (vect2d_template<REAL> const& right)
	{
		*this = *this - right;
	}

	//Scalar multiplication overload
	template <typename REAL>
	vect2d_template<REAL> vect2d_template<REAL>::operator* (REAL const& right) const
	{
		return vect2d_template<REAL> (x * right, y * right);
	}

	//Scalar multiplication shorthand overload
	template <typename REAL>
	void vect2d_template<REAL>::operator*= (REAL const& right)
	{
		*this = *this * right;
	}

	//Scalar multiplication overload
	template <typename REAL>
	vect2d_template<REAL> vect2d_template<REAL>::operator/ (REAL const& right) const
	{
		return vect2d_template<REAL> (x / right, y / right);
	}

	//Scalar multiplication shorthand overload
	template <typename REAL>
	void vect2d_template<REAL>::operator/= (REAL const& right)
	{
		*this = *this / right;
	}

	//Dot product				
	template <typename REAL>
	REAL vect2d_template<REAL>::dot (vect2d_template<REAL> const& right) const
	{
		return x * right.x + y * right.y;
	}
	// Cross product
	template <typename REAL>
	REAL vect2d_template<REAL>::cross (vect2d_template<REAL> const& right) const
	{
		return x*right.y - y*right.x; 
	}

	template <typename REAL>
	REAL& vect2d_template<REAL>::operator[] ( const int a )						//Index Accessor Added: 6/4/13
	{
		return *(&x + a);
	}
	template <typename REAL>
	const REAL& vect2d_template<REAL>::operator[] ( const int a ) const			// 6/4/13
	{
		return *(&x + a);
	}


	template <typename REAL>
	REAL vect2d_template<REAL>::magnitude (void) const
	{
		return (REAL) sqrt (x*x + y*y);
	}

	template <typename REAL>
	REAL vect2d_template<REAL>::sqrMagnitude (void) const
	{
		return (x*x + y*y);
	}

	template <typename REAL>
	vect2d_template<REAL> vect2d_template<REAL>::normal ( void ) const
	{
		REAL invMagnitude = magnitude();
		if ( fabs(invMagnitude) <= FTYPE_PRECISION ) {
			return vect2d_template<REAL>(0,0);
		}
		invMagnitude = 1.0f / invMagnitude;
		return (*this)*invMagnitude;
	}
	template <typename REAL>
	void vect2d_template<REAL>::normalize ( void )
	{
		(*this) = normal();
	}
	// Integer specialization
	template <>
	vect2d_template<int32_t> vect2d_template<int32_t>::normal ( void ) const
	{
		return vect2d_template<int32_t>( 0,0 );
	}

	// Note this doesn't edit the values
	template <typename REAL>
	vect2d_template<REAL> vect2d_template<REAL>::lerp (vect2d_template<REAL> const& right, REAL const& t) const
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
	template <typename REAL>
	bool vect2d_template<REAL>::operator== (vect2d_template<REAL> const& right) const
	{
		if (fabs(right.x - x) > FTYPE_PRECISION)
			return false;
		if (fabs(right.y - y) > FTYPE_PRECISION)
			return false;
		return true;
	}
	template <>
	bool vect2d_template<int32_t>::operator== ( vect2d_template<int32_t> const& right ) const
	{
		return right.x == x && right.y == y;
	}
	//Not equal comparison overload
	template <typename REAL>
	bool vect2d_template<REAL>::operator!= (vect2d_template<REAL> const& right) const
	{
		return !((*this) == right);
	}
	//Component-based comparison for lookups
	template <typename REAL>
	bool vect2d_template<REAL>::operator<  (vect2d_template<REAL> const& right) const
	{
		if ( x > right.x ) return false;
		else if ( x == right.x ) {
			if ( y >= right.y ) return false;
		}
		return true;
	}


	// prototypes
	template class vect2d_template<float>;
	template class vect2d_template<double>;
	template class vect2d_template<int32_t>;

}