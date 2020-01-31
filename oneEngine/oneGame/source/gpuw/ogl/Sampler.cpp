#include "gpuw/gpuw_common.h"
#ifdef GPU_API_OPENGL

#include "./Sampler.h"
#include "gpuw/Public/Error.h"
#include "./Internal/Enums.h"
#include "./ogl/GLCommon.h"

int gpu::Sampler::create ( Device* device, const SamplerCreationDescription* params )
{
	if (m_sampler == 0)
	{
		glCreateSamplers(1, &m_sampler);
	}

	gpuEnum magFilter = gpu::internal::ArEnumToGL(params->magFilter, false, params->mipmapMode);
	gpuEnum minFilter = gpu::internal::ArEnumToGL(params->minFilter, params->mipmaps, params->mipmapMode);

	glSamplerParameteri(m_sampler, GL_TEXTURE_MAG_FILTER, magFilter);
	glSamplerParameteri(m_sampler, GL_TEXTURE_MIN_FILTER, minFilter);

	glSamplerParameterf(m_sampler, GL_TEXTURE_MAX_ANISOTROPY, params->anisotropy ? params->maxAnisotropy : 0.0F);

	glSamplerParameterf(m_sampler, GL_TEXTURE_LOD_BIAS, params->mipmapLodBias);
	glSamplerParameterf(m_sampler, GL_TEXTURE_MIN_LOD, params->mipmapMinLod);
	glSamplerParameterf(m_sampler, GL_TEXTURE_MAX_LOD, params->mipmapMaxLod);

	return kError_SUCCESS;
}
int gpu::Sampler::destroy ( Device* device )
{
	glDeleteSamplers(1, &m_sampler);
	m_sampler = 0;
	return kError_SUCCESS;
}

//	valid() : is this sampler valid to be used?
bool gpu::Sampler::valid ( void )
{
	return m_sampler != 0;
}
//	nativePtr() : returns native index or pointer to the resource.
gpuHandle gpu::Sampler::nativePtr ( void )
{
	return m_sampler;
}


#endif