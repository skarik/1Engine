#include "gpuw/Device.h"

#include "gpuw/dx11/Device.dx11.h"

gpu::base::Device* gpu::createDynamicDevice ( void )
{
	switch (gpu::GetApi())
	{
#if GPU_API_DIRECTX11
		case gpu::kApiDirectX11:
			return new gpu::dx11::Device();
#endif
	}
	return nullptr;
}
