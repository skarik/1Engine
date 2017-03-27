
#ifndef _RENDERER_OPTIONS_H_
#define _RENDERER_OPTIONS_H_

#include "core/types/types.h"
#include "core/types/float.h"

namespace renderer
{
	class Options
	{
	public:
		RENDER_API static void TextureStreaming ( const bool enabled );
	};
}


#endif//_RENDERER_OPTIONS_H_