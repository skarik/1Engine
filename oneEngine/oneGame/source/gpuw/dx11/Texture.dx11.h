#ifndef _GPU_WRAPPER_TEXTURES_H_
#define _GPU_WRAPPER_TEXTURES_H_

#include "core/types/types.h"
#include "core/gfx/textureFormats.h"
#include "renderer/types/types.h"

namespace gpu
{
	class Buffer;
	class GraphicsContext;
	class ComputeContext;
	class RenderTarget;
	class BaseContext;

	// Create a read/write texture.
	class Texture
	{
	public:
		//	valid() : is this texture valid to be used?
		// If the texture has not been created, it will be removed.
		GPUW_API bool			valid ( void );
		//	nativePtr() : returns native index or pointer to the resource.
		GPUW_API gpuHandle		nativePtr ( void );

		//	allocate() : creates a texture
		GPUW_API int			allocate (
			const core::gfx::tex::arTextureType textureType,
			const core::gfx::tex::arColorFormat textureFormat, 
			const uint width = 0, const uint height = 0, const uint depth = 0, const uint levels = 0
		);

		// Sampler is in a different object.

		//	free() : destroys any allocated texture, if existing.
		GPUW_API int			free ( void );

		//	upload() : uploads data to the texture from a buffer
		GPUW_API int			upload ( gpu::BaseContext* context, gpu::Buffer& buffer, const uint level, const uint arraySlice );

	private:
		friend GraphicsContext;
		friend ComputeContext;
		friend RenderTarget;

		void*							m_texture = NULL;
		core::gfx::tex::arTextureType	m_type;
		core::gfx::tex::arColorFormat	m_format;
		void*							m_srv = NULL;

		unsigned int					m_width = 0;
		unsigned int					m_height = 0;
		unsigned int					m_depth = 0;
		unsigned int					m_levels = 0;
		unsigned int					m_dxFormat = 0;
	};

	// Create a write-only texture.
	class WOFrameAttachment
	{
	public:
		//	valid() : is this texture valid to be used?
		// If the texture has not been created, it will be removed.
		GPUW_API bool			valid ( void );
		//	nativePtr() : returns native index or pointer to the resource.
		GPUW_API gpuHandle		nativePtr ( void );

		//	allocate() : creates a texture
		GPUW_API int			allocate (
			const core::gfx::tex::arTextureType textureType,
			const core::gfx::tex::arColorFormat textureFormat, 
			const uint width = 0, const uint height = 0, const uint depth = 0, const uint levels = 0
		);

		//	free() : destroys any allocated texture, if existing.
		GPUW_API int			free ( void );

	private:
		friend GraphicsContext;
		friend ComputeContext;
		friend RenderTarget;

		void*							m_texture = NULL;
		core::gfx::tex::arTextureType	m_type;
		core::gfx::tex::arColorFormat	m_format;
		//void*							m_srv;
		unsigned int					m_dxFormat = 0;
	};
}

#endif//_GPU_WRAPPER_TEXTURES_H_