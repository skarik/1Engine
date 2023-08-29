#ifndef GPU_WRAPPER_BASE_TEXTURES_H_
#define GPU_WRAPPER_BASE_TEXTURES_H_

#include "core/types/types.h"
#include "core/gfx/textureFormats.h"
#include "renderer/types/types.h"

namespace gpu {
namespace base
{
	class Buffer;
	class GraphicsContext;
	class ComputeContext;
	class RenderTarget;
	class BaseContext;
	class WriteableResource;

	// Create a read/write texture.
	class Texture
	{
	public:
		//	valid() : is this texture valid to be used?
		// If the texture has not been created, it will be removed.
		GPUW_API virtual bool	valid ( void ) const =0;
		//	nativePtr() : returns native index or pointer to the resource.
		GPUW_API virtual gpuHandle
								nativePtr ( void ) =0;

		//	allocate() : creates a texture
		GPUW_API virtual int	allocate (
			const core::gfx::tex::arTextureType textureType,
			const core::gfx::tex::arColorFormat textureFormat, 
			const uint width = 0, const uint height = 0, const uint depth = 0, const uint levels = 0
		) =0;

		// Sampler is in a different object.

		//	free() : destroys any allocated texture, if existing.
		GPUW_API virtual int	free ( void ) =0;

		//	upload() : uploads data to the texture from a buffer
		GPUW_API virtual int	upload ( gpu::base::BaseContext* context, gpu::base::Buffer& buffer, const uint level, const uint arraySlice ) =0;

		//	copy() : uploads data to a buffer from this texture
		GPUW_API virtual int	copy ( gpu::base::BaseContext* context, gpu::base::Buffer& buffer, const uint level, const uint arraySlice ) =0;

	protected:
		friend gpu::base::GraphicsContext;
		friend gpu::base::ComputeContext;
		friend gpu::base::RenderTarget;
		friend gpu::base::WriteableResource;
	};

	// Create a write-only texture.
	class WOFrameAttachment
	{
	public:
		//	valid() : is this texture valid to be used?
		// If the texture has not been created, it will be removed.
		GPUW_API virtual bool	valid ( void ) =0;
		//	nativePtr() : returns native index or pointer to the resource.
		GPUW_API virtual gpuHandle
								nativePtr ( void ) =0;
		
		//	allocate() : creates a texture
		GPUW_API virtual int	allocate (
			const core::gfx::tex::arTextureType textureType,
			const core::gfx::tex::arColorFormat textureFormat, 
			const uint width = 0, const uint height = 0, const uint depth = 0, const uint levels = 0
		) =0;

		//	free() : destroys any allocated texture, if existing.
		GPUW_API virtual int	free ( void ) =0;

	protected:
		friend gpu::base::GraphicsContext;
		friend gpu::base::ComputeContext;
		friend gpu::base::RenderTarget;
	};
}}

#endif//GPU_WRAPPER_BASE_TEXTURES_H_