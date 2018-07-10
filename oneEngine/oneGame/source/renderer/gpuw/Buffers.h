#ifndef GPU_WRAPPER_BUFFERS_H_
#define GPU_WRAPPER_BUFFERS_H_

#include "core/types/types.h"
#include "core/gfx/textureFormats.h"

#include "renderer/types/types.h"
#include "renderer/gpuw/Transfer.h"
#include "renderer/ogl/GLCommon.h"

namespace gpu
{
	class ConstantBuffer
	{
	public:
		////	Constructor : creates uninitalized GPU wrapper object.
		//RENDER_API explicit		ConstantBuffer ( void );
		////	Destructor : destroys any allocated buffer, if existing.
		//// does nothing
		//RENDER_API 				~ConstantBuffer ( void );

		//	valid() : is this buffer valid to be used?
		// If the buffer has not been created, it will be removed.
		RENDER_API bool			valid ( void );
		//	getGlIndex() : returns index of resource in OpenGL
		RENDER_API gpuHandle	nativePtr ( void );

		//	init( data, data_size, transfer ) : initializes a constant buffer with data
		RENDER_API int			init ( void* data, const  uint64_t data_size, const TransferStyle style );
		//	upload( data, data_size, transfer ) : initializes and upload a constant buffer with data
		RENDER_API int			upload ( void* data, const  uint64_t data_size, const TransferStyle style );
		//	free() : destroys any allocated buffer, if existing.
		RENDER_API int			free ( void );

	private:
		GLuint	m_buffer;
	};
}

#endif//GPU_WRAPPER_BUFFERS_H_