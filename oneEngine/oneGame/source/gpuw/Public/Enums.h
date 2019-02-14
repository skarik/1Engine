#ifndef GPU_WRAPPER_PUBLIC_ENUMS_H_
#define GPU_WRAPPER_PUBLIC_ENUMS_H_

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

	enum PrimitiveTopology
	{
		kPrimitiveTopologyPointList,
		kPrimitiveTopologyLineList,
		kPrimitiveTopologyLineStrip,
		kPrimitiveTopologyTriangleList,
		kPrimitiveTopologyTriangleStrip,
		kPrimitiveTopologyTriangleFan,
		kPrimitiveTopologyLineListAdjacency,
		kPrimitiveTopologyLineStripAdjacency,
		kPrimitiveTopologyTriangleListAdjacency,
		kPrimitiveTopologyTriangleStripAdjacency,
		kPrimitiveTopologyLinePatchList,
	};
}

#endif//GPU_WRAPPER_PUBLIC_ENUMS_H_