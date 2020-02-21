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

		//	attach( shader, entrypoint_symbol ) : Adds a shader to a graphics shader pipeline.
		GPUW_API int			attach ( Shader* shader, const char* entrypoint_symbol );
		//	attachCompute( shader, entrypoint_symbol ) : Adds a shader to a compute shader pipeline.
		GPUW_API int			attachCompute ( Shader* shader, const char* entrypoint_symbol );
		//	attachRaytracing( shader, entrypoint_symbol ) : Adds a shader to the raytracing shhader pipeline.
		GPUW_API int			attachRaytracing ( Shader* shader, const char* entrypoint_symbol );
		//	assemble() : Assembles all attached shaders into a functional pipeline
		GPUW_API int			assemble ( void );

		//	destroy() : Frees created program after assemble
		GPUW_API int			destroy ( void );

		//	valid() : Does this have a program that is ready to be used?
		GPUW_API bool			valid ( void );

	private:
		friend GraphicsContext;
		friend ComputeContext;

		PipelineType
					m_type;

		union
		{
			struct
			{
				Shader*		m_shaderVs;
				Shader*		m_shaderHs;
				Shader*		m_shaderDs;
				Shader*		m_shaderGs;
				Shader*		m_shaderPs;
			};
			struct
			{
				Shader*		m_shaderCs;
			};
			struct
			{
				Shader*		m_shaderRGen;
				Shader*		m_shaderRInt;
				Shader*		m_shaderRCHit;
				Shader*		m_shaderRAHit;
				Shader*		m_shaderRMiss;
			};
		};

		union
		{
			struct
			{
				gpuHandle	m_vs;
				gpuHandle	m_hs;
				gpuHandle	m_ds;
				gpuHandle	m_gs;
				gpuHandle	m_ps;
			};
			struct
			{
				gpuHandle	m_cs;
			};
		};

		gpuHandle	m_program;
	};
}

#endif//GPU_WRAPPER_SHADER_PIPELINE_H_