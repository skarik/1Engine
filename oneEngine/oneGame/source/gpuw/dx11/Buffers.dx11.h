#ifndef GPU_WRAPPER_BUFFERS_H_
#define GPU_WRAPPER_BUFFERS_H_

#include "core/types/types.h"
#include "core/gfx/textureFormats.h"

#include "renderer/types/types.h"
#include "gpuw/Public/Transfer.h"
#include "gpuw/Public/Formats.h"

namespace gpu
{
	class Device;
	class GraphicsContext;
	class ComputeContext;

	enum BufferType
	{
		kBufferTypeUnknown,
		// General use buffer.
		// Cannot be used with shaders, use one of the other types.
		kBufferTypeGeneralUse,
		// Constant buffer. 
		// Corresponds to a cbuffer type in shaders.
		// Uses at minimum 1KB and at most 4KB on certain platforms.
		kBufferTypeConstant,
		// Structured buffer.
		// Corresponds to a StructuredBuffer type in shaders, RegularBuffer? (TODO) on other platforms.
		// Uses at minimum 4KB and at most 65MB on certain platforms.
		kBufferTypeStructured,
		// Vertex buffer.
		// Meant to be used as a vertex stream into to a vertex shader.
		kBufferTypeVertex,
		// Index buffer.
		// Meant to be used as a index stream for the inputs to a vs/gs shader.
		kBufferTypeIndex,
	};

	class Buffer
	{
	public:
		GPUW_API				Buffer ( void );

		//	initAsData( device, data_size ) : Initializes as a data buffer.
		// Data is uploaded separately through map/unmap or upload.
		GPUW_API int			initAsData ( Device* device, const uint64_t data_size );
		
		//	initAsVertexBuffer( device, format, element_count ) : Initializes as a vertex buffer.
		// Data is uploaded separately through map/unmap or upload.
		GPUW_API int			initAsVertexBuffer ( Device* device, Format format, const uint64_t element_count );

		//	initAsIndexBuffer( device, format, element_count ) : Initializes as an index buffer.
		// Data is uploaded separately through map/unmap or upload.
		GPUW_API int			initAsIndexBuffer ( Device* device, IndexFormat format, const uint64_t element_count );

		//	initAsData( device, data_size ) : Initializes as a constant buffer.
		// Data is uploaded separately through map/unmap or upload.
		GPUW_API int			initAsConstantBuffer ( Device* device, const uint64_t data_size );

		//	initAsStructuredBuffer( device, data_size ) : Initializes as a data buffer.
		// Data is uploaded separately through map/unmap or upload.
		GPUW_API int			initAsStructuredBuffer ( Device* device, const uint64_t data_size );

		//	map( device, style ) : Maps the entire buffer to CPU-side memory and returns the address.
		GPUW_API void*			map ( Device* device, const TransferStyle style );
		//	unmap( device ) : Unmaps the buffer.
		// If the mapping was Static, this is a synchronous operation, waiting for the data to upload to the device.
		GPUW_API int			unmap ( Device* device );

		//	upload( data, data_size, transfer ) : upload a buffer with data
		GPUW_API int			upload ( Device* device, void* data, const uint64_t data_size, const TransferStyle style );
		//	uploadElements()
		GPUW_API int			uploadElements ( Device* device, void* data, const uint64_t element_count, const TransferStyle style );

		//	free() : destroys any allocated buffer, if existing.
		// Assume that any use of the buffer after this point will be invalid, so avoid 1-frame buffers if possible
		GPUW_API int			free ( Device* device );

		//	valid() : is this buffer valid to be used?
		GPUW_API bool			valid ( void );

		//	nativePtr() : returns native index or pointer to the resource.
		GPUW_API gpuHandle		nativePtr ( void );

		//	getFormat() : returns underlying format of the data, if applicable.
		// For constant buffers, regular buffers, and structured buffers, this will always be kFormatUndefined.
		GPUW_API Format			getFormat ( void );

		//	getBufferType() : returns the buffer type.
		GPUW_API BufferType		getBufferType ( void );

	private:
		friend GraphicsContext;
		friend ComputeContext;

		BufferType		m_bufferType;
		void*			m_buffer;
		unsigned int	m_elementSize;
		Format			m_format;
		//unsigned int	m_dataSize; // Needed in OpenGL for binding buffers with an offset.
	};

}

#endif//GPU_WRAPPER_BUFFERS_H_