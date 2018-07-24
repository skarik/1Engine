#ifndef GPU_WRAPPER_PIPELINE_H_
#define GPU_WRAPPER_PIPELINE_H_

#include "core/types/types.h"
#include "renderer/types/types.h"
#include "renderer/gpuw/Public/Formats.h"
#include "renderer/gpuw/Public/Enums.h"

namespace gpu
{
	class ShaderPipeline;

	enum InputRate
	{
		kInputRatePerVertex,
		kInputRatePerInstance,
	};

	struct VertexInputBindingDescription
	{
		uint32_t    binding;
		uint32_t	stride;
		InputRate	inputRate;
	};

	struct VertexInputAttributeDescription
	{
		uint32_t    location;
		uint32_t    binding;
		Format		format;
		uint32_t    offset;
	};
	
	struct PipelineCreationDescription
	{
		// Shader pipeline to be used with this pipeline.
		ShaderPipeline*		shader_pipeline;
		// Vertex shader inputs
		VertexInputBindingDescription*
							vv_inputBindings;
		uint32_t			vv_inputBindingsCount;
		VertexInputAttributeDescription*
							vv_inputAttributes;
		uint32_t			vv_inputAttributesCount;
		// Input assembler outputs
		PrimitiveTopology	ia_topology;
		bool				ia_primitiveRestartEnable;
		// Pixel shader outputs
	};

	// defines a state of shaders, vertex formats to bind to shader inputs, etc.
	class Pipeline
	{
	public:
		//	Constructor : creates uninitalized GPU wrapper object.
		//RENDER_API explicit		Pipeline ( void );
		//	Destructor : destroys any allocated VAO/Layout, if existing.
		//RENDER_API 				~Pipeline ( void );

		// Sets up with default values
		RENDER_API explicit		Pipeline ( void )
		{
			m_vao = 0;
			m_pipeline = NULL;
		}

		//RENDER_API void			setShaderPipeline
		//RENDER_API int			setShaderPipeline ( ShaderPipeline* pipeline );

		//RENDER_API bool			assemble ( void );
		RENDER_API int			create ( const PipelineCreationDescription& params );

		RENDER_API int			destroy ( void );

	private:
		
		unsigned int		m_vao;
		ShaderPipeline*		m_pipeline;

	};
}

#endif//GPU_WRAPPER_PIPELINE_H_