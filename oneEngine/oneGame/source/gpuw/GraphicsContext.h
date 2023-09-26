#ifndef GPU_WRAPPER_DYNAMIC_GRAPHICS_CONTEXT_H
#define GPU_WRAPPER_DYNAMIC_GRAPHICS_CONTEXT_H

#include "gpuw/gpuw_common.h"
#include "gpuw/base/GraphicsContext.base.h"

namespace gpu
{
	class GraphicsContextDynamic : public gpu::base::GraphicsContext
	{
	public:
		GPUW_EXLUSIVE_API explicit
								GraphicsContextDynamic ( gpu::base::Device* device, bool isFirstOrImmediate );
		GPUW_EXLUSIVE_API 		~GraphicsContextDynamic ( void );

		// @brief Resets the context state to defaults.
		// @note On some API's, this will do nothing.
		GPUW_API virtual int	reset ( void ) override
		{
			return pInternal->reset();
		}

		// @brief Submits the current graphics commands in the queue.
		GPUW_API virtual int	submit ( void ) override
		{
			return pInternal->submit();
		}

		// @brief Checks to make sure submitted graphics commands are valid.
		// Returns gpu::kError_SUCCESS on no error.
		GPUW_API virtual int	validate ( void ) override
		{
			return pInternal->validate();
		}

		// @brief Sets rasterizer state for the next draw
		//		Attempt to batch similar states to minimize pipeline state object creation and changes.
		GPUW_API virtual int	 setRasterizerState ( const RasterizerState& state ) override
		{
			return pInternal->setRasterizerState(state);
		}
		// @brief Sets blend state for a single render target
		// Undocumented behavior when used with MRT. For MRT, use setBlendCollectiveState.
		//		Attempt to batch similar states to minimize pipeline state object creation and changes.
		GPUW_API virtual int	 setBlendState ( const BlendState& state ) override
		{
			return pInternal->setBlendState(state);
		}
		// @brief Sets blend state for all targets in an MRT.
		//		Blend states for unbound MRT slots are ignored by the driver (probably)
		//		Attempt to batch similar states to minimize pipeline state object creation and changes.
		GPUW_API virtual int	setBlendCollectiveState ( const BlendCollectiveState& state ) override
		{
			return pInternal->setBlendCollectiveState(state);
		}
		// @brief Sets depth-stencil state for the next draw
		//		Attempt to batch similar states to minimize pipeline state object creation and changes.
		GPUW_API virtual int	setDepthStencilState ( const DepthStencilState& state ) override
		{
			return pInternal->setDepthStencilState(state);
		}

		// @brief Sets viewport rect
		GPUW_API virtual int	setViewport ( uint32_t left, uint32_t top, uint32_t right, uint32_t bottom ) override
		{
			return pInternal->setViewport(left, top, right, bottom);
		}
		// @brief Sets scissor rect
		//		The GPU wrapper assumes scissor test is always on. For simple rendering, it's best to set this to same as viewport.
		GPUW_API virtual int	setScissor ( uint32_t left, uint32_t top, uint32_t right, uint32_t bottom ) override
		{
			return pInternal->setScissor(left, top, right, bottom);
		}

		// @brief Sets input render target as the current pixel shader output.
		//		MRT pixel shader outputs are created through gpu::RenderTarget attachments and gpu::Pipeline setup.
		GPUW_API virtual int	setRenderTarget ( gpu::base::RenderTarget* renderTarget ) override
		{
			return pInternal->setRenderTarget(renderTarget);
		}

		// @brief Clears current pipeline, performs synchronization with last bound render target
		//		Currently, this is a no-op on certain APIs, but on others, will transition all bound resources to a read-only state.
		//		Use sparingly.
		// @todo This should be replaced with one of the following:
		//		- waitOnRenderTarget() functionality
		//		- transitionResourceUsage() explicit transition system for resources
		GPUW_API virtual int	clearPipelineAndWait ( void ) override
		{
			return pInternal->clearPipelineAndWait();
		}

		//	setPipeline( pipeline ) : Sets current pipeline.
		// Combination set for shader pipeline, vertex attributes, and primitive topology.
		// This must be done before any resources are bound to any shader stage.
		GPUW_API virtual int	setPipeline ( const gpu::base::Pipeline* pipeline ) override
		{
			return pInternal->setPipeline(pipeline);
		}
		//	setIndexBuffer( buffer, format ) : Sets index buffer for use in the next draw.
		GPUW_API virtual int	setIndexBuffer ( const gpu::base::Buffer* buffer, IndexFormat format ) override
		{
			return pInternal->setIndexBuffer(buffer, format);
		}
		//	setVertexBuffer( slot, buffer, offset ) : Sets vertex buffer for use in the next draw, provided through a vertex attribute.
		GPUW_API virtual int	setVertexBuffer ( int slot, const gpu::base::Buffer* buffer, uint32_t offset ) override
		{
			return pInternal->setVertexBuffer(slot, buffer, offset);
		}
		//	setShaderCBuffer( stage, slot, buffer ) : Sets buffer to given slot, as a ConstantBuffer.
		// Size is limited to 4kb on some platforms.
		GPUW_API virtual int	setShaderCBuffer ( ShaderStage stage, int slot, const gpu::base::Buffer* buffer ) override
		{
			return pInternal->setShaderCBuffer(stage, slot, buffer);
		}
		//	setShaderCBuffers( stage, start slot, slot count, buffers ) : Sets buffers to slots at once, as ConstantBuffers.
		// Size is limited to 4kb on some platforms.
		GPUW_API virtual int	setShaderCBuffers ( ShaderStage stage, int startSlot, int slotCount, gpu::base::Buffer* const* buffers ) override
		{
			return pInternal->setShaderCBuffers(stage, startSlot, slotCount, buffers);
		}
		//	setShaderSBuffer( stage, slot, buffer ) : Sets buffer to given slot, as a StructuredBuffer.
		// Size must be at least 1kb on some platforms.
		// For compute stages, acts as a fast alias for setShaderResource.
		GPUW_API virtual int	setShaderSBuffer ( ShaderStage stage, int slot, gpu::base::Buffer* buffer ) override
		{
			return pInternal->setShaderSBuffer(stage, slot, buffer);
		}
		//	setShaderSampler( stage, slot, sampler ) : Sets a sampler to a given slot.
		GPUW_API virtual int	setShaderSampler ( ShaderStage stage, int slot, gpu::base::Sampler* sampler ) override
		{
			return pInternal->setShaderSampler(stage, slot, sampler);
		}
		//	setShaderTexture( stage, slot, texture ) : Sets a texture to a given slot, without a sampler.
		GPUW_API virtual int	setShaderTexture ( ShaderStage stage, int slot, gpu::base::Texture* texture ) override
		{
			return pInternal->setShaderTexture(stage, slot, texture);
		}
		//	setShaderTexture( stage, slot, texture ) : Sets a texture to a given slot, with a sampler.
		GPUW_API virtual int	setShaderTexture ( ShaderStage stage, int slot, gpu::base::Texture* texture, gpu::base::Sampler* sampler ) override
		{
			return pInternal->setShaderTexture(stage, slot, texture);
		}
		//	setShaderTextureAuto( stage, slot, texture ) : Sets texture to given slot, using an automatically generated sampler.
		GPUW_API virtual int	setShaderTextureAuto ( ShaderStage stage, int slot, gpu::base::Texture* texture ) override
		{
			return pInternal->setShaderTextureAuto(stage, slot, texture);
		}
		//	setShaderWriteable( stage, slot, resource ) : Sets a resource (texture or buffer) to a given slot, as a ReadWrite resource.
		GPUW_API virtual int	setShaderWriteable ( ShaderStage stage, int slot, gpu::base::WriteableResource* resource ) override
		{
			return pInternal->setShaderWriteable(stage, slot, resource);
		}

		//	draw( vertexCount, startVertex ) : render primitives, automatically generating indices
		GPUW_API virtual int	draw ( const uint32_t vertexCount, const uint32_t startVertex ) override
		{
			return pInternal->draw(vertexCount, startVertex);
		}
		//	drawInstanced( vertexCount, startVertex ) : render primitives, automatically generating indices
		GPUW_API virtual int	drawInstanced ( const uint32_t vertexCount, const uint32_t instanceCount, const uint32_t startVertex ) override
		{
			return pInternal->drawInstanced(vertexCount, instanceCount, startVertex);
		}
		//	drawIndexed( indexCount, startIndex, baseVertex ) : render indexed primitive
		GPUW_API virtual int	drawIndexed ( const uint32_t indexCount, const uint32_t startIndex, const uint32_t baseVertex ) override
		{
			return pInternal->drawIndexed(indexCount, startIndex, baseVertex);
		}
		//	drawIndexedInstanced( indexCount, instanceCount, startIndex, baseVertex ) : render instanced, indexed primitives
		GPUW_API virtual int	drawIndexedInstanced ( const uint32_t indexCount, const uint32_t instanceCount, const uint32_t startIndex, const uint32_t baseVertex ) override
		{
			return pInternal->drawIndexedInstanced(indexCount, instanceCount, startIndex, baseVertex);
		}
		//	setIndirectArgs( buffer ) : sets buffer used for arguments by the next drawIndirect call.
		GPUW_API virtual int	setIndirectArgs ( gpu::base::Buffer* buffer ) override
		{
			return pInternal->setIndirectArgs(buffer);
		}
		//	drawInstancedIndirect( offset ) : render instanced, indexed primitives from indirect data
		GPUW_API virtual int	drawInstancedIndirect ( const uint32_t offset ) override
		{
			return pInternal->drawInstancedIndirect(offset);
		}

		GPUW_API virtual int	clearDepthStencil ( bool clearDepth, float depth, bool clearStencil, uint8_t stencil ) override
		{
			return pInternal->clearDepthStencil(clearDepth, depth, clearStencil, stencil);
		}
		GPUW_API virtual int	clearColor ( float* rgbaColor ) override
		{
			return pInternal->clearColor(rgbaColor);
		}
		GPUW_API virtual int	clearColorAll ( float* rgbaColor ) override
		{
			return pInternal->clearColorAll(rgbaColor);
		}
		// TODO: clearDepthStencilMRT and clearColorMRT

		//	setComputeShader( shader ) : Sets compute shader to use with next dispatch.
		GPUW_API virtual int	setComputeShader ( gpu::base::ShaderPipeline* computePipeline ) override
		{
			return pInternal->setComputeShader(computePipeline);
		}
		//	dispatch(X, Y, Z) : dispatch a compute job with a given shader
		GPUW_API virtual int	dispatch ( const uint32 threadCountX, const uint32 threadCountY, const uint32 threadCountZ ) override
		{
			return pInternal->dispatch(threadCountX, threadCountY, threadCountZ);
		}

		//	signal( fence ) : Inserts a command into the command buffer to signal the given fence.
		// This can be used to make either the CPU or GPU wait on a command buffer to reach a certain point.
		GPUW_API virtual int	signal ( gpu::base::Fence* fence ) override
		{
			return pInternal->signal(fence);
		}
		//	waitOnSignal( fence ): Inserts a command into the command buffer to stall the command stream until the given fence is signaled.
		// This can be used to wait on either a CPU or GPU signal.
		GPUW_API virtual int	waitOnSignal ( gpu::base::Fence* fence ) override
		{
			return pInternal->waitOnSignal(fence);
		}

		GPUW_API virtual int	blit ( const BlitTarget& source, const BlitTarget& target ) override
		{
			return pInternal->blit(source, target);
		}
		GPUW_API virtual int	blitResolve ( const BlitTarget& source, const BlitTarget& target ) override
		{
			return pInternal->blitResolve(source, target);
		}

		//	debugGroupPush( groupName ) : Inserts a "group" tag into the commands, used to group calls for viewing in graphics debuggers.
		GPUW_API virtual int	debugGroupPush ( const char* groupName ) override
		{
			return pInternal->debugGroupPush(groupName);
		}
		//	debugGroupPop() : Ends the currently set "group" in the commands.
		GPUW_API virtual int	debugGroupPop ( void ) override
		{
			return pInternal->debugGroupPop();
		}
	private:
		gpu::base::GraphicsContext*
							pInternal = nullptr;
	};

	typedef GraphicsContextDynamic GraphicsContext;
}

#endif//GPU_WRAPPER_DYNAMIC_GRAPHICS_CONTEXT_H