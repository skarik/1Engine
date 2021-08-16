#ifndef GPU_WRAPPER_WRITEABLE_RESOURCE_H_
#define GPU_WRAPPER_WRITEABLE_RESOURCE_H_

#include "core/types/types.h"
#include "core/gfx/textureFormats.h"
#include "renderer/types/types.h"

namespace gpu
{
	class Buffer;
	class Texture;
	class GraphicsContext;
	class ComputeContext;
	class RenderTarget;
	class BaseContext;

	// Create a write-view on a resource.
	class WriteableResource
	{
	public:
		//	valid() : is this texture valid to be used?
		// If the texture has not been created, it will be removed.
		GPUW_API bool			valid ( void );
		//	nativePtr() : returns native index or pointer to the resource.
		GPUW_API gpuHandle		nativePtr ( void );

		GPUW_API int			create ( Texture* texture, const uint32 mipIndex );
		GPUW_API int			create ( Buffer* buffer );

		GPUW_API int			destroy ( void );

	private:
		Buffer*				m_buffer = NULL;
		Texture*			m_texture = NULL;
		void*				m_uav = NULL;

	};
}

#endif//GPU_WRAPPER_WRITEABLE_RESOURCE_H_