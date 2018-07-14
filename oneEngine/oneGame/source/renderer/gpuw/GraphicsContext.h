#ifndef GPU_WRAPPER_GRAPHICS_CONTEXT_H_
#define GPU_WRAPPER_GRAPHICS_CONTEXT_H_

#include "core/types.h"
#include <stdint.h>

namespace gpu
{
	enum FillMode
	{
		kFillModeSolid,
		kFillModeWireframe,
	};
	enum CullMode
	{
		kCullModeBack,
		kCullModeFront,
		kCullModeNone,
	};
	enum FrontFace
	{
		kFrontFaceCounterClockwise,
		kFrontFaceClockwise,
	};

	struct RasterizerState
	{
		FillMode	fillmode;
		CullMode	cullmode;
		FrontFace	frontface;
		bool		scissorEnabled;

		RasterizerState()
			: fillmode(kFillModeSolid), cullmode(kCullModeBack), frontface(kFrontFaceClockwise),
			scissorEnabled(true)
		{}
	};


	enum BlendMode
	{
		kBlendModeZero,
		kBlendModeOne,
		kBlendModeSrcColor,
		kBlendModeInvSrcColor,
		kBlendModeSrcAlpha,
		kBlendModeInvSrcAlpha,
		kBlendModeDstColor,
		kBlendModeInvDstColor,
		kBlendModeDstAlpha,
		kBlendModeInvDstAlpha,
		kBlendModeSrcAlphaSat,
		kBlendModeBlendFactor,
		kBlendModeInvBlendFactor,
		kBlendModeSrc1Color,
		kBlendModeInvSrc1Color,
		kBlendModeSrc1Alpha,
		kBlendModeInvSrc1Alpha,
	};
	enum BlendOp
	{
		kBlendOpAdd,
		kBlendOpSubtract,
		kBlendOpRevSubtract,
		kBlendOpMin,
		kBlendOpMax,
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

	enum CompareOp
	{
		kCompareOpNever,
		kCompareOpLess,
		kCompareOpEqual,
		kCompareOpLessEqual,
		kCompareOpGreater,
		kCompareOpNotEqual,
		kCompareOpGreaterEqual,
		kCompareOpAlways
	};
	enum StencilOp
	{
		kStencilOpKeep,
		kStencilOpZero,
		kStencilOpReplace,
		kStencilOpIncrementSaturate,
		kStencilOpDecrementSaturate,
		kStencilOpInvert,
		kStencilOpIncrement,
		kStencilOpDecrement,
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

	class Fence;
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

		RENDER_API int			sync ( Fence* fence );

	private:
		RasterizerState		m_rasterState;
	};
}

#endif//GPU_WRAPPER_GRAPHICS_CONTEXT_H_