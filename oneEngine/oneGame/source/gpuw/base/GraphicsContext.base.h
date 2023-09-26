#ifndef GPU_WRAPPER_BASE_GRAPHICS_CONTEXT_H_
#define GPU_WRAPPER_BASE_GRAPHICS_CONTEXT_H_

#include "core/types.h"
#include "gpuw/Public/Enums.h"
#include "gpuw/Public/Formats.h"
#include "gpuw/Public/Rect2.h"
#include "gpuw/Public/ShaderTypes.h"
#include "gpuw/Public/Slots.h"
#include "gpuw/Public/States.h"
#include <stdint.h>

#include "./BaseContext.base.h"

namespace gpu {
namespace base
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

	class GraphicsContext : public BaseContext
	{
	public:
		GPUW_EXLUSIVE_API explicit
								GraphicsContext ( Device* device, bool isFirstOrImmediate );
		GPUW_EXLUSIVE_API 		~GraphicsContext ( void );

		// @brief Resets the context state to defaults.
		// @note On some API's, this will do nothing.
		GPUW_API virtual int	reset ( void );

		// @brief Submits the current graphics commands in the queue.
		GPUW_API virtual int	submit ( void );

		// @brief Checks to make sure submitted graphics commands are valid.
		// Returns gpu::kError_SUCCESS on no error.
		GPUW_API virtual int	validate ( void );

		// @brief Sets rasterizer state for the next draw
		//		Attempt to batch similar states to minimize pipeline state object creation and changes.
		GPUW_API virtual int	 setRasterizerState ( const RasterizerState& state );
		// @brief Sets blend state for a single render target
		// Undocumented behavior when used with MRT. For MRT, use setBlendCollectiveState.
		//		Attempt to batch similar states to minimize pipeline state object creation and changes.
		GPUW_API virtual int	 setBlendState ( const BlendState& state );
		// @brief Sets blend state for all targets in an MRT.
		//		Blend states for unbound MRT slots are ignored by the driver (probably)
		//		Attempt to batch similar states to minimize pipeline state object creation and changes.
		GPUW_API virtual int	setBlendCollectiveState ( const BlendCollectiveState& state );
		// @brief Sets depth-stencil state for the next draw
		//		Attempt to batch similar states to minimize pipeline state object creation and changes.
		GPUW_API virtual int	setDepthStencilState ( const DepthStencilState& state );

		// @brief Sets viewport rect
		GPUW_API virtual int	setViewport ( uint32_t left, uint32_t top, uint32_t right, uint32_t bottom );
		// @brief Sets scissor rect
		//		The GPU wrapper assumes scissor test is always on. For simple rendering, it's best to set this to same as viewport.
		GPUW_API virtual int	setScissor ( uint32_t left, uint32_t top, uint32_t right, uint32_t bottom );

		// @brief Sets input render target as the current pixel shader output.
		//		MRT pixel shader outputs are created through gpu::RenderTarget attachments and gpu::Pipeline setup.
		GPUW_API virtual int	setRenderTarget ( RenderTarget* renderTarget );

		// @brief Clears current pipeline, performs synchronization with last bound render target
		//		Currently, this is a no-op on certain APIs, but on others, will transition all bound resources to a read-only state.
		//		Use sparingly.
		// @todo This should be replaced with one of the following:
		//		- waitOnRenderTarget() functionality
		//		- transitionResourceUsage() explicit transition system for resources
		GPUW_API virtual int	clearPipelineAndWait ( void );

		//	setPipeline( pipeline ) : Sets current pipeline.
		// Combination set for shader pipeline, vertex attributes, and primitive topology.
		// This must be done before any resources are bound to any shader stage.
		GPUW_API virtual int	setPipeline ( const Pipeline* pipeline );
		//	setIndexBuffer( buffer, format ) : Sets index buffer for use in the next draw.
		GPUW_API virtual int	setIndexBuffer ( const Buffer* buffer, IndexFormat format );
		//	setVertexBuffer( slot, buffer, offset ) : Sets vertex buffer for use in the next draw, provided through a vertex attribute.
		GPUW_API virtual int	setVertexBuffer ( int slot, const Buffer* buffer, uint32_t offset );
		//	setShaderCBuffer( stage, slot, buffer ) : Sets buffer to given slot, as a ConstantBuffer.
		// Size is limited to 4kb on some platforms.
		GPUW_API virtual int	setShaderCBuffer ( ShaderStage stage, int slot, const Buffer* buffer );
		//	setShaderCBuffers( stage, start slot, slot count, buffers ) : Sets buffers to slots at once, as ConstantBuffers.
		// Size is limited to 4kb on some platforms.
		GPUW_API virtual int	setShaderCBuffers ( ShaderStage stage, int startSlot, int slotCount, Buffer* const* buffers );
		//	setShaderSBuffer( stage, slot, buffer ) : Sets buffer to given slot, as a StructuredBuffer.
		// Size must be at least 1kb on some platforms.
		// For compute stages, acts as a fast alias for setShaderResource.
		GPUW_API virtual int	setShaderSBuffer ( ShaderStage stage, int slot, Buffer* buffer );
		//	setShaderSampler( stage, slot, sampler ) : Sets a sampler to a given slot.
		GPUW_API virtual int	setShaderSampler ( ShaderStage stage, int slot, Sampler* sampler );
		//	setShaderTexture( stage, slot, texture ) : Sets a texture to a given slot, without a sampler.
		GPUW_API virtual int	setShaderTexture ( ShaderStage stage, int slot, Texture* texture );
		//	setShaderTexture( stage, slot, texture ) : Sets a texture to a given slot, with a sampler.
		GPUW_API virtual int	setShaderTexture ( ShaderStage stage, int slot, Texture* texture, Sampler* sampler );
		//	setShaderTextureAuto( stage, slot, texture ) : Sets texture to given slot, using an automatically generated sampler.
		GPUW_API virtual int	setShaderTextureAuto ( ShaderStage stage, int slot, Texture* texture );
		//	setShaderWriteable( stage, slot, resource ) : Sets a resource (texture or buffer) to a given slot, as a ReadWrite resource.
		GPUW_API virtual int	setShaderWriteable ( ShaderStage stage, int slot, WriteableResource* resource );

		//	draw( vertexCount, startVertex ) : render primitives, automatically generating indices
		GPUW_API virtual int	draw ( const uint32_t vertexCount, const uint32_t startVertex );
		//	drawInstanced( vertexCount, startVertex ) : render primitives, automatically generating indices
		GPUW_API virtual int	drawInstanced ( const uint32_t vertexCount, const uint32_t instanceCount, const uint32_t startVertex );
		//	drawIndexed( indexCount, startIndex, baseVertex ) : render indexed primitive
		GPUW_API virtual int	drawIndexed ( const uint32_t indexCount, const uint32_t startIndex, const uint32_t baseVertex );
		//	drawIndexedInstanced( indexCount, instanceCount, startIndex, baseVertex ) : render instanced, indexed primitives
		GPUW_API virtual int	drawIndexedInstanced ( const uint32_t indexCount, const uint32_t instanceCount, const uint32_t startIndex, const uint32_t baseVertex );
		//	setIndirectArgs( buffer ) : sets buffer used for arguments by the next drawIndirect call.
		GPUW_API virtual int	setIndirectArgs ( Buffer* buffer );
		//	drawInstancedIndirect( offset ) : render instanced, indexed primitives from indirect data
		GPUW_API virtual int	drawInstancedIndirect ( const uint32_t offset );

		GPUW_API virtual int	clearDepthStencil ( bool clearDepth, float depth, bool clearStencil, uint8_t stencil );
		GPUW_API virtual int	clearColor ( float* rgbaColor );
		GPUW_API virtual int	clearColorAll ( float* rgbaColor );
		// TODO: clearDepthStencilMRT and clearColorMRT

		//	setComputeShader( shader ) : Sets compute shader to use with next dispatch.
		GPUW_API virtual int	setComputeShader ( ShaderPipeline* computePipeline );
		//	dispatch(X, Y, Z) : dispatch a compute job with a given shader
		GPUW_API virtual int	dispatch ( const uint32 threadCountX, const uint32 threadCountY, const uint32 threadCountZ );

		//	signal( fence ) : Inserts a command into the command buffer to signal the given fence.
		// This can be used to make either the CPU or GPU wait on a command buffer to reach a certain point.
		GPUW_API virtual int	signal ( Fence* fence );
		//	waitOnSignal( fence ): Inserts a command into the command buffer to stall the command stream until the given fence is signaled.
		// This can be used to wait on either a CPU or GPU signal.
		GPUW_API virtual int	waitOnSignal ( Fence* fence );

		GPUW_API virtual int	blit ( const BlitTarget& source, const BlitTarget& target );
		GPUW_API virtual int	blitResolve ( const BlitTarget& source, const BlitTarget& target );

		//	debugGroupPush( groupName ) : Inserts a "group" tag into the commands, used to group calls for viewing in graphics debuggers.
		GPUW_API virtual int	debugGroupPush ( const char* groupName );
		//	debugGroupPop() : Ends the currently set "group" in the commands.
		GPUW_API virtual int	debugGroupPop ( void );
	};
}}

#endif//GPU_WRAPPER_BASE_GRAPHICS_CONTEXT_H_