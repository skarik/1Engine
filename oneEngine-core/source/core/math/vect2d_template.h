
#ifndef _ENGINE_VECT2D_TEMPLATE_H_
#define _ENGINE_VECT2D_TEMPLATE_H_

#include "core/types/types.h"
#include "core/types/float.h"
#include <cmath>

class Matrix2x2;

namespace Engine
{
	template <typename REAL>
	class vect2d_template 
	{
	public:
		vect2d_template ();	
		vect2d_template (vect2d_template<REAL> const& old );
		vect2d_template (REAL const& new_x, REAL const& new_y);		//Component input constructor

		vect2d_template<REAL> operator+ (vect2d_template<REAL> const& right) const;		//Addition overload
		void operator+= (vect2d_template<REAL> const& right);				//Addition shorthand overload	
		vect2d_template<REAL> operator- (vect2d_template<REAL> const& right) const;		//Subtraction overload
		void operator-= (vect2d_template<REAL> const& right);				//Subtraction shorthand overload
		vect2d_template<REAL> operator* (REAL const& right) const;			//Scalar multiplication overload
		void operator*= (REAL const& right);					//Scalar multiplication shorthand overload
		vect2d_template<REAL> operator/ (REAL const& right) const;			//Scalar division overload   Added: 6/6/13
		void operator/= (REAL const& right);					//Scalar division shorthand overload   Added: 6/6/13
		REAL dot (vect2d_template<REAL> const& right) const;				//Dot product
		REAL cross (vect2d_template<REAL> const& right) const;				// Cross product 10/17/13

		REAL magnitude (void) const;							//Returns the magnitude of the vector
		REAL sqrMagnitude (void) const;						//Returns squared magnitude for fast comparisons 6/6/13

		vect2d_template<REAL> normal ( void ) const;							//Normal of vector Added: 6/6/13
		void normalize ( void );								//Normalize vector Added: 6/6/13

		// Note this doesn't edit the values
		vect2d_template<REAL> lerp (vect2d_template<REAL> const& right, REAL const& t) const;	//Linear interpolation Added: 7/7/13 at 1:12 pm

		REAL& operator[] ( const int );						//Index Accessor Added: 6/4/13
		const REAL& operator[] ( const int ) const;			// 6/4/13

		//vect2d_template<REAL> operator* ( Matrix2x2 const& right ) const;	//Matrix multiplication added: 6/4/13		
		//Vector2d rvrMultMatx ( Matrix2x2 const& right ) const;

		bool operator== (vect2d_template<REAL> const& right) const;			//Equal comparison overload
		bool operator!= (vect2d_template<REAL> const& right) const;			//Not equal comparison overload

		REAL x;
		REAL y;

	};
};

typedef Engine::vect2d_template<double> Vector2d_d;
typedef Engine::vect2d_template<int32_t> Vector2i;

#endif//_ENGINE_VECT2D_TEMPLATE_H_