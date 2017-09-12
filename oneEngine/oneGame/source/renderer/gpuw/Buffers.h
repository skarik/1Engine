#ifndef GPU_WRAPPER_BUFFERS_H_
#define GPU_WRAPPER_BUFFERS_H_

#include "core/types/types.h"
#include "renderer/types/types.h"
#include "renderer/types/textureFormats.h"

#include "renderer/gpuw/Transfer.h"

namespace gpu
{
	class ConstantBuffer
	{
	public:
		//	Constructor : creates uninitalized GPU wrapper object.
		explicit	ConstantBuffer( void );
		//	Destructor : destroys any allocated buffer, if existing.
					~ConstantBuffer( void );

		//	valid() : is this buffer valid to be used?
		// If the buffer has not been created, it will be removed.
		bool		valid ( void );

		//	init( data, data_size, transfer ) : initializes a constant buffer with data
		int			init ( void* data, const  uint64_t data_size, const TransferStyle style );
		//	upload( data, data_size, transfer ) : initializes and upload a constant buffer with data
		int			upload ( void* data, const  uint64_t data_size, const TransferStyle style );
		//	free() : destroys any allocated buffer, if existing.
		int			free ( void );

	private:
		glHandle	m_buffer;
	};
}

#endif//GPU_WRAPPER_BUFFERS_H_