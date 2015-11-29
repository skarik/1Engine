
#include "Vector2d.h"
#include "Vector3d.h"
#include "Vector4d.h"
#include "Quaternion.h"

#include "matrix/CMatrix.h"

//Default constructor
Vector4d::Vector4d(): x(0.0), y(0.0), z(0.0), w(0.0)
{}

//Copy constructor
Vector4d::Vector4d (Vector4d const& old): x(old.x), y(old.y), z(old.z), w(old.w)
{}

Vector4d::Vector4d (Vector3d const& old): x(old.x), y(old.y), z(old.z), w(1)
{}

//Component input constructor
Vector4d::Vector4d (ftype const& new_x, ftype const& new_y, ftype const& new_z, ftype const& new_w): x(new_x), y(new_y), z(new_z), w(new_w)
{
	/*x = new_x;
	y = new_y;
	z = new_z;*/
}
// Array input constructor Added: 8/30/12
/*Vector3d::Vector3d (const ftype* arr) : x(arr[0]), y(arr[1]), z(arr[2])
{}*/
Vector4d::Vector4d (const ftype* arr) 
{
	x = arr[0];
	y = arr[1];
	z = arr[2];
	w = arr[3];
}
Vector4d::Vector4d (const Vector2d& part1, const Vector2d& part2)
{
	x = part1.x;
	y = part1.y;
	z = part2.x;
	w = part2.y;
}


//Addition overload
Vector4d Vector4d::operator+ (Vector4d const& right) const
{
	Vector4d temp;

	temp.x = x + right.x;
	temp.y = y + right.y;
	temp.z = z + right.z;
	temp.w = w + right.w;

	return temp;
}

//Addition shorthand overload
void Vector4d::operator+= (Vector4d const& right)
{
	*this = *this + right; 
}

//Subtraction overload
Vector4d Vector4d::operator- (Vector4d const& right) const
{
	Vector4d temp;

	temp.x = x - right.x;
	temp.y = y - right.y;
	temp.z = z - right.z;
	temp.w = w - right.w;

	return temp;
}

//Subtraction shorthand overload
void Vector4d::operator-= (Vector4d const& right)
{
	*this = *this - right;
}

//Scalar multiplication overload
Vector4d Vector4d::operator* (ftype const& right) const
{
	Vector4d temp;

	temp.x = x * right;
	temp.y = y * right;
	temp.z = z * right;
	temp.w = w * right;

	return temp;
}

//Scalar multiplication shorthand overload
void Vector4d::operator*= (ftype const& right)
{
	*this = *this * right;
}

//Scalar division overload		Added: 5/3/2012
Vector4d Vector4d::operator/ (ftype const& right) const
{
	Vector4d temp;

	temp.x = x / right;
	temp.y = y / right;
	temp.z = z / right;
	temp.w = w / right;

	return temp;
}
//Scalar division shorthand overload		Added: 5/3/2012
void Vector4d::operator/= (ftype const& right)
{
	*this = *this / right;
}

//Dot product    Added: 1/4/12
ftype Vector4d::dot (Vector4d const& right) const
{
 return x * right.x + y * right.y + z * right.z + w * right.w;
}

//Cross product    Added: 1/4/12
Vector4d Vector4d::cross (Vector4d const& right) const
{
 return Vector4d ((y * right.z - z * right.y), (z * right.x - x * right.z), (x * right.y - y * right.x));
}

//Out stream overloard
std::ostream& operator<< (std::ostream& out, Vector4d const& current)
{
	out << '(' << current.x << ", " << current.y << ", " << current.z << ", " << current.w << ')';

	return out;
}

ftype Vector4d::magnitude (void) const
{
	return sqrt (x*x + y*y + z*z + w*w);
}
ftype Vector4d::sqrMagnitude (void) const
{
	return (x*x + y*y + z*z + w*w);
}


Vector4d Vector4d::normal ( void ) const
{
	return (*this)*(1/magnitude());
}
void Vector4d::normalize ( void )
{
	//(*this)/(magnitude());
	(*this) = normal();
}

Vector4d Vector4d::operator- ( void ) const
{
	return Vector4d( -x,-y,-z,-w );
}

//Index Accessor Added: 5/3/12
ftype& Vector4d::operator[] ( const int a )
{
	/*if ( a == 0 )
		return x;
	else if ( a == 1 )
		return y;
	else if ( a == 2 )
		return z;
	else if ( a == 3 )
		return w;
	return x;*/
	return *(&x + a);
}
const ftype& Vector4d::operator[] ( const int a ) const
{
	return *(&x + a);
}


// matrix multiply Added: 7/9/12
//Vector4d Vector4d::operator* ( Matrix4x4 const& right ) const
Vector4d Vector4d::rvrMultMatx ( Matrix4x4 const& right ) const
{
	return Vector4d(
		right.pData[0]*x+right.pData[4]*y+right.pData[8]*z+right.pData[12]*w,
		right.pData[1]*x+right.pData[5]*y+right.pData[9]*z+right.pData[13]*w,
		right.pData[2]*x+right.pData[6]*y+right.pData[10]*z+right.pData[14]*w,
		right.pData[3]*x+right.pData[7]*y+right.pData[11]*z+right.pData[15]*w
		);
}


//Linear interpolation Added: 9/8/12 at 9:12
Vector4d Vector4d::lerp (Vector4d const& right, ftype const& t)
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
bool Vector4d::operator== (Vector4d const& right) const
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

//Not equal comparison overload
bool Vector4d::operator!= (Vector4d const& right) const
{
	return !((*this) == right);
}

// Component multiplication
Vector4d Vector4d::mulComponents ( Vector4d const& right ) const
{
	return Vector4d( x*right.x, y*right.y, z*right.z, w*right.w );
}
Vector4d Vector4d::divComponents ( Vector4d const& right ) const
{
	return Vector4d( x/right.x, y/right.y, z/right.z, w/right.w );
}
