#ifndef GPU_WRAPPER_DYNAMIC_BUFFERS_H_
#define GPU_WRAPPER_DYNAMIC_BUFFERS_H_

#include "gpuw/gpuw_common.h"
#include "gpuw/base/Buffers.base.h"

/*#include "gpuw/module_config.h"
#ifdef GPU_API_OPENGL
#	include "gpuw/ogl/Buffers.h"
#endif

#ifdef GPU_API_VULKAN
#	include "gpuw/vk/Buffers.vk.h"
#endif

#ifdef GPU_API_DIRECTX11
#	include "gpuw/dx11/Buffers.dx11.h"
#endif*/

namespace gpu
{
	class BufferDynamic : public gpu::base::Buffer
	{
	public:
		// @brief Initializes as a vertex buffer.
		//		Data is uploaded separately through map/unmap or upload.
		GPUW_API int			initAsVertexBuffer ( Format format, const uint64_t element_count ) override;

		// @brief Initializes as an index buffer.
		//		Data is uploaded separately through map/unmap or upload.
		GPUW_API int			initAsIndexBuffer ( IndexFormat format, const uint64_t element_count ) override;

		// @brief Initializes as a constant buffer.
		//		Data is uploaded separately through map/unmap or upload.
		GPUW_API int			initAsConstantBuffer ( const uint64_t data_size, const MemoryType where = kMemoryTypeHeap ) override;

		// @brief Initializes as a data buffer.
		//		Data is uploaded separately through map/unmap or upload.
		GPUW_API int			initAsStructuredBuffer ( const uint64_t data_size ) override;

		// @brief Initializes as a data buffer, able to be used for indirect args.
		//		Data is uploaded separately through map/unmap or upload.
		GPUW_API int			initAsIndirectArgs ( const uint64_t data_size ) override;

		// @brief Initializes as a typed data buffer. Can be used to load textures.
		//		Data is uploaded separately through map/unmap or upload.
		GPUW_API int			initAsTextureBuffer (
			const core::gfx::tex::arTextureType type,
			const core::gfx::tex::arColorFormat format,
			const uint64_t element_width, const uint64_t element_height, const uint64_t element_depth
		) override;

		// @brief Initializes as a data buffer to be used for reading on CPU.
		GPUW_API int			initAsReadbackBuffer ( const uint64_t data_size ) override;
		
		// @brief Initializes as a typed data buffer. Can be used to read textures.
		GPUW_API int			initAsTextureReadbackBuffer  (
			const core::gfx::tex::arTextureType type,
			const core::gfx::tex::arColorFormat format,
			const uint64_t element_width, const uint64_t element_height, const uint64_t element_depth
		) override;

		// @brief Maps the entire buffer to CPU-side memory and returns the address.
		GPUW_API void*			map ( gpu::base::BaseContext* context, const TransferStyle style, uint32& out_row_pitch ) override
		{
			return pInternal ? pInternal->map( context, style, out_row_pitch ) : nullptr;
		}
		// @brief Maps the entire buffer to CPU-side memory and returns the address.
		GPUW_API void*			map ( gpu::base::BaseContext* context, const TransferStyle style ) override
		{
			return pInternal ? pInternal->map( context, style ) : nullptr;
		}

		// @brief Maps the entire buffer to CPU-side memory and returns the address.
		GPUW_API void*			mapSubregion ( gpu::base::BaseContext* context, const TransferStyle style, uint64_t byte_offset, uint64_t byte_size, uint32& out_row_pitch ) override
		{
			return pInternal ? pInternal->mapSubregion( context, style, byte_offset, byte_size, out_row_pitch ) : nullptr;
		}
		// @brief Unmaps the buffer.
		//		If the mapping was Static, this is a synchronous operation, waiting for the data to upload to the device.
		GPUW_API int			unmap ( gpu::base::BaseContext* context ) override
		{
			return pInternal ? pInternal->unmap( context ) : kErrorNullReference;
		}

		// @brief upload a buffer with data
		GPUW_API int			upload ( gpu::base::BaseContext* context, void* data, const uint64_t data_size, const TransferStyle style ) override
		{
			return pInternal ? pInternal->upload( context, data, data_size, style ) : kErrorNullReference;
		}
		// @brief
		GPUW_API int			uploadElements ( gpu::base::BaseContext* context, void* data, const uint64_t element_count, const TransferStyle style ) override
		{
			return pInternal ? pInternal->uploadElements( context, data, element_count, style ) : kErrorNullReference;
		}

		// @brief destroys any allocated buffer, if existing.
		//		Assume that any use of the buffer after this point will be invalid, so avoid 1-frame buffers if possible
		GPUW_API int			free ( void ) override;

		// @brief is this buffer valid to be used?
		GPUW_API bool			valid ( void ) override
		{
			return pInternal ? pInternal->valid() : false;
		}

		// @brief returns native index or pointer to the resource.
		GPUW_API gpuHandle		nativePtr ( void ) override
		{
			return pInternal ? pInternal->nativePtr() : gpuHandle();
		}

		// @brief returns underlying format of the data, if applicable.
		//		For constant buffers, regular buffers, and structured buffers, this will always be kFormatUndefined.
		GPUW_API Format			getFormat ( void ) const override
		{
			return pInternal ? pInternal->getFormat() : kFormatUndefined;
		}

		// @brief returns the buffer type.
		GPUW_API BufferType		getBufferType ( void ) const override
		{
			return pInternal ? pInternal->getBufferType() : kBufferTypeUnknown;
		}

	private:
		gpu::base::Buffer*	pInternal = nullptr;
	};

	typedef BufferDynamic Buffer;
}

#endif//GPU_WRAPPER_DYNAMIC_BUFFERS_H_