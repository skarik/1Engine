
#include "vect3d_template.h"

using namespace Engine;

//Default constructor
template <typename REAL>
vect3d_template<REAL>::vect3d_template()
	: x(0), y(0), z(0)
{
	;
}
//Copy constructor
template <typename REAL>
vect3d_template<REAL>::vect3d_template (vect3d_template<REAL> const& old)
	: x(old.x), y(old.y), z(old.z)
{
	;
}
//Component input constructor
template <typename REAL>
vect3d_template<REAL>::vect3d_template (REAL const& new_x, REAL const& new_y, REAL const& new_z)
	: x(new_x), y(new_y), z(new_z)
{
	;
}
// Array construction
template <typename REAL>
vect3d_template<REAL>::vect3d_template (const REAL* arr) 
{
	x = arr[0];
	y = arr[1];
	z = arr[2];
}

//Addition overload
template <typename REAL>
vect3d_template<REAL> vect3d_template<REAL>::operator+ (vect3d_template<REAL> const& right) const
{
	vect3d_template<REAL> temp;

	temp.x = x + right.x;
	temp.y = y + right.y;
	temp.z = z + right.z;

	return temp;
}
//Addition shorthand overload
template <typename REAL>
void vect3d_template<REAL>::operator+= (vect3d_template<REAL> const& right)
{
	*this = *this + right; 
}
//Subtraction overload
template <typename REAL>
vect3d_template<REAL> vect3d_template<REAL>::operator- (vect3d_template<REAL> const& right) const
{
	vect3d_template<REAL> temp;

	temp.x = x - right.x;
	temp.y = y - right.y;
	temp.z = z - right.z;

	return temp;
}
//Subtraction shorthand overload
template <typename REAL>
void vect3d_template<REAL>::operator-= (vect3d_template<REAL> const& right)
{
	*this = *this - right;
}
//Scalar multiplication overload
template <typename REAL>
vect3d_template<REAL> vect3d_template<REAL>::operator* (REAL const& right) const
{
	vect3d_template<REAL> temp;

	temp.x = x * right;
	temp.y = y * right;
	temp.z = z * right;

	return temp;
}
//Scalar multiplication shorthand overload
template <typename REAL>
void vect3d_template<REAL>::operator*= (REAL const& right)
{
	*this = *this * right;
}
//Scalar division overload		Added: 5/3/2012
template <typename REAL>
vect3d_template<REAL> vect3d_template<REAL>::operator/ (REAL const& right) const
{
	vect3d_template<REAL> temp;

	temp.x = x / right;
	temp.y = y / right;
	temp.z = z / right;

	return temp;
}
//Scalar division shorthand overload		Added: 5/3/2012
template <typename REAL>
void vect3d_template<REAL>::operator/= (REAL const& right)
{
	*this = *this / right;
}
//Dot product    Added: 1/4/12
template <typename REAL>
REAL vect3d_template<REAL>::dot (vect3d_template<REAL> const& right) const
{
 return x * right.x + y * right.y + z * right.z;
}

//Cross product    Added: 1/4/12
template <typename REAL>
vect3d_template<REAL> vect3d_template<REAL>::cross (vect3d_template<REAL> const& right) const
{
 return vect3d_template<REAL> ((y * right.z - z * right.y), (z * right.x - x * right.z), (x * right.y - y * right.x));
}

template <typename REAL>
REAL vect3d_template<REAL>::magnitude (void) const
{
	return (REAL) sqrt (x*x + y*y + z*z);
}
template <typename REAL>
REAL vect3d_template<REAL>::sqrMagnitude (void) const
{
	return (x*x + y*y + z*z);
}

template <typename REAL>
vect3d_template<REAL> vect3d_template<REAL>::normal ( void ) const
{
	REAL invMagnitude = magnitude();

	if ( fabs(invMagnitude) <= 1.0e-7 ) {
		return vect3d_template<REAL>(0,0,0);
	}
	invMagnitude = 1.0f / invMagnitude;
	return (*this)*invMagnitude;
}
template <typename REAL>
void vect3d_template<REAL>::normalize ( void )
{
	(*this) = normal();
}
// Integer specialization
template <>
vect3d_template<int32_t> vect3d_template<int32_t>::normal ( void ) const
{
	return vect3d_template<int32_t>( 0,0,0 );
}

template <typename REAL>
vect3d_template<REAL> vect3d_template<REAL>::operator- ( void ) const
{
	return vect3d_template<REAL>( -x,-y,-z );
}

//Index Accessor Added: 5/3/12
template <typename REAL>
REAL& vect3d_template<REAL>::operator[] ( const int a )
{
	return *(&x + a);
}
//Index Accessor Added: 5/3/12
template <typename REAL>
const REAL& vect3d_template<REAL>::operator[] ( const int a ) const
{
	return *(&x + a);
}

//Linear interpolation Added: 9/8/12 at 9:12
template <typename REAL>
vect3d_template<REAL> vect3d_template<REAL>::lerp (vect3d_template<REAL> const& right, REAL const& t) const
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

//Equal comparison overload
template <typename REAL>
bool vect3d_template<REAL>::operator== (vect3d_template<REAL> const& right) const
{
	if (fabs(right.x - x) > FTYPE_PRECISION)
		return false;
	if (fabs(right.y - y) > FTYPE_PRECISION)
		return false;
	if (fabs(right.z - z) > FTYPE_PRECISION)
		return false;
	return true;
}
template <>
bool vect3d_template<int32_t>::operator== ( vect3d_template<int32_t> const& right ) const
{
	return right.x == x && right.y == y && right.z == z;
}
//Not equal comparison overload
template <typename REAL>
bool vect3d_template<REAL>::operator!= (vect3d_template<REAL> const& right) const
{
	return !((*this) == right);
}
//Component-based comparison for lookups
template <typename REAL>
bool vect3d_template<REAL>::operator<  (vect3d_template<REAL> const& right) const
{
	if ( x > right.x ) return false;
	else if ( x == right.x ) {
		if ( y > right.y ) return false;
		else if ( y == right.y ) {
			if ( z >= right.z ) return false;
		}
	}
	return false;
}

// prototypes
template class vect3d_template<float>;
template class vect3d_template<double>;
template class vect3d_template<int32_t>;
