#ifndef _GPU_WRAPPER_SAMPLER_H_
#define _GPU_WRAPPER_SAMPLER_H_

#include "core/types/types.h"
#include "core/gfx/textureFormats.h"
#include "renderer/types/types.h"

namespace gpu
{
	class Texture;

	class Sampler
	{
	public:
		//	valid() : is this sampler valid to be used?
		// If the texture has not been created, it will be removed.
		RENDER_API bool			valid ( void );
		//	nativePtr() : returns native index or pointer to the resource.
		RENDER_API gpuHandle	nativePtr ( void );

	private:
		Texture*		m_texture;
	};

}

#endif//_GPU_WRAPPER_TEXTURES_H_