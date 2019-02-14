#ifndef _GPU_WRAPPER_TEXTURES_H_
#define _GPU_WRAPPER_TEXTURES_H_

#include "core/types/types.h"
#include "core/gfx/textureFormats.h"
#include "renderer/types/types.h"

namespace gpu
{
	class Buffer;

	// Create a read/write texture.
	class Texture
	{
	public:
		////	Constructor : creates uninitalized GPU wrapper object.
		//GPUW_API explicit		Texture ( void );
		////	Destructor : destroys any allocated texture, if existing.
		//GPUW_API				~Texture ( void );

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
		GPUW_API int			upload ( gpu::Buffer& buffer, const uint level );

	private:
		unsigned int					m_texture;
		core::gfx::tex::arTextureType	m_type;

		unsigned int					m_width;
		unsigned int					m_height;
		unsigned int					m_depth;
		unsigned int					m_levels;
		gpuEnum							m_glcomponent;
		gpuEnum							m_gltype;
	};

	// Create a write-only texture.
	class WOFrameAttachment
	{
	public:
		////	Constructor : creates uninitalized GPU wrapper object.
		//GPUW_API explicit		WOFrameAttachment ( void );
		////	Destructor : destroys any allocated texture, if existing.
		//GPUW_API				~WOFrameAttachment ( void );

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
		unsigned int					m_texture;
		core::gfx::tex::arTextureType	m_type;
	};
}

#endif//_GPU_WRAPPER_TEXTURES_H_