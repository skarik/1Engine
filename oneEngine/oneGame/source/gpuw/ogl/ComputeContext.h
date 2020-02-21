#ifndef GPU_WRAPPER_COMPUTE_CONTEXT_H_
#define GPU_WRAPPER_COMPUTE_CONTEXT_H_

#include "core/types.h"
#include "gpuw/Public/ShaderTypes.h"
#include "gpuw/Public/Indirect.h"

namespace gpu
{
	class ShaderPipeline;
	class Pipeline;
	class Buffer;
	class Fence;
	class Sampler;
	class Texture;

	class ComputeContext
	{
	public:
		GPUW_EXLUSIVE_API explicit
								ComputeContext ( void );
		GPUW_EXLUSIVE_API 		~ComputeContext ( void );

		//	reset() : Resets the context state to defaults.
		// Note that on some API's, this will do nothing.
		GPUW_API int			reset ( void );

		//	submit() : Submits the current graphics commands in the queue.
		GPUW_API int			submit ( void );

		//	validate() : Checks to make sure submitted graphics commands are valid.
		// Returns gpu::kError_SUCCESS on no error.
		GPUW_API int			validate ( void );

		//	setPipeline( pipeline ) : Sets current pipeline.
		// Combination set for shader pipeline.
		GPUW_API int			setPipeline ( Pipeline* pipeline );
		//	setShaderCBuffer( stage, slot, buffer ) : Sets buffer to given slot, as a ConstantBuffer.
		// Size is limited to 4kb on some platforms.
		GPUW_API int			setShaderCBuffer ( ShaderStage stage, int slot, Buffer* buffer );
		//	setShaderSBuffer( stage, slot, buffer ) : Sets buffer to given slot, as a StructuredBuffer.
		// Size must be at least 1kb on some platforms.
		// For compute stages, acts as a fast alias for setShaderResource.
		GPUW_API int			setShaderSBuffer ( ShaderStage stage, int slot, Buffer* buffer );
		//	setShaderSampler( stage, slot, sampler ) : Sets a sampler to a given slot.
		GPUW_API int			setShaderSampler ( ShaderStage stage, int slot, Sampler* buffer );
		//	setShaderSampler( stage, slot, sampler ) : Sets a texture to a given slot, without a sampler.
		GPUW_API int			setShaderTexture ( ShaderStage stage, int slot, Texture* buffer );
		//	setShaderTextureAuto( stage, slot, texture ) : Sets texture to given slot, using an automatically generated sampler.
		GPUW_API int			setShaderTextureAuto ( ShaderStage stage, int slot, Texture* buffer );
		//	setShaderResource( stage, slot, buffer ) : Sets a buffer to a given slot, as a generic data buffer.
		GPUW_API int			setShaderResource ( ShaderStage stage, int slot, Buffer* buffer );

		//	dispatch( groupCountX, groupCountY, groupCountZ ) : Dispatches a compute job.
		GPUW_API int			dispatch ( const uint32_t groupCountX, const uint32_t groupCountY, const uint32_t groupCountZ );
		//	setIndirectArgs( buffer ) : sets buffer used for arguments by the next dispatchIndirect call.
		GPUW_API int			setIndirectArgs ( Buffer* buffer );
		//	dispatch( groupCountX, groupCountY, groupCountZ ) : Dispatches a compute job, arguments from indirect data.
		GPUW_API int			dispatchIndirect ( const uint32_t offset );

		//	signal( fence ) : Inserts a command into the command buffer to signal the given fence.
		// This can be used to make either the CPU or GPU wait on a command buffer to reach a certain point.
		GPUW_API int			signal ( Fence* fence );
		//	waitOnSignal( fence ): Inserts a command into the command buffer to stall the command stream until the given fence is signaled.
		// This can be used to wait on either a CPU or GPU signal.
		GPUW_API int			waitOnSignal ( Fence* fence );

	private:
		// implementation details:

		Pipeline*				m_pipeline;
		bool					m_pipelineBound;
		bool					m_pipelineDataBound;

		int						dispatchPreparePipeline ( void );
	};
}

#endif//GPU_WRAPPER_COMPUTE_CONTEXT_H_