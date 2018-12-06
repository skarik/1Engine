#ifndef GPU_WRAPPER_SHADER_H_
#define GPU_WRAPPER_SHADER_H_

#include "core/types.h"
#include "renderer/gpuw/Public/ShaderTypes.h"

namespace gpu
{
	class ShaderPipeline;

	class Shader
	{
	public:
		RENDER_API int			createFromFile ( ShaderStage shaderType, const char* file );
		RENDER_API int			createFromBytes ( ShaderStage shaderType, const void* buffer, const size_t len );
		RENDER_API int			destroy ( void );

	private:
		friend ShaderPipeline;
		ShaderStage		m_type;
		char*			m_shaderBytes;
		uint64_t		m_shaderLength;
	};
}

#endif//GPU_WRAPPER_SHADER_H_