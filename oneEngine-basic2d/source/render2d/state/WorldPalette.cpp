
#include "WorldPalette.h"
#include "renderer/texture/CRenderTexture.h"
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
{
	memset(palette_data, 0, sizeof(palette_data));
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
		palette_texture_needs_update = true;
	}
	if ( palette_texture_needs_update )
	{
		palette_texture->Upload(palette_data, palette_width, MAX_HEIGHT, Clamp, Clamp, MipmapNone, SamplingPoint);
		palette_texture_needs_update = false;
	}

	return palette_texture;
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
		palette_width = n_palette_width;
		palette_size = n_palette_to_add_size;
		memcpy( palette_data, n_palette_to_add, sizeof(pixel_t) * n_palette_to_add_size * n_palette_width );
	}
	// Use the LUT utilities to combile the palettes (if there will be room)
	else
	{
		palette_size = Preprocess::CombineLUT( palette_data, palette_size, n_palette_to_add, n_palette_to_add_size, palette_width );
	}

	return true;
}