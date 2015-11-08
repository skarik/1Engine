#ifndef _COLOR_H_
#define _COLOR_H_

#include "core/types/types.h"
#include "core/types/float.h"

class Color
{
public:
	// Construct color, opaque black
	FORCE_INLINE Color ( void );
	// Construct color
	FORCE_INLINE Color ( double r, double g, double b, double a=1.0 );


	// Linear interpolation
	FORCE_INLINE static Color Lerp(Color const c_one, Color const c_two, Real t);
	// Linear interpolation
	FORCE_INLINE Color Lerp(Color const c_two, Real t) const;
	// Addition overload
	FORCE_INLINE Color operator+ (Color const& right) const;		
	// Subtraction overload
	FORCE_INLINE Color operator- (Color const& right) const;	
	// Multiplication overload
	FORCE_INLINE Color operator* (Color const& right) const;	
	// Multiplication overload (scalar)
	FORCE_INLINE Color operator* (Real const& right) const;				
	// Division overload
	FORCE_INLINE Color operator/ (Color const& right) const;
	// Division overload (scalar)
	FORCE_INLINE Color operator/ (Real const& right) const;
	// Equal comparison overload
	FORCE_INLINE bool operator== (Color const& right) const;
	//Not equal comparison overload
	FORCE_INLINE bool operator!= (Color const& right) const;				

	//Return the address of the color red as a pointer
	FORCE_INLINE const Real* start_point (void) const;
	//Return the address of the color red as a pointer
	FORCE_INLINE Real* start_point (void);

	//===============================================================================================//
	// HSL + INT Conversion
	//===============================================================================================//

	// Returns a Color containing HSL values
	void GetHSLC ( Color& outHSL ) const;
	// Sets the Color to a value with the given HSL values (RGB as HSL)
	void SetHSL ( const Color& inHSL );

	// Returns an integer representing the color
	FORCE_INLINE uint32_t GetCode ( void ) const;
	// Sets the Color to a value represented by the given integer.
	FORCE_INLINE void SetCode ( const uint32_t code );

	//===============================================================================================//
	// Extensions
	//===============================================================================================//

	// Color randomizer (mostly useless imo, but is hilarious)
	// Also has the bonus of being aligned to a 32-bit color
	void randomize (void);

public:
	Real red;
	Real green;
	Real blue;
	Real alpha;
};


//===============================================================================================//
// Color implementation
//===============================================================================================//
#include <cmath>

// Construct color, opaque black
FORCE_INLINE Color::Color()
	: red(0), green(0), blue(0), alpha(1)
{
	;
}

// Construct color
FORCE_INLINE Color::Color ( double r, double g, double b, double a )
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
FORCE_INLINE Color Color::operator* (Real const& right) const 
{
	return Color (red * right, green * right, blue * right, alpha * right);
}

//Division operator overload
FORCE_INLINE Color Color::operator/ (Color const& right) const
{
	return Color (red / right.red, green / right.green, blue / right.blue, alpha / right.alpha);
}
//Division operator overload (scalar)
FORCE_INLINE Color Color::operator/ (Real const& right) const
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


//Return the address of the red component to use as an array
FORCE_INLINE const Real* Color::start_point (void) const
{
	return &red;
}
FORCE_INLINE Real* Color::start_point (void)
{
	return &red;
}

//===============================================================================================//
// HSL + INT Conversion
//===============================================================================================//

// Returns an integer representing the color
FORCE_INLINE uint32_t Color::GetCode ( void ) const
{
	uint32_t result;
	result =	(uint32_t(red*255)		& 0xFF) << 24;
	result |=	(uint32_t(green*255)	& 0xFF) << 16;
	result |=	(uint32_t(blue*255)		& 0xFF) << 8;
	result |=	(uint32_t(alpha*255) & 0xFF);
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

#endif//_COLOR_H_