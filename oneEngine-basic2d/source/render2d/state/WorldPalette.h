
#ifndef _RENDER2D_WORLD_PALETTE_H_
#define _RENDER2D_WORLD_PALETTE_H_

#include "core/types.h"
#include "renderer/types/pixelFormat.h"

class CTexture;
class CRenderTexture;

namespace Render2D
{
	class WorldPalette
	{
		// Singleton definition:
	private:
		static WorldPalette* m_active_palette;
	public:
		RENDER2D_API static WorldPalette* Active ( void );

	private:
		// Constructor & Destructor
			WorldPalette ( void );
			~WorldPalette ( void );

	public:

		//	Reset ()
		// Clears out the palette, forcing any sprites to re-add themselves
		RENDER2D_API void Reset ( void );

		//	GetTexture ()
		// Returns render texture with the current palette. Will update it on request if needed.
		RENDER2D_API CTexture* GetTexture ( void );

		//	AddPalette ()
		// Adds a palette to current internal palette.
		// Returns true if successful. Returns false if the widths were not equal, or ran out of room.
		RENDER2D_API bool AddPalette ( pixel_t* n_palette_to_add, const uint n_palette_to_add_size, const uint n_palette_width );

		static const uint MAX_HEIGHT = 128;
		static const uint MAX_COLORS = 32;

		pixel_t palette_data [MAX_COLORS * MAX_HEIGHT];
		uint palette_size;
		uint palette_width;

		CTexture*	palette_texture;
		bool palette_texture_needs_update;
	};
}

#endif//_RENDER2D_WORLD_PALETTE_H_