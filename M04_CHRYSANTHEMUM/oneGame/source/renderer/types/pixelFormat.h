//===============================================================================================//
// pixelFormat.h
//
// Contains definitions for individual pixel data storage
//===============================================================================================//
#ifndef _RENDERER_PIXEL_FORMAT_H_
#define _RENDERER_PIXEL_FORMAT_H_

#include "core/types/types.h"
#include "core/types/float.h"
#include "core/math/Color.h"

// 32 bit pixel struct
struct tPixel
{
	// Data set
	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t a;
	
	// Default constructor
	tPixel ( void ) :
		r(0x00), g(0x00), b(0x00), a(0xFF)
	{
		;
	}
	// Construct from color
	tPixel ( Color& color )
	{
		*((uint32_t*)&r) = color.GetCode();
	}
};
typedef tPixel pixel_t;

// Floating point pixel struct
struct tPixelF
{
	Real_32 r;
	Real_32 g;
	Real_32 b;
	Real_32 a;
};
typedef tPixelF pixelF_t;


#endif//_RENDERER_PIXEL_FORMAT_H_