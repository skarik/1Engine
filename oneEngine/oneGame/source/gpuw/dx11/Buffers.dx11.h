#ifndef GPU_WRAPPER_BUFFERS_DX11_H_
#define GPU_WRAPPER_BUFFERS_DX11_H_

#include "core/types/types.h"
#include "core/gfx/textureFormats.h"

#include "renderer/types/types.h"
#include "gpuw/Public/Transfer.h"
#include "gpuw/Public/Formats.h"

#include "gpuw/base/Buffers.base.h"

namespace gpu {
namespace dx11
{
	class Device;
	class GraphicsContext;
	class ComputeContext;
	class BaseContext;

	class Buffer
	{
	public:
		GPUW_API				Buffer ( void );

		//	initAsVertexBuffer( device, format, element_count ) : Initializes as a vertex buffer.
		// Data is uploaded separately through map/unmap or upload.
		GPUW_API int			initAsVertexBuffer ( Device* device, Format format, const uint64_t element_count );

		//	initAsIndexBuffer( device, format, element_count ) : Initializes as an index buffer.
		// Data is uploaded separately through map/unmap or upload.
		GPUW_API int			initAsIndexBuffer ( Device* device, IndexFormat format, const uint64_t element_count );

		//	initAsData( device, data_size ) : Initializes as a constant buffer.
		// Data is uploaded separately through map/unmap or upload.
		GPUW_API int			initAsConstantBuffer ( Device* device, const uint64_t data_size, const MemoryType where = kMemoryTypeHeap );

		//	initAsStructuredBuffer( device, data_size ) : Initializes as a data buffer.
		// Data is uploaded separately through map/unmap or upload.
		GPUW_API int			initAsStructuredBuffer ( Device* device, const uint64_t data_size );

		//	initAsIndirectArgs( device, data_size ) : Initializes as a data buffer, able to be used for indirect args.
		// Data is uploaded separately through map/unmap or upload.
		GPUW_API int			initAsIndirectArgs ( Device* device, const uint64_t data_size );

		//	initAsTextureBuffer( device, type, format, element_width, element_height, element_width ) : Initializes as a typed data buffer. Can be used to load textures.
		// Data is uploaded separately through map/unmap or upload.
		GPUW_API int			initAsTextureBuffer (
			Device* device,
			const core::gfx::tex::arTextureType type,
			const core::gfx::tex::arColorFormat format,
			const uint64_t element_width, const uint64_t element_height, const uint64_t element_depth
		);

		//	initAsReadbackBuffer( device, data_size ) : Initializes as a data buffer to be used for reading on CPU.
		GPUW_API int			initAsReadbackBuffer ( Device* device, const uint64_t data_size );
		
		//	initAsTextureReadbackBuffer( device, type, format, element_width, element_height, element_width ) : Initializes as a typed data buffer. Can be used to read textures.
		GPUW_API int			initAsTextureReadbackBuffer  (
			Device* device,
			const core::gfx::tex::arTextureType type,
			const core::gfx::tex::arColorFormat format,
			const uint64_t element_width, const uint64_t element_height, const uint64_t element_depth
		);

		//	map( device, style, out row_pitch ) : Maps the entire buffer to CPU-side memory and returns the address.
		GPUW_API void*			map ( BaseContext* context, const TransferStyle style, uint32& out_row_pitch );
		//	map( device, style ) : Maps the entire buffer to CPU-side memory and returns the address.
		GPUW_API void*			map ( BaseContext* context, const TransferStyle style );

		//	map( device, style ) : Maps the entire buffer to CPU-side memory and returns the address.
		GPUW_API void*			mapSubregion ( BaseContext* context, const TransferStyle style, uint32& out_row_pitch );
		//	unmap( device ) : Unmaps the buffer.
		// If the mapping was Static, this is a synchronous operation, waiting for the data to upload to the device.
		GPUW_API int			unmap ( BaseContext* context );

		//	upload( data, data_size, transfer ) : upload a buffer with data
		GPUW_API int			upload ( BaseContext* context, void* data, const uint64_t data_size, const TransferStyle style );
		//	uploadElements()
		GPUW_API int			uploadElements ( BaseContext* context, void* data, const uint64_t element_count, const TransferStyle style );

		//	free() : destroys any allocated buffer, if existing.
		// Assume that any use of the buffer after this point will be invalid, so avoid 1-frame buffers if possible
		GPUW_API int			free ( Device* device );

		//	valid() : is this buffer valid to be used?
		GPUW_API bool			valid ( void );

		//	nativePtr() : returns native index or pointer to the resource.
		GPUW_API gpuHandle		nativePtr ( void );

		//	getFormat() : returns underlying format of the data, if applicable.
		// For constant buffers, regular buffers, and structured buffers, this will always be kFormatUndefined.
		GPUW_API Format			getFormat ( void ) const;

		//	getBufferType() : returns the buffer type.
		GPUW_API gpu::BufferType
								getBufferType ( void ) const;

	private:
		friend GraphicsContext;
		friend ComputeContext;

		gpu::BufferType	m_bufferType;
		void*			m_buffer;
		unsigned int	m_elementSize;
		Format			m_format;

		void*			m_srv;
		void*			m_uav;

#	if GPU_API_DEBUG_MAP_OVERRUNS
		void*			m_intermediateBuffer;
		size_t			m_intermediateBufferSize;
		void*			m_actualBuffer;
#	endif
	};

}}

#endif//GPU_WRAPPER_BUFFERS_DX11_H_