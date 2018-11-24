//Implementation Copyright 2012 by John Choi
// Modified and improved 2013 by Joshua Boren

#include "Vector2f.h"
#include "Vector3d.h"
#include "Vector4d.h"
#include "Quaternion.h"

#include "matrix/CMatrix.h"

//Truncating constructor
Vector2f::Vector2f (Vector3d const& old)
	: x(old.x), y(old.y)
{
	;
}

//Out stream overloard
std::ostream& operator<< (std::ostream& out, Vector2f const& current)
{
	out << '(' << current.x << ", " << current.y << ')';

	return out;
}

//Matrix multiplication
Vector2f Vector2f::rvrMultMatx ( Matrix2x2 const& right ) const
{
	return Vector2f(
		right.pData[0]*x+right.pData[1]*y,
		right.pData[2]*x+right.pData[3]*y
		);
}

// Component multiplication
Vector2f Vector2f::mulComponents ( Vector2f const& right ) const
{
	return Vector2f( x*right.x, y*right.y );
}
// Component multiplication
Vector2f Vector2f::divComponents ( Vector2f const& right ) const
{
	return Vector2f( x/right.x, y/right.y );
}