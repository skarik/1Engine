#ifndef GPU_WRAPPER_SHADER_PIPELINE_H_
#define GPU_WRAPPER_SHADER_PIPELINE_H_

#include "core/types.h"
#include "renderer/types/types.h"
#include "gpuw/Public/ShaderTypes.h"

namespace gpu
{
	class Shader;
	class GraphicsContext;
	class ComputeContext;

	class ShaderPipeline
	{
	public:
		GPUW_API				ShaderPipeline ( void );

		GPUW_API int			attach ( Shader* shader, const char* entrypoint_symbol );
		
		GPUW_API int			assemble ( void );
		GPUW_API int			destroy ( void );

		GPUW_API bool			valid ( void );

	private:
		friend GraphicsContext;
		friend ComputeContext;

		Shader*		m_shaderVs;
		Shader*		m_shaderHs;
		Shader*		m_shaderDs;
		Shader*		m_shaderGs;
		Shader*		m_shaderPs;

		gpuHandle	m_vs;
		gpuHandle	m_hs;
		gpuHandle	m_ds;
		gpuHandle	m_gs;
		gpuHandle	m_ps;

		gpuHandle	m_program;
	};
}

#endif//GPU_WRAPPER_SHADER_PIPELINE_H_