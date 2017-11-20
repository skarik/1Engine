#ifndef RENDERER_OPTIONS_H_
#define RENDERER_OPTIONS_H_

#include "core/types/types.h"
#include "core/types/float.h"

#include "renderer/types/ShaderSettings.h"

namespace renderer
{
	class Options
	{
	public:
		RENDER_API static void TextureStreaming ( const bool enabled );
		RENDER_API static void DeferredShaders ( const rrDeferredShaderSettings& shaderInfo );
	};
}

#endif//RENDERER_OPTIONS_H_