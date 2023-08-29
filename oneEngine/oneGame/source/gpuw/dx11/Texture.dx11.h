#ifndef _GPU_WRAPPER_TEXTURES_H_
#define _GPU_WRAPPER_TEXTURES_H_

#include "core/types/types.h"
#include "core/gfx/textureFormats.h"
#include "renderer/types/types.h"

#include "gpuw/base/Texture.base.h"

namespace gpu {
namespace dx11
{
	class Buffer;
	class GraphicsContext;
	class ComputeContext;
	class RenderTarget;
	class BaseContext;
	class WriteableResource;

	// Create a read/write texture.
	class Texture : public base::Texture
	{
	public:
		//	valid() : is this texture valid to be used?
		// If the texture has not been created, it will be removed.
		GPUW_API bool			valid ( void ) const override;
		//	nativePtr() : returns native index or pointer to the resource.
		GPUW_API gpuHandle		nativePtr ( void ) override;

		//	allocate() : creates a texture
		GPUW_API int			allocate (
			const core::gfx::tex::arTextureType textureType,
			const core::gfx::tex::arColorFormat textureFormat, 
			const uint width = 0, const uint height = 0, const uint depth = 0, const uint levels = 0
		) override;

		// Sampler is in a different object.

		//	free() : destroys any allocated texture, if existing.
		GPUW_API int			free ( void ) override;

		//	upload() : uploads data to the texture from a buffer
		GPUW_API int			upload ( gpu::base::BaseContext* context, gpu::base::Buffer& buffer, const uint level, const uint arraySlice ) override;

		//	copy() : uploads data to a buffer from this texture
		GPUW_API int			copy ( gpu::base::BaseContext* context, gpu::base::Buffer& buffer, const uint level, const uint arraySlice ) override;

	private:
		friend GraphicsContext;
		friend ComputeContext;
		friend RenderTarget;
		friend WriteableResource;

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
	class WOFrameAttachment : public base::WOFrameAttachment
	{
	public:
		//	valid() : is this texture valid to be used?
		// If the texture has not been created, it will be removed.
		GPUW_API bool			valid ( void ) override;
		//	nativePtr() : returns native index or pointer to the resource.
		GPUW_API gpuHandle		nativePtr ( void ) override;

		//	allocate() : creates a texture
		GPUW_API int			allocate (
			const core::gfx::tex::arTextureType textureType,
			const core::gfx::tex::arColorFormat textureFormat, 
			const uint width = 0, const uint height = 0, const uint depth = 0, const uint levels = 0
		) override;

		//	free() : destroys any allocated texture, if existing.
		GPUW_API int			free ( void ) override;

	private:
		friend GraphicsContext;
		friend ComputeContext;
		friend RenderTarget;

		void*							m_texture = NULL;
		core::gfx::tex::arTextureType	m_type;
		core::gfx::tex::arColorFormat	m_format;
		unsigned int					m_dxFormat = 0;
	};
}}

#endif//_GPU_WRAPPER_TEXTURES_H_