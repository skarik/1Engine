//===============================================================================================//
//
//		Color.h
//
// Contains the following classes:
//		Color, floating point RGBA color.
//		ColorRGBA8, byte RGBA color.
//		ColorRGBA16, short RGBA color.
//
//===============================================================================================//
#ifndef CORE_MATH_COLOR_H_
#define CORE_MATH_COLOR_H_

#include <algorithm>
#include "core/types/types.h"
#include "core/types/float.h"
#include "core/math/Vector4.h"

class Color;
class ColorRGBA8;
class ColorRGBA16;

// Provides a mapping of values for converting between types.
// Defaults to mapping [0.0, 1.0] with [0, 256).
struct ColorRangeMapping
{
	float			min_float = 0.0F;
	float			max_float = 1.0F;
	int32_t			min_int = 0;
	int32_t			max_int = 255;
};

class Color
{
public:
	// Construct color, opaque black
	FORCE_INLINE Color ( void );
	// Copy constructor
	FORCE_INLINE Color ( const Color&c );
	// Construct color
	FORCE_INLINE Color ( Real32 r, Real32 g, Real32 b, Real32 a=1.0F );
	// Construct color
	FORCE_INLINE Color ( Real64 r, Real64 g, Real64 b, Real64 a=1.0F );
	// Construct color from templated RGB inputs:
	template <typename T0, typename T1, typename T2>
	FORCE_INLINE Color (T0 r, T1 g, T2 b);
	// Construct color from templated RGBA inputs:
	template <typename T0, typename T1, typename T2, typename T3>
	FORCE_INLINE Color (T0 r, T1 g, T2 b, T3 a);

	// Implicit cast to Vector4.
	operator Vector4f() const;

	// Linear interpolation
	FORCE_INLINE static Color Lerp (Color const c_one, Color const c_two, Real t);
	// Linear interpolation
	FORCE_INLINE Color Lerp (Color const c_two, Real t) const;

	// Vector arithmetic
	FORCE_INLINE Color operator+ (Color const& right) const;		
	FORCE_INLINE Color operator- (Color const& right) const;	
	FORCE_INLINE Color operator* (Color const& right) const;	
	FORCE_INLINE Color operator/ (Color const& right) const;

	FORCE_INLINE void operator+= (Color const& right)
	{
		*this = *this + right;
	}
	FORCE_INLINE void operator-= (Color const& right)
	{
		*this = *this - right;
	}

	// Scalar arithmetic
	FORCE_INLINE Color operator* (Real32 const& right) const;
	FORCE_INLINE Color operator* (Real64 const& right) const;
	FORCE_INLINE Color operator/ (Real32 const& right) const;
	FORCE_INLINE Color operator/ (Real64 const& right) const;

	template <typename REAL>
	FORCE_INLINE void operator*= (REAL const& right)
	{
		*this = *this * right;
	}
	template <typename REAL>
	FORCE_INLINE void operator/= (REAL const& right)
	{
		*this = *this * right;
	}
	
	// Comparison
	FORCE_INLINE bool operator== (Color const& right) const;
	FORCE_INLINE bool operator!= (Color const& right) const;

	// Access via array
	FORCE_INLINE Real& operator[] ( const int );
	// Const access via array
	FORCE_INLINE const Real& operator[] ( const int ) const;

	// luminosity() : Return luminosity of the color value
	FORCE_INLINE Real luminosity (void) const
	{
		return r * 0.299F + g * 0.587F + b * 0.114F;
	}
	// luminosityRGBA() : Return luminosity of the color value, as a grayscale color
	FORCE_INLINE Color luminosityRGBA (void) const
	{
		Real lumin = luminosity();
		return Color(lumin, lumin, lumin, alpha);
	}

	//===============================================================================================//
	// HSL + INT Conversion
	//===============================================================================================//

	// Returns a Color containing HSL values
	/*CORE_API*/ void		GetHSLC ( Color& outHSL ) const;
	// Sets the Color to a value with the given HSL values (RGB as HSL)
	/*CORE_API*/ void		SetHSL ( const Color& inHSL );

	// Returns an integer representing the color
	FORCE_INLINE uint32_t	GetCode ( void ) const;
	// Sets the Color to a value represented by the given integer.
	FORCE_INLINE void		SetCode ( const uint32_t code );

	/*CORE_API*/ ColorRGBA8	ToRGBA8 ( const ColorRangeMapping& mapping = ColorRangeMapping() ) const;
	/*CORE_API*/ ColorRGBA16
							ToRGBA16 ( const ColorRangeMapping& mapping = ColorRangeMapping() ) const;

	//===============================================================================================//
	// Extensions
	//===============================================================================================//

	// Color randomizer (mostly useless imo, but is hilarious)
	// Also has the bonus of being aligned to a 32-bit color
	void randomize (void);

public:
	union
	{
		struct
		{
			Real red;
			Real green;
			Real blue;
			Real alpha;
		};
		struct
		{
			Real r;
			Real g;
			Real b;
			Real a;
		};
		struct
		{
			Real x;
			Real y;
			Real z;
			Real w;
		};
		struct
		{
			Real raw [4];
		};
	};
};

class ColorRGBA8
{
public:
	FORCE_INLINE ColorRGBA8 ( uint8_t r, uint8_t g, uint8_t b, uint8_t a = UINT8_MAX )
		: r(r), g(g), b(b), a(a)
		{}

	/*CORE_API*/ Color		ToRGBAFloat ( const ColorRangeMapping& mapping = ColorRangeMapping() ) const;

public:
	union
	{
		struct
		{
			uint8_t r;
			uint8_t g;
			uint8_t b;
			uint8_t a;
		};
		struct
		{
			uint8_t x;
			uint8_t y;
			uint8_t z;
			uint8_t w;
		};
		struct
		{
			uint8_t raw [4];
		};
	};
};

class ColorRGBA16
{
public:
	FORCE_INLINE ColorRGBA16 ( int16_t r, int16_t g, int16_t b, int16_t a = INT16_MAX )
		: r(r), g(g), b(b), a(a)
		{}

	/*CORE_API*/ Color		ToRGBAFloat ( const ColorRangeMapping& mapping = ColorRangeMapping() ) const;

public:
	union
	{
		struct
		{
			int16_t r;
			int16_t g;
			int16_t b;
			int16_t a;
		};
		struct
		{
			int16_t x;
			int16_t y;
			int16_t z;
			int16_t w;
		};
		struct
		{
			int16_t raw [4];
		};
	};
};

//===============================================================================================//
// Color implementation
//===============================================================================================//
#include <cmath>

// Construct color, opaque black
FORCE_INLINE Color::Color()
	: red(0.0F), green(0.0F), blue(0.0F), alpha(1.0F)
{
	;
}
// Copy constructor
FORCE_INLINE Color::Color ( const Color&c )
	: r(c.r), g(c.g), b(c.b), a(c.a)
{
	;
}
// Construct color
FORCE_INLINE Color::Color ( Real32 r, Real32 g, Real32 b, Real32 a )
	: red(Real(r)), green(Real(g)), blue(Real(b)), alpha(Real(a))
{
	;
}
// Construct color
FORCE_INLINE Color::Color ( Real64 r, Real64 g, Real64 b, Real64 a )
	: red(Real(r)), green(Real(g)), blue(Real(b)), alpha(Real(a))
{
	;
}
// Construct color from templated RGB inputs:
template <typename T0, typename T1, typename T2>
FORCE_INLINE Color::Color (T0 r, T1 g, T2 b)
	: red(Real(r)), green(Real(g)), blue(Real(b)), alpha(Real(1.0))
{

}
// Construct color from templated RGBA inputs:
template <typename T0, typename T1, typename T2, typename T3>
FORCE_INLINE Color::Color (T0 r, T1 g, T2 b, T3 a)
	: red(Real(r)), green(Real(g)), blue(Real(b)), alpha(Real(a))
{
	;
}

// Linear interpolation
FORCE_INLINE Color Color::Lerp (Color const c_one, Color const c_two, Real t)
{
	if(t <= 0)
	{
		return c_one;
	}
	else if(t >= 1)
	{
		return c_two;
	}
	else
	{
		return ((c_two - c_one)*t + c_one);
	}
}
// Linear interpolation
FORCE_INLINE Color Color::Lerp (Color const c_two, Real t) const
{
	if(t <= 0)
	{
		return (*this);
	}
	else if(t >= 1)
	{
		return c_two;
	}
	else
	{
		return ((c_two - (*this))*t + (*this));
	}
}

//Addition operator overload
FORCE_INLINE Color Color::operator+ (Color const& right) const
{
	return Color (red + right.red, green + right.green, blue + right.blue, alpha + right.alpha);
}

//Subtraction operator overload
FORCE_INLINE Color Color::operator- (Color const& right) const
{
	return Color (red - right.red, green - right.green, blue - right.blue, alpha - right.alpha);
}

//Multiplication operator overload
FORCE_INLINE Color Color::operator* (Color const& right) const
{
	return Color (red * right.red, green * right.green, blue * right.blue, alpha * right.alpha);
}
//Multiplication operator overload (scalar)
FORCE_INLINE Color Color::operator* (Real32 const& right) const 
{
	return Color (red * right, green * right, blue * right, alpha * right);
}
FORCE_INLINE Color Color::operator* (Real64 const& right) const 
{
	return Color (red * right, green * right, blue * right, alpha * right);
}

//Division operator overload
FORCE_INLINE Color Color::operator/ (Color const& right) const
{
	return Color (red / right.red, green / right.green, blue / right.blue, alpha / right.alpha);
}
//Division operator overload (scalar)
FORCE_INLINE Color Color::operator/ (Real32 const& right) const
{
	return Color (red / right, green / right, blue / right, alpha / right);
}
FORCE_INLINE Color Color::operator/ (Real64 const& right) const
{
	return Color (red / right, green / right, blue / right, alpha / right);
}

//Equal comparison overload
FORCE_INLINE bool Color::operator== (Color const& right) const
{
	if (fabs(right.blue - blue) > FTYPE_PRECISION)
		return false;
	if (fabs(right.green - green) > FTYPE_PRECISION)
		return false;
	if (fabs(right.red - red) > FTYPE_PRECISION)
		return false; 
	if (fabs(right.alpha - alpha) > 2 * FTYPE_PRECISION)
		return false;
	return true;
}
FORCE_INLINE bool Color::operator!= (Color const& right) const
{
	return !((*this) == right);
}

// Access via array
FORCE_INLINE Real& Color::operator[] ( const int a )
{
	return raw[a];
}
// Const access via array
FORCE_INLINE const Real& Color::operator[] ( const int a ) const
{
	return raw[a];
}

//===============================================================================================//
// HSL + INT Conversion
//===============================================================================================//

// Returns an integer representing the color
FORCE_INLINE uint32_t Color::GetCode ( void ) const
{
	uint32_t result;
	result =	(uint32_t(red*255 + 0.5)	& 0xFF) << 24;
	result |=	(uint32_t(green*255 + 0.5)	& 0xFF) << 16;
	result |=	(uint32_t(blue*255 + 0.5)	& 0xFF) << 8;
	result |=	(uint32_t(alpha*255 + 0.5)	& 0xFF);
	return result;
}
// Sets the Color to a value represented by the given integer.
FORCE_INLINE void Color::SetCode ( const uint32_t code )
{
	red =	((code >> 24)	& 0xFF) / Real(255);
	green = ((code >> 16)	& 0xFF) / Real(255);
	blue =	((code >> 8)	& 0xFF) / Real(255);
	alpha =	((code) & 0xFF) / Real(255);
}

#endif//CORE_MATH_COLOR_H_