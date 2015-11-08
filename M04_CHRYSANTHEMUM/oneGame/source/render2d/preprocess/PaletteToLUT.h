
#ifndef _RENDER2D_PALETTE_TO_LUT_H_
#define _RENDER2D_PALETTE_TO_LUT_H_

#include "core/types.h"
#include "renderer/types/pixelFormat.h"

namespace Render2D
{
	namespace Preprocess
	{
		//	CreatePalatteFromData
		// Creates an unordered palette from the data
		// Actual final palette should be tweaked by the used before used in the DataToLUT conversion tool.
		RENDER2D_API void CreatePaletteFromData ( const tPixel* n_pixel_data, const uint n_pixel_count, tPixel* o_palette, uint& io_palette_size );
		//	DataToLUT
		// Given a palette and image, turns an image into an XY lookup coordinate in the palette.
		RENDER2D_API void DataToLUT ( tPixel* io_pixel_data, const uint n_pixel_count, const tPixel* n_palette, const uint n_palette_size, const uint n_palette_width );
	};
};

#endif//_RENDER2D_PALETTE_TO_LUT_H_