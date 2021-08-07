#include "vect4d_template.h"
#include "vect3d_template.h"
#include "vect2d_template.h"

namespace core
{
	template <typename REAL>
	vect4d_template<REAL>::vect4d_template(void)
		: x(0), y(0), z(0), w(0)
	{
		;
	}

	template <typename REAL>
	vect4d_template<REAL>::vect4d_template (vect2d_template<REAL> const& old, const REAL& new_z, const REAL& new_w)
		: x(old.x), y(old.y), z(new_z), w(new_w)
	{
		;
	}

	template <typename REAL>
	vect4d_template<REAL>::vect4d_template (vect3d_template<REAL> const& old, const REAL& new_w)
		: x(old.x), y(old.y), z(old.z), w(new_w)
	{
		;
	}

	template <typename REAL>
	vect4d_template<REAL>::vect4d_template (vect4d_template<REAL> const& old)
		: x(old.x), y(old.y), z(old.z), w(old.w)
	{
		;
	}

	template <typename REAL>
	vect4d_template<REAL>::vect4d_template (REAL const& new_x, REAL const& new_y, REAL const& new_z, REAL const& new_w)
		: x(new_x), y(new_y), z(new_z), w(new_w)
	{
		;
	}
	// Array construction
	template <typename REAL>
	vect4d_template<REAL>::vect4d_template (const REAL* arr) 
	{
		x = arr[0];
		y = arr[1];
		z = arr[2];
		w = arr[3];
	}

	//Addition overload
	template <typename REAL>
	vect4d_template<REAL> vect4d_template<REAL>::operator+ (vect4d_template<REAL> const& right) const
	{
		vect4d_template<REAL> temp;

		temp.x = x + right.x;
		temp.y = y + right.y;
		temp.z = z + right.z;
		temp.w = w + right.w;

		return temp;
	}
	//Addition shorthand overload
	template <typename REAL>
	void vect4d_template<REAL>::operator+= (vect4d_template<REAL> const& right)
	{
		*this = *this + right; 
	}
	//Subtraction overload
	template <typename REAL>
	vect4d_template<REAL> vect4d_template<REAL>::operator- (vect4d_template<REAL> const& right) const
	{
		vect4d_template<REAL> temp;

		temp.x = x - right.x;
		temp.y = y - right.y;
		temp.z = z - right.z;
		temp.w = w - right.z;

		return temp;
	}
	//Subtraction shorthand overload
	template <typename REAL>
	void vect4d_template<REAL>::operator-= (vect4d_template<REAL> const& right)
	{
		*this = *this - right;
	}
	//Scalar multiplication overload
	template <typename REAL>
	vect4d_template<REAL> vect4d_template<REAL>::operator* (REAL const& right) const
	{
		vect4d_template<REAL> temp;

		temp.x = x * right;
		temp.y = y * right;
		temp.z = z * right;
		temp.w = w * right;

		return temp;
	}
	//Scalar multiplication shorthand overload
	template <typename REAL>
	void vect4d_template<REAL>::operator*= (REAL const& right)
	{
		*this = *this * right;
	}
	template <typename REAL>
	vect4d_template<REAL> vect4d_template<REAL>::operator/ (REAL const& right) const
	{
		vect4d_template<REAL> temp;

		temp.x = x / right;
		temp.y = y / right;
		temp.z = z / right;
		temp.w = w / right;

		return temp;
	}
	template <typename REAL>
	void vect4d_template<REAL>::operator/= (REAL const& right)
	{
		*this = *this / right;
	}
	template <typename REAL>
	REAL vect4d_template<REAL>::dot (vect4d_template<REAL> const& right) const
	{
	 return x * right.x + y * right.y + z * right.z + w * right.w;
	}

	template <typename REAL>
	vect4d_template<REAL> vect4d_template<REAL>::cross (vect4d_template<REAL> const& right) const
	{
		//return vect4d_template<REAL> ((y * right.z - z * right.y), (z * right.x - x * right.z), (x * right.y - y * right.x));
		return vect4d_template<REAL>(0,0,0,0);
	}

	template <typename REAL>
	REAL vect4d_template<REAL>::magnitude (void) const
	{
		return (REAL) sqrt (x*x + y*y + z*z);
	}
	template <typename REAL>
	REAL vect4d_template<REAL>::sqrMagnitude (void) const
	{
		return (x*x + y*y + z*z);
	}

	template <typename REAL>
	vect4d_template<REAL> vect4d_template<REAL>::normal ( void ) const
	{
		REAL invMagnitude = magnitude();

		if ( fabs(invMagnitude) <= 1.0e-7 ) {
			return vect4d_template<REAL>(0,0,0,0);
		}
		invMagnitude = (REAL)(1.0 / invMagnitude);
		return (*this)*invMagnitude;
	}
	template <typename REAL>
	void vect4d_template<REAL>::normalize ( void )
	{
		(*this) = normal();
	}
	// Integer specialization
	template <>
	vect4d_template<int32_t> vect4d_template<int32_t>::normal ( void ) const
	{
		return vect4d_template<int32_t>( 0,0,0,0 );
	}
	template <>
	vect4d_template<uint8_t> vect4d_template<uint8_t>::normal ( void ) const
	{
		return vect4d_template<uint8_t>( 0,0,0,0 );
	}

	template <typename REAL>
	vect4d_template<REAL> vect4d_template<REAL>::operator- ( void ) const
	{
		return vect4d_template<REAL>( -x,-y,-z,-w );
	}

	template <typename REAL>
	REAL& vect4d_template<REAL>::operator[] ( const int a )
	{
		return raw[a];
	}
	template <typename REAL>
	const REAL& vect4d_template<REAL>::operator[] ( const int a ) const
	{
		return raw[a];
	}

	template <typename REAL>
	vect4d_template<REAL> vect4d_template<REAL>::lerp (vect4d_template<REAL> const& right, REAL const& t) const
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
	bool vect4d_template<REAL>::operator== (vect4d_template<REAL> const& right) const
	{
		if (fabs(right.x - x) > FTYPE_PRECISION)
			return false;
		if (fabs(right.y - y) > FTYPE_PRECISION)
			return false;
		if (fabs(right.z - z) > FTYPE_PRECISION)
			return false;
		if (fabs(right.w - w) > FTYPE_PRECISION)
			return false;
		return true;
	}
	template <>
	bool vect4d_template<int32_t>::operator== ( vect4d_template<int32_t> const& right ) const
	{
		return right.x == x && right.y == y && right.z == z && right.w == w;
	}
	template <>
	bool vect4d_template<uint8_t>::operator== ( vect4d_template<uint8_t> const& right ) const
	{
		return right.x == x && right.y == y && right.z == z && right.w == w;
	}
	//Not equal comparison overload
	template <typename REAL>
	bool vect4d_template<REAL>::operator!= (vect4d_template<REAL> const& right) const
	{
		return !((*this) == right);
	}
	//Component-based comparison for lookups
	template <typename REAL>
	bool vect4d_template<REAL>::operator<  (vect4d_template<REAL> const& right) const
	{
		if ( x > right.x ) return false;
		else if ( x == right.x ) {
			if ( y > right.y ) return false;
			else if ( y == right.y ) {
				if ( z >= right.z ) return false;
				else if ( z == right.z ) {
					if ( w >= right.w ) return false;
				}
			}
		}
		return false;
	}

	// prototypes
	template class vect4d_template<float>;
	template class vect4d_template<double>;
	template class vect4d_template<int32_t>;
	template class vect4d_template<uint16_t>;
	template class vect4d_template<uint8_t>;

}