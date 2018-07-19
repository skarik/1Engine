#ifndef GPU_WRAPPER_GRAPHICS_CONTEXT_H_
#define GPU_WRAPPER_GRAPHICS_CONTEXT_H_

#include "core/types.h"
#include <stdint.h>
#include "renderer/gpuw/Public/Enums.h"

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

	class GraphicsContext
	{
	public:
		RENDER_API int			reset ( void );

		RENDER_API int			submit ( void );

		//	DeviceSetFillMode( device, fillMode ) : Set device's fill mode.
		// Controls how to fill polygons for given device. (glPolygonMode in OpenGL)
		// NULL device sets for current active device.
		RENDER_API int			setFillMode( const FillMode fillMode );

		RENDER_API int			setRasterizerState ( const RasterizerState& state );
		RENDER_API int			setBlendState ( const BlendState& state );
		RENDER_API int			setBlendCollectiveState ( const BlendCollectiveState& state );
		RENDER_API int			setDepthStencilState ( const DepthStencilState& state );

		RENDER_API int			setViewport ( uint32_t left, uint32_t top, uint32_t right, uint32_t bottom );
		RENDER_API int			setScissor ( uint32_t left, uint32_t top, uint32_t right, uint32_t bottom );

		RENDER_API int			setPipeline ( Pipeline* pipeline );
		RENDER_API int			setVertexBuffer ( VertexBuffer* buffer );

		RENDER_API int			draw ( const uint32_t vertexCount, const uint32_t startVertex );
		RENDER_API int			drawIndexed ( const uint32_t indexCount, const uint32_t startIndex );
		RENDER_API int			drawIndirect ( void );

		RENDER_API int			sync ( Fence* fence );

	private:
		RasterizerState			m_rasterState;
		BlendCollectiveState	m_blendCollectState;
		DepthStencilState		m_depthStencilState;

		Pipeline*				m_pipeline;
		bool					m_pipelineBound;
		bool					m_pipelineDataBound;

		int						drawPreparePipeline ( void );
	};
}

#endif//GPU_WRAPPER_GRAPHICS_CONTEXT_H_