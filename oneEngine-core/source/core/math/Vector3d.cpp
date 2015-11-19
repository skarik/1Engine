#include "Vector2d.h"
#include "Vector3d.h"
#include "Vector4d.h"
#include "Quaternion.h"

#include "matrix/CMatrix.h"

// Preset public static values
//Vector3d Vector3d::zero = Vector3d( 0,0,0 );
const Vector3d Vector3d::zero		= Vector3d( 0,0,0 );
const Vector3d Vector3d::forward	= Vector3d( 1,0,0 );
const Vector3d Vector3d::up			= Vector3d( 0,0,1 );
const Vector3d Vector3d::left		= Vector3d( 0,1,0 );

//Default constructor
Vector3d::Vector3d(): x(0.0), y(0.0), z(0.0)
{}

//Copy constructor
Vector3d::Vector3d (Vector2d const& old, ftype const new_z): x(old.x), y(old.y), z(new_z)
{}
Vector3d::Vector3d (Vector3d const& old): x(old.x), y(old.y), z(old.z)
{}
Vector3d::Vector3d (Vector4d const& old): x(old.x), y(old.y), z(old.z)
{}

//Component input constructor
Vector3d::Vector3d (ftype const& new_x, ftype const& new_y, ftype const& new_z): x(new_x), y(new_y), z(new_z)
{
	/*x = new_x;
	y = new_y;
	z = new_z;*/
}
// Array input constructor Added: 8/30/12
/*Vector3d::Vector3d (const ftype* arr) : x(arr[0]), y(arr[1]), z(arr[2])
{}*/
Vector3d::Vector3d (const ftype* arr) 
{
	x = arr[0];
	y = arr[1];
	z = arr[2];
}

//Addition overload
Vector3d Vector3d::operator+ (Vector3d const& right) const
{
	Vector3d temp;

	temp.x = x + right.x;
	temp.y = y + right.y;
	temp.z = z + right.z;

	return temp;
}

//Addition shorthand overload
void Vector3d::operator+= (Vector3d const& right)
{
	*this = *this + right; 
}

//Subtraction overload
Vector3d Vector3d::operator- (Vector3d const& right) const
{
	Vector3d temp;

	temp.x = x - right.x;
	temp.y = y - right.y;
	temp.z = z - right.z;

	return temp;
}

//Subtraction shorthand overload
void Vector3d::operator-= (Vector3d const& right)
{
	*this = *this - right;
}

//Scalar multiplication overload
Vector3d Vector3d::operator* (ftype const& right) const
{
	Vector3d temp;

	temp.x = x * right;
	temp.y = y * right;
	temp.z = z * right;

	return temp;
}

//Scalar multiplication shorthand overload
void Vector3d::operator*= (ftype const& right)
{
	*this = *this * right;
}

//Scalar division overload		Added: 5/3/2012
Vector3d Vector3d::operator/ (ftype const& right) const
{
	Vector3d temp;

	temp.x = x / right;
	temp.y = y / right;
	temp.z = z / right;

	return temp;
}
//Scalar division shorthand overload		Added: 5/3/2012
void Vector3d::operator/= (ftype const& right)
{
	*this = *this / right;
}

//Dot product    Added: 1/4/12
ftype Vector3d::dot (Vector3d const& right) const
{
 return x * right.x + y * right.y + z * right.z;
}

//Cross product    Added: 1/4/12
Vector3d Vector3d::cross (Vector3d const& right) const
{
 return Vector3d ((y * right.z - z * right.y), (z * right.x - x * right.z), (x * right.y - y * right.x));
}

//Out stream overloard
std::ostream& operator<< (std::ostream& out, Vector3d const& current)
{
	out << '(' << current.x << ", " << current.y << ", " << current.z << ')';

	return out;
}

ftype Vector3d::magnitude (void) const
{
	return sqrt (x*x + y*y + z*z);
}
ftype Vector3d::sqrMagnitude (void) const
{
	return (x*x + y*y + z*z);
}

Vector3d Vector3d::normal ( void ) const
{
	ftype invMagnitude = magnitude();
/*#ifdef _ENGINE_DEBUG
	if ( fabs(invMagnitude) <= 1.0e-7 ) {
		fnl_assert(0);
	}
#endif*/
	if ( fabs(invMagnitude) <= 1.0e-7 ) {
		return Vector3d::zero;
	}
	invMagnitude = 1.0f / invMagnitude;
	return (*this)*invMagnitude;
}
void Vector3d::normalize ( void )
{
	//(*this)/(magnitude());
	(*this) = normal();
}

Vector3d Vector3d::operator- ( void ) const
{
	return Vector3d( -x,-y,-z );
}

//Index Accessor Added: 5/3/12
ftype& Vector3d::operator[] ( const int a )
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
const ftype& Vector3d::operator[] ( const int a ) const
{
	return *(&x + a);
}

//returns a euler angle vector from a direction vector Added: 7/6/12
Vector3d Vector3d::toEulerAngles ( void ) const
{
	std::cout << "(" << __LINE__ << ") INCORRECT VECTOR3D FUNC" << std::endl;
	return Vector3d(
		0,
		-(ftype)radtodeg( atan2(z, sqrt((x * x) + (y * y))) ),
		//(ftype)radtodeg( atan( z / sqrt( x*x + y*y ) ) ),
		(ftype)radtodeg( atan2(y, x) )
		);
}

// matrix multiply Added: 7/9/12
//Vector3d Vector3d::operator* ( Matrix4x4 const& right ) const
Vector3d Vector3d::rvrMultMatx ( Matrix4x4 const& right ) const
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
}

//Linear interpolation Added: 9/8/12 at 9:12
Vector3d Vector3d::lerp (Vector3d const& right, ftype const& t) const
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
bool Vector3d::operator== (Vector3d const& right) const
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
bool Vector3d::operator!= (Vector3d const& right) const
{
	return !((*this) == right);
}

// Component multiplication
Vector3d Vector3d::mulComponents ( Vector3d const& right ) const
{
	return Vector3d( x*right.x, y*right.y, z*right.z );
}
