
#include "WorldPalette.h"
#include "renderer/texture/CTexture.h"
#include "renderer/texture/CTexture3D.h"
#include "render2d/preprocess/PaletteToLUT.h"

using namespace Render2D;

WorldPalette* WorldPalette::m_active_palette = NULL;
WorldPalette* WorldPalette::Active ( void )
{
	if ( m_active_palette == NULL )
	{
		m_active_palette = new WorldPalette;
	}
	return m_active_palette;
}

WorldPalette::WorldPalette ( void )
	: palette_texture(NULL), palette3d_texture(NULL)
{
	memset(palette_data, 0x0000FF00, sizeof(palette_data));
	palette_size = 0;
	palette_width = 0;
}
WorldPalette::~WorldPalette ( void )
{

}

//	Reset () : Clears out the palette, forcing any sprites to re-add themselves
void WorldPalette::Reset ( void )
{
	palette_size = 0;
	palette_width = 0;
}

//	GetTexture () : Returns render texture with the current palette. Will update it on request if needed.
CTexture* WorldPalette::GetTexture ( void )
{
	if ( palette_texture == NULL )
	{
		palette_texture = new CTexture("_hx_SYSTEM_SKIP");
		palette3d_texture = new CTexture3D("_hx_SYSTEM_SKIP");
		palette_texture_needs_update = true;
	}
	if ( palette_texture_needs_update )
	{
		palette_texture->Upload(palette_data, palette_width, MAX_HEIGHT, Clamp, Clamp, MipmapNone, SamplingPoint);
		BuildAndUpdatePalette3D();
		palette_texture_needs_update = false;
	}

	return palette_texture;
}
//	GetTexture3D ()
// Returns 3D texture which is a distance-matched RGB lookup with current palette.
// Will update it on request if needed.
CTexture3D* WorldPalette::GetTexture3D ( void )
{
	GetTexture(); // Force update
	return palette3d_texture;
}

//	AddPalette ()
// Adds a palette to current internal palette.
// Returns true if successful. Returns false if the widths were not equal, or ran out of room.
bool WorldPalette::AddPalette ( pixel_t* n_palette_to_add, const uint n_palette_to_add_size, const uint n_palette_width )
{
	if ( palette_width != 0 && palette_width != n_palette_width )
	{	// Incoming palette is not the same width, so don't allow for combining.
		return false;
	}
	if ( palette_size + n_palette_to_add_size >= MAX_HEIGHT )
	{	// Going to run out of room, so don't allow for combining.
		return false;
	}

	// No palette yet, so we just copy it over
	if ( palette_width == 0 )
	{
		// Do init of the first row
		for ( uint i = 0; i < n_palette_width; ++i )
		{
			palette_data[i].r = 0;
			palette_data[i].g = 0;
			palette_data[i].b = 0;
			palette_data[i].a = 255;
		}

		// Copy over to just past the first row
		palette_width = n_palette_width;
		palette_size = n_palette_to_add_size + 1;
		memcpy( palette_data + n_palette_width, n_palette_to_add, sizeof(pixel_t) * n_palette_to_add_size * n_palette_width );
	}
	// Use the LUT utilities to combile the palettes (if there will be room)
	else
	{
		palette_size = Preprocess::CombineLUT( palette_data, palette_size, n_palette_to_add, n_palette_to_add_size, palette_width );
	}

	// Need an update for next load
	palette_texture_needs_update = true;

	return true;
}


//	BuildAndUpdatePalette3D ()
// Rebuilds and updates the 3D palette based on the current world palette.
void WorldPalette::BuildAndUpdatePalette3D ( void )
{
	const int lookup3d_depth = 64;
	const int lookup3d_size = lookup3d_depth * lookup3d_depth * lookup3d_depth;

	const int offset_x = 1;
	const int offset_y = lookup3d_depth;
	const int offset_z = lookup3d_depth * lookup3d_depth;

	pixel_t* lookup3d_data = new pixel_t [lookup3d_size];
	// Start with empty pixels
	memset( lookup3d_data, 0, sizeof(pixel_t) * lookup3d_size );

#if 0
	// Start off placing each palette color in the palette
	for ( uint i = 0; i < palette_size * palette_width; ++i )
	{
		int tx = (int)(palette_data[i].r * (lookup3d_depth / 256.0F));
		int ty = (int)(palette_data[i].g * (lookup3d_depth / 256.0F));
		int tz = (int)(palette_data[i].b * (lookup3d_depth / 256.0F));
		lookup3d_data[tx + ty * offset_y + tz * offset_z] = palette_data[i];
	}

	// Now loop repeatedly through the palette until it has been completely filled with color
	for ( int n = 0; n < lookup3d_depth; ++n )
	{
		for ( int i = 0; i < lookup3d_size; ++i )
		{
			// Loop through all the empty pixels:
			if ( lookup3d_data[i].a ) continue;

			// Create the table of pixels to grab colors from
			int lookup_indices [6] = {
				i - offset_x, i + offset_x,
				i - offset_y, i + offset_y,
				i - offset_z, i + offset_z
			};
			pixel_t* lookups [6] = {0};
			for ( uint m = 0; m < 6; ++m )
			{
				if (lookup_indices[m] >= 0 && lookup_indices[m] <= lookup3d_size)
					lookups[m] = lookup3d_data + lookup_indices[m] - offset_x;
			}

			// Create current position
			int color_x = i % lookup3d_depth;
			int color_y = (i / lookup3d_depth) % lookup3d_depth;
			int color_z = i / (lookup3d_depth * lookup3d_depth);

			// Loop through the samples and find the closest (valid) one
			uint closest = 255;
			int closest_distance = 16777216;
			for ( uint m = 0; m < 6; ++m )
			{
				if ( !lookups[m] || !lookups[m]->a ) continue; // Skip nonset pixels

				// Calculate distance to the color
				//int distance = (color_x - lookups[m]->r) * (color_y - lookups[m]->g) * (color_z - lookups[m]->b); // neat glitch
				int distance = sqr(color_x - lookups[m]->r) + sqr(color_y - lookups[m]->g) + sqr(color_z - lookups[m]->b);

				// If nonset, or current color is closer, save that color
				if ( closest == 255 || distance < closest_distance )
				{
					closest = m;
					closest_distance = distance;
				}
			}

			// If closest is set, use that color
			if ( closest != 255 )
			{
				lookup3d_data[i] = *lookups[closest];
				lookup3d_data[i].a = 255;
			}
		}
		// Done with one spread iteration
	}
#endif

	for ( int i = 0; i < lookup3d_size; ++i )
	{
		// Create current position
		int color_x = i % lookup3d_depth;
		int color_y = (i / lookup3d_depth) % lookup3d_depth;
		int color_z = i / (lookup3d_depth * lookup3d_depth);

		color_x *= (256 / lookup3d_depth);
		color_y *= (256 / lookup3d_depth);
		color_z *= (256 / lookup3d_depth);

		// Loop through the palette and find the closest (valid) one
		uint closest = 255;
		int closest_distance = 16777216;
		for ( uint m = 0; m < palette_size * palette_width; ++m )
		{
			// Calculate distance to the color
			int distance = sqr(color_x - palette_data[m].r) + sqr(color_y - palette_data[m].g) + sqr(color_z - palette_data[m].b);

			// If nonset, or current color is closer, save that color
			if ( closest == 255 || distance < closest_distance )
			{
				closest = m;
				closest_distance = distance;
			}
		}

		lookup3d_data[i] = palette_data[closest];
	}

	// Now that the palette is built, upload it.
	palette3d_texture->Upload(lookup3d_data, lookup3d_depth, lookup3d_depth, lookup3d_depth, Clamp, Clamp, Clamp, MipmapNone, SamplingPoint);

	// Free unused data
	delete [] lookup3d_data;
}