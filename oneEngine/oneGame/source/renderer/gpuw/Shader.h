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
		unsigned int	m_handle;
		ShaderStage		m_type;
	};
}

#endif//GPU_WRAPPER_SHADER_H_