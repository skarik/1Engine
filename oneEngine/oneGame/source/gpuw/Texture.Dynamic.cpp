#include "gpuw/Texture.h"
#include "gpuw/dx11/Texture.dx11.h"
#include "gpuw/Public/Error.h"

// @brief Allocates the given pointer if needed.
static void allocateIfNeeded ( gpu::base::Texture*& pointer )
{
	if (pointer == nullptr)
	{
		switch (gpu::GetApi())
		{
#if GPU_API_DIRECTX11
		case gpu::kApiDirectX11:
			pointer = new gpu::dx11::Texture();
			break;
#endif
		}
		ARCORE_ASSERT(pointer);
	}
}


int gpu::TextureDynamic::allocate (
	const core::gfx::tex::arTextureType textureType,
	const core::gfx::tex::arColorFormat textureFormat, 
	const uint width = 0, const uint height = 0, const uint depth = 0, const uint levels = 0
)
{
	allocateIfNeeded(pInternal);
	if (pInternal)
	{
		return pInternal->allocate(textureType, textureFormat, width, height, depth, levels);
	}
	return gpu::kErrorNullReference;
}

int gpu::TextureDynamic::free ( void )
{
	int result = gpu::kErrorNullReference;
	if (pInternal)
		result = pInternal->free();
	delete_safe(pInternal);
	return result;
}