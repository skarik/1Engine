#ifndef GPU_WRAPPER_PUBLIC_SLOT_TYPES_H_
#define GPU_WRAPPER_PUBLIC_SLOT_TYPES_H_

namespace gpu
{
	enum RenderTargetSlot
	{
		kRenderTargetSlotColor0 = 0,
		kRenderTargetSlotDepth = -1,
		kRenderTargetSlotDepthStencil = -1,
		kRenderTargetSlotStencil = -2,
	};
}

#endif//GPU_WRAPPER_PUBLIC_SLOT_TYPES_H_