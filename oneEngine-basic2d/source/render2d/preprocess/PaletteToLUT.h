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
		// Actual final palette should be tweaked by the user before used in the DataToLUT conversion tool.
		//RENDER2D_API void CreatePaletteFromData ( const pixel_t* n_pixel_data, const uint n_pixel_count, pixel_t* o_palette, uint& io_palette_size );
		//	DataToLUT
		// Given a palette and image, turns an image into an XY lookup coordinate in the palette.
		RENDER2D_API void DataToLUT ( pixel_t* io_pixel_data, const uint n_pixel_count, const pixel_t* n_palette, const uint n_palette_size, const uint n_palette_width );

		//	CombineLUT
		// Given to LUTs of equal width, combines the two into the first, taking into account duplicate rows when able
		// Returns new size of palette
		RENDER2D_API uint CombineLUT ( pixel_t* n_palette, const uint n_palette_size, pixel_t* n_palette_to_add, const uint n_palette_to_add_size, const uint n_palette_width );
	};
};

#endif//_RENDER2D_PALETTE_TO_LUT_H_