#include "gpuw/Buffers.h"
#include "gpuw/dx11/Buffers.dx11.h"
#include "gpuw/Public/Error.h"

// @brief Allocates the given pointer if needed.
static void allocateIfNeeded ( gpu::base::Buffer*& pointer )
{
	if (pointer == nullptr)
	{
		switch (gpu::GetApi())
		{
#if GPU_API_DIRECTX11
		case gpu::kApiDirectX11:
			pointer = new gpu::dx11::Buffer();
			break;
#endif
		}
		ARCORE_ASSERT(pointer);
	}
}


int gpu::BufferDynamic::initAsVertexBuffer ( Format format, const uint64_t element_count )
{
	allocateIfNeeded(pInternal);
	if (pInternal)
	{
		return pInternal->initAsVertexBuffer(format, element_count);
	}
	return gpu::kErrorNullReference;
}

int gpu::BufferDynamic::initAsIndexBuffer ( IndexFormat format, const uint64_t element_count ) 
{
	allocateIfNeeded(pInternal);
	if (pInternal)
	{
		return pInternal->initAsIndexBuffer(format, element_count);
	}
	return gpu::kErrorNullReference;
}

int gpu::BufferDynamic::initAsConstantBuffer ( const uint64_t data_size, const MemoryType where ) 
{
	allocateIfNeeded(pInternal);
	if (pInternal)
	{
		return pInternal->initAsConstantBuffer(data_size, where);
	}
	return gpu::kErrorNullReference;
}

int gpu::BufferDynamic::initAsStructuredBuffer ( const uint64_t data_size )
{
	allocateIfNeeded(pInternal);
	if (pInternal)
	{
		return pInternal->initAsStructuredBuffer(data_size);
	}
	return gpu::kErrorNullReference;
}

int gpu::BufferDynamic::initAsIndirectArgs ( const uint64_t data_size )
{
	allocateIfNeeded(pInternal);
	if (pInternal)
	{
		return pInternal->initAsIndirectArgs(data_size);
	}
	return gpu::kErrorNullReference;
}

int gpu::BufferDynamic::initAsTextureBuffer (
	const core::gfx::tex::arTextureType type,
	const core::gfx::tex::arColorFormat format,
	const uint64_t element_width, const uint64_t element_height, const uint64_t element_depth
)
{
	allocateIfNeeded(pInternal);
	if (pInternal)
	{
		return pInternal->initAsTextureBuffer(type, format, element_width, element_height, element_depth);
	}
	return gpu::kErrorNullReference;
}

int gpu::BufferDynamic::initAsReadbackBuffer ( const uint64_t data_size )
{
	allocateIfNeeded(pInternal);
	if (pInternal)
	{
		return pInternal->initAsReadbackBuffer(data_size);
	}
	return gpu::kErrorNullReference;
}
		
int gpu::BufferDynamic::initAsTextureReadbackBuffer  (
	const core::gfx::tex::arTextureType type,
	const core::gfx::tex::arColorFormat format,
	const uint64_t element_width, const uint64_t element_height, const uint64_t element_depth
)
{
	allocateIfNeeded(pInternal);
	if (pInternal)
	{
		return pInternal->initAsTextureReadbackBuffer(type, format, element_width, element_height, element_depth);
	}
	return gpu::kErrorNullReference;
}

int gpu::BufferDynamic::free ( void )
{
	int result = gpu::kErrorNullReference;
	if (pInternal)
		result = pInternal->free();
	delete_safe(pInternal);
	return result;
}