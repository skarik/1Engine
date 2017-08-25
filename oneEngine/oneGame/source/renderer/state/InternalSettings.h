#ifndef _RENDER_INTERNAL_SETTINGS_STRUCT_
#define _RENDER_INTERNAL_SETTINGS_STRUCT_

// Includes
#include "core/types/float.h"
#include "core/math/Color.h"
#include "renderer/types/textureFormats.h"

class CRenderState;
class RrMaterial;

namespace renderer
{
	// Struct
	struct internalSettings_t
	{
		eColorFormat	mainColorAttachmentFormat;
		uint			mainColorAttachmentCount;
		eDepthFormat	mainDepthFormat;
		eStencilFormat	mainStencilFormat;
	};
}

#endif//_RENDER_INTERNAL_SETTINGS_STRUCT_