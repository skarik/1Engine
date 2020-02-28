#ifndef GPU_WRAPPER_SHADER_H_
#define GPU_WRAPPER_SHADER_H_

#include "core/types.h"
#include "gpuw/Public/ShaderTypes.h"

namespace gpu
{
	class ShaderPipeline;
	class Pipeline;

	class Shader
	{
	public:
		GPUW_API				Shader ( void );

		GPUW_API int			createFromFile ( ShaderStage shaderType, const char* file );
		GPUW_API int			createFromBytes ( ShaderStage shaderType, const void* buffer, const size_t len );
		GPUW_API int			destroy ( void );

	private:
		friend ShaderPipeline;
		friend Pipeline;
		ShaderStage		m_type;
		char*			m_shaderBytes;
		uint64_t		m_shaderLength;
	};
}

#endif//GPU_WRAPPER_SHADER_H_