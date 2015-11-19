//	class Vector4d
// 4 component vector math class

#ifndef VECTOR4D_H
#define VECTOR4D_H

#include "core/types/float.h"
#include <iostream>
#include <cmath>

// Declare class usage
class Matrix4x4;

class Vector2d;
class Vector3d;

class Vector4d 
{
	friend std::ostream& operator<< (std::ostream& out, Vector4d const& current);		//Out stream overload
	
public:
	Vector4d ();
	Vector4d (Vector4d const& old);	
	Vector4d (ftype const& new_x, ftype const& new_y, ftype const& new_z, ftype const& new_w=0);
	Vector4d (const ftype* arr);
	Vector4d (Vector3d const& old);
	Vector4d (const Vector2d& part1, const Vector2d& part2);

	Vector4d operator+ (Vector4d const& right) const;
	void operator+= (Vector4d const& right);				//Addition shorthand overload	
	Vector4d operator- (Vector4d const& right) const;		//Subtraction overload
	void operator-= (Vector4d const& right);				//Subtraction shorthand overload
	Vector4d operator* (ftype const& right) const;			//Scalar multiplication overload
	void operator*= (ftype const& right);					//Scalar multiplication shorthand overload
	Vector4d operator/ (ftype const& right) const;			//Scalar division overload   Added: 5/3/12
	void operator/= (ftype const& right);					//Scalar division shorthand overload   Added: 5/3/12
	ftype dot (Vector4d const& right) const;				//Dot product  Added: 1/4/12
	Vector4d cross (Vector4d const& right) const;			//Cross product  Added: 1/4/12
	Vector4d normal ( void ) const;							//Normal of vector Added: 1/12/12
	
	void normalize ( void );								//Normalize vector Added: 12/29/12

	// Note this doesn't edit the values
	Vector4d lerp (Vector4d const& right, ftype const& t);	//Linear interpolation Added: 9/8/12 at 9:12 am

	Vector4d operator- () const;							//Negation Added: 2/24/12

	ftype& operator[] ( const int );						//Index Accessor Added: 5/3/12
	const ftype& operator[] ( const int ) const;			// 6/4/13

	ftype magnitude (void) const;							//Returns the magnitude of the vector
	ftype sqrMagnitude (void) const;						//Returns the squared magnitude of the vector

	//Vector4d operator* ( Matrix4x4 const& right ) const;	//Matrix multiplication added: 7/9/12
	Vector4d rvrMultMatx ( Matrix4x4 const& right ) const;
	// Component multiply
	Vector4d mulComponents ( Vector4d const& right ) const;

	bool operator== (Vector4d const& right) const;			//Equal comparison overload
	bool operator!= (Vector4d const& right) const;			//Not equal comparison overload

public:
	ftype x;
	ftype y;
	ftype z;
	ftype w;

};


#endif