#ifndef RENDER2D_PALETTE_TO_LUT_H_
#define RENDER2D_PALETTE_TO_LUT_H_

#include "core/types.h"
#include "core/gfx/pixelFormat.h"

namespace render2d
{
	namespace preprocess
	{
		//	CreatePalatteFromData
		// Creates an unordered palette from the data
		// Actual final palette should be tweaked by the user before used in the DataToLUT conversion tool.
		//RENDER2D_API void CreatePaletteFromData ( const pixel_t* n_pixel_data, const uint n_pixel_count, pixel_t* o_palette, uint& io_palette_size );

		//	DataToLUT( io_data, io_data_elements, palette, palette_elements, palette_width )
		// Given a palette and image, turns an image into an XY lookup coordinate in the palette.
		RENDER2D_API void		DataToLUT ( core::gfx::arPixel* io_pixel_data,
											const uint n_pixel_count,
											const core::gfx::arPixel* n_palette,
											const uint n_palette_size,
											const uint n_palette_width );

		//	CombineLUT( io_palette, palette_elements, new_palette, new_palette_elements, new_palette_width )
		// Given to LUTs of equal width, combines the two into the first, taking into account duplicate rows when able
		// Returns new size of palette
		RENDER2D_API uint		CombineLUT ( core::gfx::arPixel* n_palette,
											 const uint n_palette_size,
											 core::gfx::arPixel* n_palette_to_add,
											 const uint n_palette_to_add_size,
											 const uint n_palette_width );
	};
};

#endif//RENDER2D_PALETTE_TO_LUT_H_