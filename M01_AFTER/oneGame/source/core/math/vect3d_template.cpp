
#include "vect3d_template.h"

using namespace Engine;

//Default constructor
template <typename REAL>
vect3d_template<REAL>::vect3d_template(): x(0.0), y(0.0), z(0.0)
{}

//Copy constructor
template <typename REAL>
vect3d_template<REAL>::vect3d_template (vect3d_template<REAL> const& old): x(old.x), y(old.y), z(old.z)
{}


//Component input constructor
template <typename REAL>
vect3d_template<REAL>::vect3d_template (REAL const& new_x, REAL const& new_y, REAL const& new_z): x(new_x), y(new_y), z(new_z)
{
	/*x = new_x;
	y = new_y;
	z = new_z;*/
}
// Array input constructor Added: 8/30/12
/*Vector3d::Vector3d (const ftype* arr) : x(arr[0]), y(arr[1]), z(arr[2])
{}*/
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

//Out stream overloard
/*template <typename REAL>
ostream& operator<< (ostream& out, vect3d_template<REAL> const& current)
{
	out << '(' << current.x << ", " << current.y << ", " << current.z << ')';

	return out;
}*/

template <typename REAL>
REAL vect3d_template<REAL>::magnitude (void) const
{
	return sqrt (x*x + y*y + z*z);
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
/*#ifdef _ENGINE_DEBUG
	if ( fabs(invMagnitude) <= 1.0e-7 ) {
		fnl_assert(0);
	}
#endif*/
	if ( fabs(invMagnitude) <= 1.0e-7 ) {
		return vect3d_template<REAL>(0,0,0);
	}
	invMagnitude = 1.0f / invMagnitude;
	return (*this)*invMagnitude;
}
template <typename REAL>
void vect3d_template<REAL>::normalize ( void )
{
	//(*this)/(magnitude());
	(*this) = normal();
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
	/*if ( a == 0 )
		return x;
	else if ( a == 1 )
		return y;
	else if ( a == 2 )
		return z;
	return x;*/
	return *(&x + a);
}
//Index Accessor Added: 5/3/12
template <typename REAL>
const REAL& vect3d_template<REAL>::operator[] ( const int a ) const
{
	return *(&x + a);
}

//returns a euler angle vector from a direction vector Added: 7/6/12
/*template <typename REAL>
vect3d_template<REAL> vect3d_template<REAL>::toEulerAngles ( void ) const
{
	cout << "(" << __LINE__ << ") INCORRECT VECTOR3D FUNC" << endl;
	return vect3d_template<REAL>(
		0,
		-(ftype)radtodeg( atan2(z, sqrt((x * x) + (y * y))) ),
		//(ftype)radtodeg( atan( z / sqrt( x*x + y*y ) ) ),
		(ftype)radtodeg( atan2(y, x) )
		);
}*/

// matrix multiply Added: 7/9/12
//Vector3d Vector3d::operator* ( Matrix4x4 const& right ) const
/*vect3d_template<REAL> vect3d_template<REAL>::rvrMultMatx ( Matrix4x4 const& right ) const
{
	return Vector3d(
		right.pData[0]*x+right.pData[4]*y+right.pData[8]*z+right.pData[3],
		right.pData[1]*x+right.pData[5]*y+right.pData[9]*z+right.pData[7],
		right.pData[2]*x+right.pData[6]*y+right.pData[10]*z+right.pData[11]
		);
}
// matrix multiply Added: 1/5/13
//Vector3d Vector3d::operator* ( Matrix3x3 const& right ) const
Vector3d Vector3d::rvrMultMatx ( Matrix3x3 const& right ) const
{
	return Vector3d(
		right.pData[0]*x+right.pData[3]*y+right.pData[6]*z,
		right.pData[1]*x+right.pData[4]*y+right.pData[7]*z,
		right.pData[2]*x+right.pData[5]*y+right.pData[8]*z
		);
}*/

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

//Not equal comparison overload
template <typename REAL>
bool vect3d_template<REAL>::operator!= (vect3d_template<REAL> const& right) const
{
	return !((*this) == right);
}

// prototypes
template class vect3d_template<float>;
template class vect3d_template<double>;
