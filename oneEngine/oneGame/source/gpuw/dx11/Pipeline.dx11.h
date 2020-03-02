#ifndef GPU_WRAPPER_PIPELINE_H_
#define GPU_WRAPPER_PIPELINE_H_

#include "core/types/types.h"
#include "renderer/types/types.h"
#include "gpuw/Public/Formats.h"
#include "gpuw/Public/Enums.h"
#include "gpuw/Public/PipelineCreationParams.h"

namespace gpu
{
	class ShaderPipeline;
	class Device;
	class GraphicsContext;
	class ComputeContext;
	class Buffer;

	// defines a state of shaders, vertex formats to bind to shader inputs, etc.
	class Pipeline
	{
	public:
		// Sets up with default values
		GPUW_API explicit		Pipeline ( void )
		{
			m_layout = NULL;
			m_pipeline = NULL;
			ia_bindingInfo = NULL;
		}

		GPUW_API int			create ( Device* device, const PipelineCreationDescription* params );

		GPUW_API int			destroy ( Device* device );

		//	valid() : is this pipeline valid to be used?
		// If the pipeline failed to be created or doesnt exist, this will be false
		GPUW_API bool			valid ( void );
		//	nativePtr() : returns native index or pointer to the resource.
		GPUW_API gpuHandle		nativePtr ( void );

	private:
		friend GraphicsContext;
		friend ComputeContext;

		void*				m_layout;
		ShaderPipeline*		m_pipeline;
		Buffer*				m_boundIndexBuffer;

		// Input assembler outputs
		PrimitiveTopology	ia_topology;
		bool				ia_primitiveRestartEnable;
		VertexInputBindingDescription*
							ia_bindingInfo;
		uint32_t			ia_bindingInfoCount;
	};
}

#endif//GPU_WRAPPER_PIPELINE_H_