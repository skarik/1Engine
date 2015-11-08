#ifndef _ENGINE_VECT3D_TEMPLATE_H_
#define _ENGINE_VECT3D_TEMPLATE_H_

#include "core/types/float.h"
#include <cmath>

namespace Engine
{
	template <typename REAL>
	class vect3d_template 
	{
	public:
		vect3d_template ();												//Default constructor
		//vect3d_template (Vector2d const& old, REAL const new_z=0);
		//vect3d_template (Vector3d const& old);								//Copy constructor
		//vect3d_template (Vector4d const& old);								//Copy constructor
		vect3d_template (vect3d_template<REAL> const& old );
		vect3d_template (REAL const& new_x, REAL const& new_y, REAL const& new_z);		//Component input constructor
		vect3d_template (const REAL* arr);							// Array input constructor Added: 8/30/12

		vect3d_template<REAL> operator+ (vect3d_template<REAL> const& right) const;		//Addition overload
		void operator+= (vect3d_template<REAL> const& right);				//Addition shorthand overload	
		vect3d_template<REAL> operator- (vect3d_template<REAL> const& right) const;		//Subtraction overload
		void operator-= (vect3d_template<REAL> const& right);				//Subtraction shorthand overload
		vect3d_template<REAL> operator* (REAL const& right) const;			//Scalar multiplication overload
		void operator*= (REAL const& right);					//Scalar multiplication shorthand overload
		vect3d_template<REAL> operator/ (REAL const& right) const;			//Scalar division overload   Added: 5/3/12
		void operator/= (REAL const& right);					//Scalar division shorthand overload   Added: 5/3/12
		REAL dot (vect3d_template<REAL> const& right) const;				//Dot product  Added: 1/4/12
		vect3d_template<REAL> cross (vect3d_template<REAL> const& right) const;			//Cross product  Added: 1/4/12
		vect3d_template<REAL> normal ( void ) const;							//Normal of vector Added: 1/12/12
	
		void normalize ( void );								//Normalize vector Added: 12/29/12

		// Note this doesn't edit the values
		vect3d_template<REAL> lerp (vect3d_template<REAL> const& right, REAL const& t) const;	//Linear interpolation Added: 9/8/12 at 9:12 am

		vect3d_template<REAL> operator- () const;							//Negation Added: 2/24/12

		REAL& operator[] ( const int );						//Index Accessor Added: 5/3/12
		const REAL& operator[] ( const int ) const;			// 6/4/13

		REAL magnitude (void) const;							//Returns the magnitude of the vector
		REAL sqrMagnitude (void) const;						//Returns the squared magnitude of the vector

		//vect3d_template<REAL> toEulerAngles ( void ) const;					//returns a euler angle vector from a direction vector Added: 7/6/12

		//Vector3d operator* ( Matrix4x4 const& right ) const;	//Matrix multiplication added: 7/9/12
		//Vector3d operator* ( Matrix3x3 const& right ) const;		//Rotator multiplication added: 1/5/13
		//vect3d_template<REAL> rvrMultMatx ( Matrix4x4 const& right ) const;
		//vect3d_template<REAL> rvrMultMatx ( Matrix3x3 const& right ) const;

		bool operator== (vect3d_template<REAL> const& right) const;			//Equal comparison overload
		bool operator!= (vect3d_template<REAL> const& right) const;			//Not equal comparison overload

	public: // CHANGED TO PUBLIC FROM PRIVATE SOZ
		REAL x;
		REAL y;
		REAL z;

	};

};

typedef Engine::vect3d_template<double> Vector3d_d;

#endif//_ENGINE_VECT3D_TEMPLATE_H_