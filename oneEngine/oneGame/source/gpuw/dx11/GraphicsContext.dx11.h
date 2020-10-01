#ifndef GPU_WRAPPER_GRAPHICS_CONTEXT_H_
#define GPU_WRAPPER_GRAPHICS_CONTEXT_H_

#include "core/types.h"
#include "gpuw/Public/Enums.h"
#include "gpuw/Public/Formats.h"
#include "gpuw/Public/Rect2.h"
#include "gpuw/Public/ShaderTypes.h"
#include "gpuw/Public/Slots.h"
#include "gpuw/Public/States.h"
#include <stdint.h>

namespace gpu
{
	class ShaderPipeline;
	class Pipeline;
	class Fence;
	class Sampler;
	class Texture;
	class Buffer;
	class RenderTarget;

	class GraphicsContext
	{
	public:
		GPUW_EXLUSIVE_API explicit
								GraphicsContext ( void* wrapperDevice );
		GPUW_EXLUSIVE_API 		~GraphicsContext ( void );

		//	reset() : Resets the context state to defaults.
		// Note that on some API's, this will do nothing.
		GPUW_API int			reset ( void );

		//	submit() : Submits the current graphics commands in the queue.
		GPUW_API int			submit ( void );

		//	validate() : Checks to make sure submitted graphics commands are valid.
		// Returns gpu::kError_SUCCESS on no error.
		GPUW_API int			validate ( void );

		//	setRasterizerState( state ) : Sets rasterizer state for the next draw
		GPUW_API int			setRasterizerState ( const RasterizerState& state );
		//	setBlendState( state ) : Sets blend state for a single render target
		// Undocumented behavior when used with MRT. For MRT, use setBlendCollectiveState.
		GPUW_API int			setBlendState ( const BlendState& state );
		//	setBlendCollectiveState( state ) : Sets blend state for all targets in an MRT.
		// Blend states for unbound MRT slots are ignored by the driver (probably)
		GPUW_API int			setBlendCollectiveState ( const BlendCollectiveState& state );
		//	setDepthStencilState( state ) : Sets depth-stencil state for the next draw
		GPUW_API int			setDepthStencilState ( const DepthStencilState& state );

		//	setViewPort( left, top, right, bottom ) : Sets viewport rect
		GPUW_API int			setViewport ( uint32_t left, uint32_t top, uint32_t right, uint32_t bottom );
		//	setScissor( left, top, right, bottom ) : Sets scissor rect
		// The GPU wrapper assumes scissor test is always on. For simple rendering, it's best to set this to same as viewport.
		GPUW_API int			setScissor ( uint32_t left, uint32_t top, uint32_t right, uint32_t bottom );

		//	setRenderTarget( renderTarget ) : Sets input render target as the current pixel shader output.
		// MRT pixel shader outputs are set through RenderTarget setup and Pipeline setup.
		GPUW_API int			setRenderTarget ( RenderTarget* renderTarget );

		//	clearPipelineAndWait( pipeline ) : Clears current pipeline, performs synchronization with last bound render target
		// This should be replaced with a waitOnRenderTarget() functionality later.
		GPUW_API int			clearPipelineAndWait ( void );

		//	setPipeline( pipeline ) : Sets current pipeline.
		// Combination set for shader pipeline, vertex attributes, and primitive topology.
		// This must be done before any resources are bound to any shader stage.
		GPUW_API int			setPipeline ( Pipeline* pipeline );
		//	setIndexBuffer( buffer, format ) : Sets index buffer for use in the next draw.
		GPUW_API int			setIndexBuffer ( Buffer* buffer, IndexFormat format );
		//	setVertexBuffer( slot, buffer, offset ) : Sets vertex buffer for use in the next draw, provided through a vertex attribute.
		GPUW_API int			setVertexBuffer ( int slot, Buffer* buffer, uint32_t offset );
		//	setShaderCBuffer( stage, slot, buffer ) : Sets buffer to given slot, as a ConstantBuffer.
		// Size is limited to 4kb on some platforms.
		GPUW_API int			setShaderCBuffer ( ShaderStage stage, int slot, Buffer* buffer );
		//	setShaderSBuffer( stage, slot, buffer ) : Sets buffer to given slot, as a StructuredBuffer.
		// Size must be at least 1kb on some platforms.
		// For compute stages, acts as a fast alias for setShaderResource.
		GPUW_API int			setShaderSBuffer ( ShaderStage stage, int slot, Buffer* buffer );
		//	setShaderSampler( stage, slot, sampler ) : Sets a sampler to a given slot.
		GPUW_API int			setShaderSampler ( ShaderStage stage, int slot, Sampler* sampler );
		//	setShaderSampler( stage, slot, sampler ) : Sets a texture to a given slot, without a sampler.
		GPUW_API int			setShaderTexture ( ShaderStage stage, int slot, Texture* texture );
		//	setShaderTextureAuto( stage, slot, texture ) : Sets texture to given slot, using an automatically generated sampler.
		GPUW_API int			setShaderTextureAuto ( ShaderStage stage, int slot, Texture* texture );
		//	setShaderResource( stage, slot, buffer ) : Sets a buffer to a given slot, as a generic data buffer.
		GPUW_API int			setShaderResource ( ShaderStage stage, int slot, Buffer* buffer );

		//	draw( vertexCount, startVertex ) : render primitives, automatically generating indices
		GPUW_API int			draw ( const uint32_t vertexCount, const uint32_t startVertex );
		//	drawIndexed( indexCount, startIndex, baseVertex ) : render indexed primitive
		GPUW_API int			drawIndexed ( const uint32_t indexCount, const uint32_t startIndex, const uint32_t baseVertex );
		//	drawIndexedInstanced( indexCount, instanceCount, startIndex, baseVertex ) : render instanced, indexed primitives
		GPUW_API int			drawIndexedInstanced ( const uint32_t indexCount, const uint32_t instanceCount, const uint32_t startIndex, const uint32_t baseVertex );
		//	setIndirectArgs( buffer ) : sets buffer used for arguments by the next drawIndirect call.
		GPUW_API int			setIndirectArgs ( Buffer* buffer );
		//	drawInstancedIndirect( offset ) : render instanced, indexed primitives from indirect data
		GPUW_API int			drawInstancedIndirect ( const uint32_t offset );

		GPUW_API int			clearDepthStencil ( bool clearDepth, float depth, bool clearStencil, uint8_t stencil );
		GPUW_API int			clearColor ( float* rgbaColor );
		// TODO: clearDepthStencilMRT and clearColorMRT

		//	signal( fence ) : Inserts a command into the command buffer to signal the given fence.
		// This can be used to make either the CPU or GPU wait on a command buffer to reach a certain point.
		GPUW_API int			signal ( Fence* fence );
		//	waitOnSignal( fence ): Inserts a command into the command buffer to stall the command stream until the given fence is signaled.
		// This can be used to wait on either a CPU or GPU signal.
		GPUW_API int			waitOnSignal ( Fence* fence );

		GPUW_API int			blit ( const BlitTarget& source, const BlitTarget& target );
		GPUW_API int			blitResolve ( const BlitTarget& source, const BlitTarget& target );

		//	debugGroupPush( groupName ) : Inserts a "group" tag into the commands, used to group calls for viewing in graphics debuggers.
		GPUW_API int			debugGroupPush ( const char* groupName );
		//	debugGroupPop() : Ends the currently set "group" in the commands.
		GPUW_API int			debugGroupPop ( void );

	private:
		// implementation details:

		void*					m_wrapperDevice;
		void*					m_deferredContext;

		//RasterizerState			m_rasterState;
		void*					m_rasterStateCachedMap;
		//void*					m_rasterStateCurrent;
		uint32_t				m_rasterStateCurrentBitfilter;

		//BlendCollectiveState	m_blendCollectState;
		void*					m_blendStateCachedMap;
		//void*					m_blendStateCurrent;
		uint64_t				m_blendStateCurrentBitfilter [4];

		void*					m_depthStateCachedMap;
		uint64_t				m_depthStateCurrentBitfilter;

		//DepthStencilState		m_depthStencilState;
		PrimitiveTopology		m_primitiveType;

		void*					m_renderTarget;
		void*					m_depthStencilTarget;

		Pipeline*				m_pipeline;
		bool					m_pipelineBound;
		bool					m_pipelineDataBound;

		IndexFormat				m_indexFormat;
		Buffer*					m_indexBuffer;
		//Buffer*					m_vertexBuffer[32/*D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT*/];
		Buffer*					m_indirectArgsBuffer;

		Sampler*				m_defaultSampler;

		int						drawPreparePipeline ( void );
	};
}

#endif//GPU_WRAPPER_GRAPHICS_CONTEXT_H_