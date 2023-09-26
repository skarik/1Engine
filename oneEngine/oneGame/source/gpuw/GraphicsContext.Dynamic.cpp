#include "gpuw/GraphicsContext.h"
#include "gpuw/dx11/GraphicsContext.dx11.h"
#include "gpuw/Public/Error.h"

gpu::GraphicsContextDynamic::GraphicsContextDynamic ( gpu::base::Device* device, bool isFirstOrImmediate )
	: gpu::base::GraphicsContext(device, isFirstOrImmediate)
{
	switch (gpu::GetApi())
	{
#if GPU_API_DIRECTX11
	case gpu::kApiDirectX11:
		pInternal = new gpu::dx11::GraphicsContext(device, isFirstOrImmediate);
		break;
#endif
	}
	ARCORE_ASSERT(pInternal);
}
gpu::GraphicsContextDynamic::~GraphicsContextDynamic ( void )
{
	delete_safe(pInternal);
}