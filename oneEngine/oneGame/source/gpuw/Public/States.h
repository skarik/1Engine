#ifndef GPU_WRAPPER_GRAPHICS_STATES_H_
#define GPU_WRAPPER_GRAPHICS_STATES_H_

#include "core/types/types.h"
#include "gpuw/Public/Enums.h"
#include "renderer/types/types.h"

namespace gpu
{
	class RenderTarget;

	struct RasterizerState
	{
		FillMode	fillmode;	// how each polygon is filled
		CullMode	cullmode;	// which face is culled
		FrontFace	frontface;	// controls winding order
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
}

#endif//GPU_WRAPPER_GRAPHICS_STATES_H_