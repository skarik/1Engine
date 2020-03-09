#ifndef _GPU_WRAPPER_SAMPLER_H_
#define _GPU_WRAPPER_SAMPLER_H_

#include "core/types/types.h"
#include "renderer/types/types.h"
#include "gpuw/Public/Sampler.h"

namespace gpu
{
	class Device;

	// defines how a texture is sampled within the shader
	class Sampler
	{
	public:
		GPUW_API explicit			Sampler ( void )
			: m_sampler(NULL)
			{}

		GPUW_API int			create ( Device* device, const SamplerCreationDescription* params );
		GPUW_API int			destroy ( Device* device );

		//	valid() : is this sampler valid to be used?
		GPUW_API bool			valid ( void );
		//	nativePtr() : returns native index or pointer to the resource.
		GPUW_API gpuHandle		nativePtr ( void );

	private:
		void*				m_sampler;
	};

}

#endif//_GPU_WRAPPER_TEXTURES_H_