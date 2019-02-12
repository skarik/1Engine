//===============================================================================================//
//
//		pixelFormat.h
//
// Contains definitions for individual pixel data storage
//
//===============================================================================================//
#ifndef CORE_GFX_PIXEL_FORMAT_H_
#define CORE_GFX_PIXEL_FORMAT_H_

#include "core/types/types.h"
#include "core/types/float.h"
#include "core/math/Color.h"

namespace core {
namespace gfx {

	// 32 bit pixel struct
	struct arPixel
	{
		// Data set
		uint8_t r;
		uint8_t g;
		uint8_t b;
		uint8_t a;
	
		// Default constructor
		arPixel ( void ) :
			r(0x00), g(0x00), b(0x00), a(0xFF)
		{
			;
		}
		// Construct from channel bytes
		arPixel ( uint8_t _r, uint8_t _g, uint8_t _b, uint8_t _a ) :
			r(_r), g(_g), b(_b), a(_a)
		{
			;
		}
		// Construct from color
		arPixel ( Color& color )
		{
			*((uint32_t*)&r) = color.GetCode();
		}

		// comparison for using in maps
		bool operator== (const arPixel& pixel) const
		{
			return (r == pixel.r && b == pixel.b && g == pixel.g && a == pixel.a);
		}
		bool operator!= (const arPixel& pixel) const
		{
			return (r != pixel.r || b != pixel.b || g != pixel.g || a != pixel.a);
		}
		bool operator< (const arPixel& pixel) const
		{
			if ( r < pixel.r ) {
				return true;
			}
			else if ( r == pixel.r )
			{
				if ( g < pixel.g ) {
					return true;
				}
				else if ( g == pixel.g )
				{
					if ( b < pixel.b ) {
						return true;
					}
					else if ( b == pixel.b ) {
						if ( a < pixel.a ) {
							return true;
						}
					}
				}
			}
			return false;
		}

		// size_t cast : used for indexing RangeVectors into hash tables
		operator size_t () const
		{
			return (r) | (b << 8) | (g << 8) | (a << 8);
		}
	};
	typedef arPixel _B_ixel;

	// Floating point pixel struct
	struct arPixelF
	{
		Real32 r;
		Real32 g;
		Real32 b;
		Real32 a;
	};

}}

// Definition for C++11 hash function
template<> struct std::hash <core::gfx::arPixel>
{
	size_t operator() ( const core::gfx::arPixel& arg ) const {
		return (size_t)(arg);
	}
};


#endif//CORE_GFX_PIXEL_FORMAT_H_