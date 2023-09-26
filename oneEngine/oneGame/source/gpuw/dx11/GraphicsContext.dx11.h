#ifndef GPU_WRAPPER_GRAPHICS_CONTEXT_DX11_H_
#define GPU_WRAPPER_GRAPHICS_CONTEXT_DX11_H_

#include "core/types.h"
#include "gpuw/Public/Enums.h"
#include "gpuw/Public/Formats.h"
#include "gpuw/Public/Rect2.h"
#include "gpuw/Public/ShaderTypes.h"
#include "gpuw/Public/Slots.h"
#include "gpuw/Public/States.h"
#include <stdint.h>

#include "gpuw/base/GraphicsContext.base.h"
#include "gpuw/base/BaseContext.base.h"

namespace gpu {
namespace dx11
{
	class ShaderPipeline;
	class Pipeline;
	class Fence;
	class Sampler;
	class Texture;
	class Buffer;
	class WriteableResource;
	class RenderTarget;
	class Device;

	class GraphicsContext : public gpu::base::GraphicsContext
	{
	public:
		GPUW_EXLUSIVE_API explicit
								GraphicsContext ( gpu::base::Device* wrapperDevice, bool passthroughAsImmediate );
		GPUW_EXLUSIVE_API 		~GraphicsContext ( void );

		//	reset() : Resets the context state to defaults.
		// Note that on some API's, this will do nothing.
		GPUW_API int			reset ( void ) override;

		//	submit() : Submits the current graphics commands in the queue.
		GPUW_API int			submit ( void ) override;

		//	validate() : Checks to make sure submitted graphics commands are valid.
		// Returns gpu::kError_SUCCESS on no error.
		GPUW_API int			validate ( void ) override;

		//	setRasterizerState( state ) : Sets rasterizer state for the next draw
		GPUW_API int			setRasterizerState ( const RasterizerState& state ) override;
		//	setBlendState( state ) : Sets blend state for a single render target
		// Undocumented behavior when used with MRT. For MRT, use setBlendCollectiveState.
		GPUW_API int			setBlendState ( const BlendState& state ) override;
		//	setBlendCollectiveState( state ) : Sets blend state for all targets in an MRT.
		// Blend states for unbound MRT slots are ignored by the driver (probably)
		GPUW_API int			setBlendCollectiveState ( const BlendCollectiveState& state ) override;
		//	setDepthStencilState( state ) : Sets depth-stencil state for the next draw
		GPUW_API int			setDepthStencilState ( const DepthStencilState& state ) override;

		//	setViewPort( left, top, right, bottom ) : Sets viewport rect
		GPUW_API int			setViewport ( uint32_t left, uint32_t top, uint32_t right, uint32_t bottom ) override;
		//	setScissor( left, top, right, bottom ) : Sets scissor rect
		// The GPU wrapper assumes scissor test is always on. For simple rendering, it's best to set this to same as viewport.
		GPUW_API int			setScissor ( uint32_t left, uint32_t top, uint32_t right, uint32_t bottom ) override;

		//	setRenderTarget( renderTarget ) : Sets input render target as the current pixel shader output.
		// MRT pixel shader outputs are set through RenderTarget setup and Pipeline setup.
		GPUW_API int			setRenderTarget ( gpu::base::RenderTarget* renderTarget ) override;

		//	clearPipelineAndWait( pipeline ) : Clears current pipeline, performs synchronization with last bound render target
		// This should be replaced with a waitOnRenderTarget() functionality later.
		GPUW_API int			clearPipelineAndWait ( void ) override;

		//	setPipeline( pipeline ) : Sets current pipeline.
		// Combination set for shader pipeline, vertex attributes, and primitive topology.
		// This must be done before any resources are bound to any shader stage.
		GPUW_API int			setPipeline ( const gpu::base::Pipeline* pipeline ) override;
		//	setIndexBuffer( buffer, format ) : Sets index buffer for use in the next draw.
		GPUW_API int			setIndexBuffer ( const gpu::base::Buffer* buffer, IndexFormat format ) override;
		//	setVertexBuffer( slot, buffer, offset ) : Sets vertex buffer for use in the next draw, provided through a vertex attribute.
		GPUW_API int			setVertexBuffer ( int slot, const gpu::base::Buffer* buffer, uint32_t offset ) override;
		//	setShaderCBuffer( stage, slot, buffer ) : Sets buffer to given slot, as a ConstantBuffer.
		// Size is limited to 4kb on some platforms.
		GPUW_API int			setShaderCBuffer ( ShaderStage stage, int slot, const gpu::base::Buffer* buffer ) override;
		//	setShaderCBuffers( stage, start slot, slot count, buffers ) : Sets buffers to slots at once, as ConstantBuffers.
		// Size is limited to 4kb on some platforms.
		GPUW_API int			setShaderCBuffers ( ShaderStage stage, int startSlot, int slotCount, gpu::base::Buffer* const* buffers ) override;
		//	setShaderSBuffer( stage, slot, buffer ) : Sets buffer to given slot, as a StructuredBuffer.
		// Size must be at least 1kb on some platforms.
		// For compute stages, acts as a fast alias for setShaderResource.
		GPUW_API int			setShaderSBuffer ( ShaderStage stage, int slot, gpu::base::Buffer* buffer ) override;
		//	setShaderSampler( stage, slot, sampler ) : Sets a sampler to a given slot.
		GPUW_API int			setShaderSampler ( ShaderStage stage, int slot, gpu::base::Sampler* sampler ) override;
		//	setShaderTexture( stage, slot, texture ) : Sets a texture to a given slot, without a sampler.
		GPUW_API int			setShaderTexture ( ShaderStage stage, int slot, gpu::base::Texture* texture ) override;
		//	setShaderTexture( stage, slot, texture ) : Sets a texture to a given slot, with a sampler.
		GPUW_API int			setShaderTexture ( ShaderStage stage, int slot, gpu::base::Texture* texture, gpu::base::Sampler* sampler ) override;
		//	setShaderTextureAuto( stage, slot, texture ) : Sets texture to given slot, using an automatically generated sampler.
		GPUW_API int			setShaderTextureAuto ( ShaderStage stage, int slot, gpu::base::Texture* texture ) override;
		//	setShaderWriteable( stage, slot, resource ) : Sets a resource (texture or buffer) to a given slot, as a ReadWrite resource.
		GPUW_API int			setShaderWriteable ( ShaderStage stage, int slot, gpu::base::WriteableResource* resource ) override;

		//	draw( vertexCount, startVertex ) : render primitives, automatically generating indices
		GPUW_API int			draw ( const uint32_t vertexCount, const uint32_t startVertex ) override;
		//	drawInstanced( vertexCount, startVertex ) : render primitives, automatically generating indices
		GPUW_API int			drawInstanced ( const uint32_t vertexCount, const uint32_t instanceCount, const uint32_t startVertex ) override;
		//	drawIndexed( indexCount, startIndex, baseVertex ) : render indexed primitive
		GPUW_API int			drawIndexed ( const uint32_t indexCount, const uint32_t startIndex, const uint32_t baseVertex ) override;
		//	drawIndexedInstanced( indexCount, instanceCount, startIndex, baseVertex ) : render instanced, indexed primitives
		GPUW_API int			drawIndexedInstanced ( const uint32_t indexCount, const uint32_t instanceCount, const uint32_t startIndex, const uint32_t baseVertex ) override;
		//	setIndirectArgs( buffer ) : sets buffer used for arguments by the next drawIndirect call.
		GPUW_API int			setIndirectArgs ( gpu::base::Buffer* buffer ) override;
		//	drawInstancedIndirect( offset ) : render instanced, indexed primitives from indirect data
		GPUW_API int			drawInstancedIndirect ( const uint32_t offset ) override;

		GPUW_API int			clearDepthStencil ( bool clearDepth, float depth, bool clearStencil, uint8_t stencil ) override;
		GPUW_API int			clearColor ( float* rgbaColor ) override;
		GPUW_API int			clearColorAll ( float* rgbaColor ) override;
		// TODO: clearDepthStencilMRT and clearColorMRT

		//	setComputeShader( shader ) : Sets compute shader to use with next dispatch.
		GPUW_API int			setComputeShader ( gpu::base::ShaderPipeline* computePipeline ) override;
		//	dispatch(X, Y, Z) : dispatch a compute job with a given shader
		GPUW_API int			dispatch ( const uint32 threadCountX, const uint32 threadCountY, const uint32 threadCountZ ) override;

		//	signal( fence ) : Inserts a command into the command buffer to signal the given fence.
		// This can be used to make either the CPU or GPU wait on a command buffer to reach a certain point.
		GPUW_API int			signal ( gpu::base::Fence* fence ) override;
		//	waitOnSignal( fence ): Inserts a command into the command buffer to stall the command stream until the given fence is signaled.
		// This can be used to wait on either a CPU or GPU signal.
		GPUW_API int			waitOnSignal ( gpu::base::Fence* fence ) override;

		GPUW_API int			blit ( const BlitTarget& source, const BlitTarget& target ) override;
		GPUW_API int			blitResolve ( const BlitTarget& source, const BlitTarget& target ) override;

		//	debugGroupPush( groupName ) : Inserts a "group" tag into the commands, used to group calls for viewing in graphics debuggers.
		GPUW_API int			debugGroupPush ( const char* groupName ) override;
		//	debugGroupPop() : Ends the currently set "group" in the commands.
		GPUW_API int			debugGroupPop ( void ) override;

	public:
		GPUW_EXLUSIVE_API void*	getNativeContext ( void )
		{
			return m_deferredContext;
		}

	protected:
		void*					m_deferredContext = nullptr;

	private:
		// implementation details:

		Device*					m_wrapperDevice;
		//void*					m_deferredContext;
		bool					m_isImmediateMode;

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

		void*					m_renderTarget [16];
		void*					m_depthStencilTarget;

		Pipeline*				m_pipeline;
		bool					m_pipelineBound;
		bool					m_pipelineDataBound;

		IndexFormat				m_indexFormat;
		Buffer*					m_indexBuffer;
		//Buffer*					m_vertexBuffer[32/*D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT*/];
		Buffer*					m_indirectArgsBuffer;

		struct ConstantBufferGroup
		{
			void*				m_buffers [16];
		};
		ConstantBufferGroup		m_constantBuffers [kShaderStageMAX];

		Sampler*				m_defaultSampler;

		int						drawPreparePipeline ( void );
	};
}}

#endif//GPU_WRAPPER_GRAPHICS_CONTEXT_DX11_H_