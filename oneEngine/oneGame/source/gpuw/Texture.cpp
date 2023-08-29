#include "gpuw/Texture.h"
#include "gpuw/dx11/Texture.dx11.h"
#include "gpuw/Public/Error.h"

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
	}
}


bool gpu::TextureDynamic::valid ( void ) const
{
	return pInternal ? pInternal->valid() : false;
}
		
gpuHandle gpu::TextureDynamic::nativePtr ( void )
{
	return pInternal ? pInternal->nativePtr() : gpuHandle();
}

int gpu::TextureDynamic::allocate (
	const core::gfx::tex::arTextureType textureType,
	const core::gfx::tex::arColorFormat textureFormat, 
	const uint width = 0, const uint height = 0, const uint depth = 0, const uint levels = 0
)
{
	allocateIfNeeded(pInternal);
	return pInternal->allocate(textureType, textureFormat, width, height, depth, levels);
}

int gpu::TextureDynamic::free ( void )
{
	int result = gpu::kErrorNullReference;
	if (pInternal)
		result = pInternal->free();
	delete pInternal;
	return result;
}

int gpu::TextureDynamic::upload ( gpu::base::BaseContext* context, gpu::base::Buffer& buffer, const uint level, const uint arraySlice )
{
	ARCORE_ASSERT(pInternal);
	if (pInternal)
	{
		return pInternal->upload(context, buffer, level, arraySlice);
	}
	return gpu::kErrorNullReference;
}

int gpu::TextureDynamic::copy ( gpu::base::BaseContext* context, gpu::base::Buffer& buffer, const uint level, const uint arraySlice )
{
	ARCORE_ASSERT(pInternal);
	if (pInternal)
	{
		return pInternal->copy(context, buffer, level, arraySlice);
	}
	return gpu::kErrorNullReference;
}