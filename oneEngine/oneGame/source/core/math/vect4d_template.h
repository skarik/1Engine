#ifndef ENGINE_VECTOR_4D_TEMPLATE_H_
#define ENGINE_VECTOR_4D_TEMPLATE_H_

#include "core/types/types.h"
#include "core/types/float.h"
#include <cmath>

namespace core
{
	template <typename REAL>
	class vect2d_template;
	template <typename REAL>
	class vect3d_template;

	template <typename REAL>
	class vect4d_template 
	{
	public:
		vect4d_template (void);
		vect4d_template (vect2d_template<REAL> const& old, const REAL& new_z = 0, const REAL& new_w = 0 );
		vect4d_template (vect3d_template<REAL> const& old, const REAL& new_w = 0 );
		vect4d_template (vect4d_template<REAL> const& old );
		vect4d_template (REAL const& new_x, REAL const& new_y, REAL const& new_z, REAL const& new_w);
		vect4d_template (const REAL* arr);

		vect4d_template<REAL> operator+ (vect4d_template<REAL> const& right) const;
		void operator+= (vect4d_template<REAL> const& right);
		vect4d_template<REAL> operator- (vect4d_template<REAL> const& right) const;
		void operator-= (vect4d_template<REAL> const& right);
		vect4d_template<REAL> operator* (REAL const& right) const;
		void operator*= (REAL const& right);
		vect4d_template<REAL> operator/ (REAL const& right) const;
		void operator/= (REAL const& right);
		REAL dot (vect4d_template<REAL> const& right) const;
		vect4d_template<REAL> cross (vect4d_template<REAL> const& right) const;
		vect4d_template<REAL> normal ( void ) const;
	
		void normalize ( void );

		// Note this doesn't edit the values
		vect4d_template<REAL> lerp (vect4d_template<REAL> const& right, REAL const& t) const;

		vect4d_template<REAL> operator- () const;

		REAL& operator[] ( const int );
		const REAL& operator[] ( const int ) const;

		REAL magnitude (void) const;
		REAL sqrMagnitude (void) const;

		bool operator== (vect4d_template<REAL> const& right) const;
		bool operator!= (vect4d_template<REAL> const& right) const;
		bool operator<  (vect4d_template<REAL> const& right) const;

	public:
		union
		{
			struct
			{
				REAL r;
				REAL g;
				REAL b;
				REAL a;
			};
			struct
			{
				REAL x;
				REAL y;
				REAL z;
				REAL w;
			};
			struct
			{
				REAL raw [4];
			};
		};
	};

	typedef vect4d_template<double> Vector4f_d;
	typedef vect4d_template<int32_t> Vector4i32;
	typedef vect4d_template<uint8_t> Vector4u8;
	typedef vect4d_template<uint16_t> Vector4u16;
};

typedef core::Vector4f_d	Vector4f_d;
typedef core::Vector4i32	Vector4i;
typedef core::Vector4u8		Vector4u8;
typedef core::Vector4u16	Vector4u16;

#endif//ENGINE_VECTOR_4D_TEMPLATE_H_