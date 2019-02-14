#ifndef GPU_WRAPPER_GRAPHICS_CONTEXT_H_
#define GPU_WRAPPER_GRAPHICS_CONTEXT_H_

#include "core/types.h"
#include "gpuw/Public/Enums.h"
#include "gpuw/Public/Formats.h"
#include "gpuw/Public/Rect2.h"
#include "gpuw/Public/ShaderTypes.h"
#include "gpuw/Public/Slots.h"
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

	struct RasterizerState
	{
		FillMode	fillmode;
		CullMode	cullmode;
		FrontFace	frontface;
		bool		scissorEnabled;

		RasterizerState()
			: fillmode(kFillModeSolid), cullmode(kCullModeBack), frontface(kFrontFaceCounterClockwise),
			scissorEnabled(true)
		{}
	};

	struct BlendState
	{
		bool		enable;
		BlendMode	src;
		BlendMode	dst;
		BlendOp		op;
		BlendMode	srcAlpha;
		BlendMode	dstAlpha;
		BlendOp		opAlpha;
		uint8_t		channelMask;

		BlendState()
			: enable(false), 
			src(kBlendModeOne), dst(kBlendModeZero), op(kBlendOpAdd),
			srcAlpha(kBlendModeOne), dstAlpha(kBlendModeZero), opAlpha(kBlendOpAdd),
			channelMask(0xFF)
			{}
	};
	struct BlendCollectiveState
	{
		BlendState	blend [16];
	};

	struct StencilOpInfo
	{
		StencilOp		failOp;
		StencilOp		depthFailOp;
		StencilOp		passOp;
		CompareOp		func;

		StencilOpInfo()
			: failOp(kStencilOpKeep), depthFailOp(kStencilOpKeep), passOp(kStencilOpKeep),
			func(kCompareOpAlways)
			{}
	};
	struct DepthStencilState
	{
		bool			depthTestEnabled;
		bool			depthWriteEnabled;
		CompareOp		depthFunc;
		bool			stencilTestEnabled;
		uint8_t			stencilReadMask;
		uint8_t			stencilWriteMask;
		StencilOpInfo	stencilOpFrontface;
		StencilOpInfo	stencilOpBackface;

		DepthStencilState()
			: depthTestEnabled(true), depthWriteEnabled(true), depthFunc(kCompareOpLess),
			stencilTestEnabled(false), stencilReadMask(0xFF), stencilWriteMask(0xFF),
			stencilOpFrontface(), stencilOpBackface()
			{}
	};

	struct BlitTarget
	{
		RenderTarget*	renderTarget;
		RenderTargetSlot
						target;
		Rect2			rect;

		BlitTarget()
			: renderTarget(NULL), target(gpu::kRenderTargetSlotColor0), rect(0,0,0,0)
			{}
	};

	class GraphicsContext
	{
	public:
		//	reset() : Resets the context state to defaults.
		// Note that on some API's, this will do nothing.
		GPUW_API int			reset ( void );

		//	submit() : Submits the current graphics commands in the queue.
		GPUW_API int			submit ( void );

		//	validate() : Checks to make sure submitted graphics commands are valid.
		// Returns gpu::kError_SUCCESS on no error.
		GPUW_API int			validate ( void );

		//	setFillMode( fillMode ) : Set fill mode.
		// Controls how to fill polygons. (glPolygonMode in OpenGL)
		GPUW_API int			setFillMode( const FillMode fillMode );

		GPUW_API int			setRasterizerState ( const RasterizerState& state );
		//	setBlendState( state ) : Sets blend state for a single render target
		// Undocumented behavior when used with MRT. For MRT, use setBlendCollectiveState.
		GPUW_API int			setBlendState ( const BlendState& state );
		//	setBlendCollectiveState( state ) : Sets blend state for all targets in an MRT.
		// Blend states for unbound MRT slots are ignored by the driver (probably)
		GPUW_API int			setBlendCollectiveState ( const BlendCollectiveState& state );
		GPUW_API int			setDepthStencilState ( const DepthStencilState& state );

		//	setViewPort( left, top, right, bottom ) : Sets viewport rect
		GPUW_API int			setViewport ( uint32_t left, uint32_t top, uint32_t right, uint32_t bottom );
		//	setScissor( left, top, right, bottom ) : Sets scissor rect
		// The GPU wrapper assumes scissor test is always on. For simple rendering, it's best to set this to same as viewport.
		GPUW_API int			setScissor ( uint32_t left, uint32_t top, uint32_t right, uint32_t bottom );

		//	setRenderTarget( renderTarget ) : Sets input render target as the current pixel shader output.
		// MRT pixel shader outputs are set through RenderTarget setup and Pipeline setup.
		GPUW_API int			setRenderTarget ( RenderTarget* renderTarget );

		//	setPipeline( pipeline ) : Sets current pipeline.
		// Combination set for shader pipeline, vertex attributes, and primitive topology.
		GPUW_API int			setPipeline ( Pipeline* pipeline );
		GPUW_API int			setIndexBuffer ( Buffer* buffer, IndexFormat format );
		GPUW_API int			setVertexBuffer ( int slot, Buffer* buffer, uint32_t offset );
		//	setShaderCBuffer( stage, slot, buffer ) : Sets buffer to given slot, as a ConstantBuffer.
		// Size is limited to 4kb on some platforms.
		GPUW_API int			setShaderCBuffer ( ShaderStage stage, int slot, Buffer* buffer );
		//	setShaderSBuffer( stage, slot, buffer ) : Sets buffer to given slot, as a StructuredBuffer.
		// Size must be at least 1kb on some platforms.
		// For compute stages, acts as a fast alias for setShaderResource.
		GPUW_API int			setShaderSBuffer ( ShaderStage stage, int slot, Buffer* buffer );
		GPUW_API int			setShaderSampler ( ShaderStage stage, int slot, Sampler* buffer );
		//	setShaderSamplerAuto( stage, slot, texture ) : Sets texture to given slot using an automatically generated sampler.
		GPUW_API int			setShaderSamplerAuto ( ShaderStage stage, int slot, Texture* buffer );
		GPUW_API int			setShaderResource ( ShaderStage stage, int slot, Buffer* buffer );

		GPUW_API int			draw ( const uint32_t vertexCount, const uint32_t startVertex );
		GPUW_API int			drawIndexed ( const uint32_t indexCount, const uint32_t startIndex );
		GPUW_API int			drawIndexedInstanced ( const uint32_t indexCount, const uint32_t instanceCount, const uint32_t startIndex );
		GPUW_API int			drawIndirect ( void );

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

	private:
		// implementation details:

		RasterizerState			m_rasterState;
		BlendCollectiveState	m_blendCollectState;
		DepthStencilState		m_depthStencilState;
		PrimitiveTopology		m_primitiveType;

		Pipeline*				m_pipeline;
		bool					m_pipelineBound;
		bool					m_pipelineDataBound;

		IndexFormat				m_indexFormat;
		Buffer*					m_indexBuffer;

		int						drawPreparePipeline ( void );
	};
}

#endif//GPU_WRAPPER_GRAPHICS_CONTEXT_H_