#ifndef GPU_WRAPPER_GRAPHICS_CONTEXT_H_
#define GPU_WRAPPER_GRAPHICS_CONTEXT_H_

#include "core/types.h"
#include "renderer/gpuw/Public/Enums.h"
#include "renderer/gpuw/Public/Formats.h"
#include "renderer/gpuw/Public/ShaderTypes.h"
#include <stdint.h>

namespace gpu
{
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

	class ShaderPipeline;
	class Pipeline;
	class Fence;
	class VertexBuffer;
	class ConstantBuffer;
	class Sampler;
	class Texture;
	class Buffer;
	class RenderTarget;

	class GraphicsContext
	{
	public:
		RENDER_API int			reset ( void );

		RENDER_API int			submit ( void );

		RENDER_API int			validate ( void );

		//	setFillMode( fillMode ) : Set device's fill mode.
		// Controls how to fill polygons for given device. (glPolygonMode in OpenGL)
		// NULL device sets for current active device.
		RENDER_API int			setFillMode( const FillMode fillMode );

		RENDER_API int			setRasterizerState ( const RasterizerState& state );
		RENDER_API int			setBlendState ( const BlendState& state );
		RENDER_API int			setBlendCollectiveState ( const BlendCollectiveState& state );
		RENDER_API int			setDepthStencilState ( const DepthStencilState& state );

		RENDER_API int			setViewport ( uint32_t left, uint32_t top, uint32_t right, uint32_t bottom );
		RENDER_API int			setScissor ( uint32_t left, uint32_t top, uint32_t right, uint32_t bottom );

		RENDER_API int			setRenderTarget ( RenderTarget* renderTarget );

		//	setPipeline( pipeline ) : Sets current pipeline.
		// Combination set for shader pipeline, vertex attributes, and primitive topology.
		RENDER_API int			setPipeline ( Pipeline* pipeline );
		RENDER_API int			setIndexBuffer ( Buffer* buffer, Format format );
		RENDER_API int			setVertexBuffer ( int slot, VertexBuffer* buffer, uint32_t offset );
		RENDER_API int			setShaderCBuffer ( ShaderStage stage, int slot, ConstantBuffer* buffer );
		RENDER_API int			setShaderSampler ( ShaderStage stage, int slot, Sampler* buffer );
		RENDER_API int			setShaderSamplerAuto ( ShaderStage stage, int slot, Texture* buffer );
		RENDER_API int			setShaderResource ( ShaderStage stage, int slot, Buffer* buffer );

		//RENDER_API int			setPrimitiveTopology ( PrimitiveTopology topology ); // handled inside pipeline

		RENDER_API int			draw ( const uint32_t vertexCount, const uint32_t startVertex );
		RENDER_API int			drawIndexed ( const uint32_t indexCount, const uint32_t startIndex );
		RENDER_API int			drawIndirect ( void );

		RENDER_API int			clearDepthStencil ( bool clearDepth, float depth, bool clearStencil, uint8_t stencil );
		RENDER_API int			clearColor ( float* rgbaColor );

		RENDER_API int			sync ( Fence* fence );

	private:
		RasterizerState			m_rasterState;
		BlendCollectiveState	m_blendCollectState;
		DepthStencilState		m_depthStencilState;
		PrimitiveTopology		m_primitiveType;

		Pipeline*				m_pipeline;
		bool					m_pipelineBound;
		bool					m_pipelineDataBound;

		int						drawPreparePipeline ( void );
	};
}

#endif//GPU_WRAPPER_GRAPHICS_CONTEXT_H_