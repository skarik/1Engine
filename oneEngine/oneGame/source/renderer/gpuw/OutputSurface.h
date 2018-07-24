#ifndef GPU_WRAPPER_OUTPUT_SURFACE_H_
#define GPU_WRAPPER_OUTPUT_SURFACE_H_

#include "core/types/types.h"
#include "renderer/types/types.h"
#include "renderer/gpuw/Public/Formats.h"

namespace gpu
{
	class Device;
	class RenderTarget;
	class OutputSurface
	{
	public:
		RENDER_API int			create ( Device* device );
		RENDER_API int			destroy ( void );
		RENDER_API RenderTarget*
								getRenderTarget ( void );
	};
}

#endif//GPU_WRAPPER_OUTPUT_SURFACE_H_