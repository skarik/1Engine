#ifndef _RENDER_SETTINGS_STRUCT_
#define _RENDER_SETTINGS_STRUCT_

// Includes
#include "core/types/float.h"
#include "core/math/Color.h"

class CRenderState;
class RrMaterial;

namespace renderer
{
	// Struct
	struct renderSettings_t
	{
		bool	lightingEnabled;
		Color	clearColor;
		Color	ambientColor;
		int		maxLights;
		short	swapIntervals;
		Real	fogStart;
		Real	fogEnd;
		Color	fogColor;
		bool	fogEnabled;	
	private:
		friend CRenderState;
		friend RrMaterial;

		Real	fogScale;
	};

	// Bleh
	RENDER_API extern renderSettings_t Settings;
}

#endif//_RENDER_SETTINGS_STRUCT_