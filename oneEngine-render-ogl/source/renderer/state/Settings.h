

#ifndef _RENDER_SETTINGS_STRUCT_
#define _RENDER_SETTINGS_STRUCT_

// Includes
//#include "CGameSettings.h"
#include "core/types/float.h"
#include "core/math/Color.h"

class CRenderState;
class glMaterial;

namespace Renderer
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
		friend glMaterial;

		Real	fogScale;
	};

	// Bleh
	RENDER_API extern renderSettings_t Settings;
}


#endif