#ifndef GPU_WRAPPER_BUFFERS_BASE_H_
#define GPU_WRAPPER_BUFFERS_BASE_H_

#include "core/types/types.h"
#include "core/gfx/textureFormats.h"

#include "renderer/types/types.h"
#include "gpuw/Public/Transfer.h"
#include "gpuw/Public/Formats.h"

namespace gpu
{
	enum BufferType
	{
		kBufferTypeUnknown,
		// Vertex buffer.
		// Meant to be used as a vertex stream into to a vertex shader.
		kBufferTypeVertex,
		// Index buffer.
		// Meant to be used as a index stream for the inputs to a vs/gs shader.
		kBufferTypeIndex,
		// Constant buffer. 
		// Corresponds to a cbuffer type in shaders.
		// Uses at minimum 1KB and at most 4KB on certain platforms.
		kBufferTypeConstant,
		// Structured buffer.
		// Corresponds to a StructuredBuffer type in shaders, RegularBuffer? (TODO) on other platforms.
		// Uses at minimum 4KB and at most 65MB on certain platforms.
		kBufferTypeStructured,
		// Indirect args buffer.
		// Use for storing arguments for indirect draws.
		kBufferTypeIndirectArgs,
		// Texture buffer.
		// Use for loading textures.
		kBufferTypeTexture,

		// Readback buffer.
		// Used for reading GPU data on the CPU.
		kBufferTypeReadback,
		// Readback texture buffer.
		// Used for reading specifically texture GPU data on the CPU.
		kBufferTypeTextureReadback,
	};

	enum MemoryType
	{
		// Allocates memory from the managed GPU heap.
		kMemoryTypeHeap,

		// Allocates from the command buffer stack. Use for Push-Constants.
		// When not available (such as DX11) will default to allocating from the heap.
		kMemoryTypeSingleFrame,
	};
}

namespace gpu {
namespace base 
{
	class Device;
	class GraphicsContext;
	class ComputeContext;
	class BaseContext;

	class Buffer
	{
	public:
		// @brief Initializes as a vertex buffer.
		//		Data is uploaded separately through map/unmap or upload.
		GPUW_API virtual int	initAsVertexBuffer ( Format format, const uint64_t element_count ) =0;

		// @brief Initializes as an index buffer.
		//		Data is uploaded separately through map/unmap or upload.
		GPUW_API virtual int	initAsIndexBuffer ( IndexFormat format, const uint64_t element_count ) =0;

		// @brief Initializes as a constant buffer.
		//		Data is uploaded separately through map/unmap or upload.
		GPUW_API virtual int	initAsConstantBuffer ( const uint64_t data_size, const MemoryType where = kMemoryTypeHeap ) =0;

		// @brief Initializes as a data buffer.
		//		Data is uploaded separately through map/unmap or upload.
		GPUW_API virtual int	initAsStructuredBuffer ( const uint64_t data_size ) =0;

		// @brief Initializes as a data buffer, able to be used for indirect args.
		//		Data is uploaded separately through map/unmap or upload.
		GPUW_API virtual int	initAsIndirectArgs ( const uint64_t data_size ) =0;

		// @brief Initializes as a typed data buffer. Can be used to load textures.
		//		Data is uploaded separately through map/unmap or upload.
		GPUW_API virtual int	initAsTextureBuffer (
			const core::gfx::tex::arTextureType type,
			const core::gfx::tex::arColorFormat format,
			const uint64_t element_width, const uint64_t element_height, const uint64_t element_depth
		) =0;

		// @brief Initializes as a data buffer to be used for reading on CPU.
		GPUW_API virtual int	initAsReadbackBuffer ( const uint64_t data_size ) =0;
		
		// @brief Initializes as a typed data buffer. Can be used to read textures.
		GPUW_API virtual int	initAsTextureReadbackBuffer  (
			const core::gfx::tex::arTextureType type,
			const core::gfx::tex::arColorFormat format,
			const uint64_t element_width, const uint64_t element_height, const uint64_t element_depth
		) =0;

		// @brief Maps the entire buffer to CPU-side memory and returns the address.
		GPUW_API virtual void*	map ( gpu::base::BaseContext* context, const TransferStyle style, uint32& out_row_pitch ) =0;
		// @brief Maps the entire buffer to CPU-side memory and returns the address.
		GPUW_API virtual void*	map ( gpu::base::BaseContext* context, const TransferStyle style ) =0;
		// @brief Maps the given region of the buffer to CPU-side memory and returns the address.
		GPUW_API virtual void*	mapSubregion ( gpu::base::BaseContext* context, const TransferStyle style, uint64_t byte_offset, uint64_t byte_size, uint32& out_row_pitch ) =0;
		// @brief Unmaps the buffer.
		//	If the mapping was Static, this is a synchronous operation, waiting for the data to upload to the device.
		GPUW_API virtual int	unmap ( gpu::base::BaseContext* context ) =0;

		// @brief upload a buffer with data
		GPUW_API virtual int	upload ( gpu::base::BaseContext* context, void* data, const uint64_t data_size, const TransferStyle style ) =0;
		// @brief
		GPUW_API virtual int	uploadElements ( gpu::base::BaseContext* context, void* data, const uint64_t element_count, const TransferStyle style ) =0;

		// @brief destroys any allocated buffer, if existing.
		//		Assume that any use of the buffer after this point will be invalid, so avoid 1-frame buffers if possible
		GPUW_API virtual int	free ( void ) =0;

		// @brief is this buffer valid to be used?
		GPUW_API virtual bool	valid ( void ) =0;

		// @brief returns native index or pointer to the resource.
		GPUW_API virtual gpuHandle
								nativePtr ( void ) =0;

		// @brief returns underlying format of the data, if applicable.
		//		For constant buffers, regular buffers, and structured buffers, this will always be kFormatUndefined.
		GPUW_API virtual Format	getFormat ( void ) const =0;

		// @brief returns the buffer type.
		GPUW_API virtual BufferType
								getBufferType ( void ) const =0;

	private:
		friend gpu::base::GraphicsContext;
		friend gpu::base::ComputeContext;
	};

}}

#endif//GPU_WRAPPER_BUFFERS_BASE_H_