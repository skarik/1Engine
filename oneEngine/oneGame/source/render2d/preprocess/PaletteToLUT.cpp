#include "PaletteToLUT.h"
#include "core/math/Math2d.h"
#include "core/math/vect2d_template.h"

#include <map>

//	DataToLUT
// Given a palette and image, turns an image into an XY lookup coordinate in the palette.
void render2d::preprocess::DataToLUT ( core::gfx::arPixel* io_pixel_data, const uint n_pixel_count, const core::gfx::arPixel* n_palette, const uint n_palette_size, const uint n_palette_width )
{
	core::gfx::arPixel* current_pixel = io_pixel_data;
	std::map<core::gfx::arPixel, Vector2i> previous_cases;

	// Loop through each pixel in the image
	for (uint i = 0; i < n_pixel_count; ++i)
	{
		// Skip transparent pixels
		if (current_pixel->a == 0)
		{
			current_pixel++;
			continue;
		}

		auto case_data = previous_cases.find(*current_pixel);

		// Missing lookup data, need to find it:
		if (case_data == previous_cases.end())
		{
			bool work = true;
			for (uint row = 0; work && row < n_palette_size; ++row)
			{
				for (uint column = 0; work && column < n_palette_width; ++column)
				{
					if (*current_pixel == n_palette[column + row * n_palette_width])
					{
						// Update case data now
						case_data = previous_cases.insert(
								std::pair<core::gfx::arPixel, Vector2i>(*current_pixel, Vector2i(column, row))
							).first; // First returns an iterator to the new data.
						// Break out
						work = false;
						break;
					}
				}
			}
		}

		// Change color to the XY position on the lookup table
		if (case_data != previous_cases.end())
		{
			current_pixel->r = (uint8_t)( 255.0 * ((Real)case_data->second.x + 0.5F) / n_palette_width );
			current_pixel->g = (uint8_t)( 255.0 * ((Real)case_data->second.y + 0.5F) / n_palette_size );
			current_pixel->b = 0;
			current_pixel->a = 255;
		}
		else
		{
			throw core::MissingDataException();
		}
		// Go to next pixel
		current_pixel++;
	}
}

//	CombineLUT
// Given to LUTs of equal width, combines the two into the first, taking into account duplicate rows when able
// Returns new size of palette
uint render2d::preprocess::CombineLUT ( core::gfx::arPixel* n_palette, const uint n_palette_size, core::gfx::arPixel* n_palette_to_add, const uint n_palette_to_add_size, const uint n_palette_width )
{
	uint palette_size = n_palette_size;

	// Loop through each row of the palette we're adding:
	for (uint row = 0; row < n_palette_to_add_size; ++row)
	{
		uint target_row = 0;
		bool unique_row = true;
		for (target_row = 0; target_row < palette_size; ++target_row)
		{
			bool matches = true; // Fail-first test on each row
			for (uint column = 0; column < n_palette_width; ++column)
			{
				if (n_palette[column + target_row * n_palette_width] != n_palette_to_add[column + row * n_palette_width])
				{	// If anything is not equal, then we have no match and we go check the next target row
					matches = false;
					break;
				}
			}
			
			if (matches)
			{	// if there is a match, then we no longer have a unique row
				unique_row = false;
				break;
			}
		}

		// Not a unique row, skip it
		if (!unique_row)
		{
			continue;
		}
		else
		{
			// Copy the row over to the main palette
			memcpy(n_palette + palette_size*n_palette_width, n_palette_to_add + row*n_palette_width, sizeof(core::gfx::arPixel) * n_palette_width);
			palette_size += 1; // Increment height of the palette
		}
	}
	
	return palette_size;
}

