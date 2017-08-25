#include "Vector2d.h"
#include "Vector3d.h"
#include "Vector4d.h"
#include "Quaternion.h"

#include "matrix/CMatrix.h"

// Preset public static values
const Vector3d Vector3d::zero		= Vector3d( 0,0,0 );
const Vector3d Vector3d::forward	= Vector3d( 1,0,0 );
const Vector3d Vector3d::up			= Vector3d( 0,0,1 );
const Vector3d Vector3d::left		= Vector3d( 0,1,0 );

//Copy constructor
Vector3d::Vector3d (Vector2d const& old, Real const new_z): x(old.x), y(old.y), z(new_z)
{}
Vector3d::Vector3d (Vector4d const& old): x(old.x), y(old.y), z(old.z)
{}

//Out stream overloard
std::ostream& operator<< (std::ostream& out, Vector3d const& current)
{
	out << '(' << current.x << ", " << current.y << ", " << current.z << ')';

	return out;
}

//returns a euler angle vector from a direction vector Added: 7/6/12
Vector3d Vector3d::toEulerAngles ( void ) const
{
	std::cout << "(" << __LINE__ << ") INCORRECT VECTOR3D FUNC" << std::endl;
	return Vector3d(
		0,
		-(Real)radtodeg( atan2(z, sqrt((x * x) + (y * y))) ),
		//(Real)radtodeg( atan( z / sqrt( x*x + y*y ) ) ),
		(Real)radtodeg( atan2(y, x) )
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

// Component multiplication
Vector3d Vector3d::mulComponents ( Vector3d const& right ) const
{
	return Vector3d( x*right.x, y*right.y, z*right.z );
}
Vector3d Vector3d::divComponents ( Vector3d const& right ) const
{
	return Vector3d( x/right.x, y/right.y, z/right.z );
}
