//===============================================================================================//
//
//		1docs Style Instruction
//
// A quick references on coding and documentatation style for consistency.
//
//===============================================================================================//
#ifndef RENDERER_RENDER_MODES_H_
#define RENDERER_RENDER_MODES_H_

namespace renderer
{
	enum rrPrimitiveMode
	{
		kPrimitiveModeTriangleList,
		kPrimitiveModeTriangleStrip,
		kPrimitiveModeTriangleFan,
		kPrimitiveModeLineList,
		kPrimitiveModeLineStrip,
		kPrimitiveModePointList,

		kPrimitiveModeTriangleList_Indexed,
		kPrimitiveModeLineList_Indexed,
	};
}

#endif//RENDERER_RENDER_MODES_H_