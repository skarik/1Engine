#ifndef GPU_WRAPPER_SHADER_PIPELINE_H_
#define GPU_WRAPPER_SHADER_PIPELINE_H_

#include "core/types.h"
#include "renderer/gpuw/Public/ShaderTypes.h"

namespace gpu
{
	class Shader;

	class ShaderPipeline
	{
	public:
		RENDER_API int			attach ( Shader* shader, const char* entrypoint_symbol );
		
		RENDER_API int			assemble ( void );
		RENDER_API int			destroy ( void );

		RENDER_API bool			valid ( void );

	private:
		Shader*	m_shaderVs;
		Shader*	m_shaderHs;
		Shader*	m_shaderDs;
		Shader*	m_shaderGs;
		Shader*	m_shaderPs;
	};
}

#endif//GPU_WRAPPER_SHADER_PIPELINE_H_