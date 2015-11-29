//Implementation Copyright 2012 by John Choi
// Modified and improved 2013 by Joshua Boren

#include "Vector2d.h"
#include "Vector3d.h"
#include "Vector4d.h"
#include "Quaternion.h"

#include "matrix/CMatrix.h"

//Truncating constructor
Vector2d::Vector2d (Vector3d const& old)
	: x(old.x), y(old.y)
{
	;
}

//Out stream overloard
std::ostream& operator<< (std::ostream& out, Vector2d const& current)
{
	out << '(' << current.x << ", " << current.y << ')';

	return out;
}

//Matrix multiplication
//Vector2d Vector2d::operator* ( Matrix2x2 const& right ) const
Vector2d Vector2d::rvrMultMatx ( Matrix2x2 const& right ) const
{
	return Vector2d(
		right.pData[0]*x+right.pData[1]*y,
		right.pData[2]*x+right.pData[3]*y
		);
}

// Component multiplication
Vector2d Vector2d::mulComponents ( Vector2d const& right ) const
{
	return Vector2d( x*right.x, y*right.y );
}
// Component multiplication
Vector2d Vector2d::divComponents ( Vector2d const& right ) const
{
	return Vector2d( x/right.x, y/right.y );
}