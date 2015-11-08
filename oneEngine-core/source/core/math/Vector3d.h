#ifndef VECTOR3D_H
#define VECTOR3D_H

#include "core/types/float.h"
#include <iostream>
#include <cmath>

//using namespace std;

// Declare class usage
class Matrix3x3;
class Matrix4x4;

class Vector2d;
class Vector4d;

class Vector3d 
{
	friend std::ostream& operator<< (std::ostream& out, Vector3d const& current);		//Out stream overload
	
public:
	Vector3d ();												//Default constructor
	Vector3d (Vector2d const& old, Real const new_z=0);
	Vector3d (Vector3d const& old);								//Copy constructor
	Vector3d (Vector4d const& old);								//Copy constructor
	Vector3d (Real const& new_x, Real const& new_y, Real const& new_z);		//Component input constructor
	Vector3d (const Real* arr);							// Array input constructor Added: 8/30/12

	Vector3d operator+ (Vector3d const& right) const;		//Addition overload
	void operator+= (Vector3d const& right);				//Addition shorthand overload	
	Vector3d operator- (Vector3d const& right) const;		//Subtraction overload
	void operator-= (Vector3d const& right);				//Subtraction shorthand overload
	Vector3d operator* (Real const& right) const;			//Scalar multiplication overload
	void operator*= (Real const& right);					//Scalar multiplication shorthand overload
	Vector3d operator/ (Real const& right) const;			//Scalar division overload   Added: 5/3/12
	void operator/= (Real const& right);					//Scalar division shorthand overload   Added: 5/3/12
	Real dot (Vector3d const& right) const;				//Dot product  Added: 1/4/12
	Vector3d cross (Vector3d const& right) const;			//Cross product  Added: 1/4/12
	Vector3d normal ( void ) const;							//Normal of vector Added: 1/12/12
	
	void normalize ( void );								//Normalize vector Added: 12/29/12

	// Note this doesn't edit the values
	Vector3d lerp (Vector3d const& right, Real const& t) const;	//Linear interpolation Added: 9/8/12 at 9:12 am

	Vector3d operator- () const;							//Negation Added: 2/24/12

	Real& operator[] ( const int );						//Index Accessor Added: 5/3/12
	const Real& operator[] ( const int ) const;			// 6/4/13

	Real magnitude (void) const;							//Returns the magnitude of the vector
	Real sqrMagnitude (void) const;						//Returns the squared magnitude of the vector

	Vector3d toEulerAngles ( void ) const;					//returns a euler angle vector from a direction vector Added: 7/6/12

	//Vector3d operator* ( Matrix4x4 const& right ) const;	//Matrix multiplication added: 7/9/12
	//Vector3d operator* ( Matrix3x3 const& right ) const;		//Rotator multiplication added: 1/5/13
	Vector3d rvrMultMatx ( Matrix4x4 const& right ) const;
	Vector3d rvrMultMatx ( Matrix3x3 const& right ) const;

	bool operator== (Vector3d const& right) const;			//Equal comparison overload
	bool operator!= (Vector3d const& right) const;			//Not equal comparison overload

public: // CHANGED TO PUBLIC FROM PRIVATE SOZ
	Real x;
	Real y;
	Real z;

	//static Vector3d zero;

public:
	static const Vector3d zero;
	static const Vector3d forward;
	static const Vector3d up;
	static const Vector3d left;

};


#endif