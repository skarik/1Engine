#include "renderer/gpuw/Buffers.h"

#include "renderer/system/glMainSystem.h"

gpu::ConstantBuffer::ConstantBuffer( void )
{
	m_buffer = 0;
}
gpu::ConstantBuffer::~ConstantBuffer( void )
{
	// Free the buffer on death.
	if (m_buffer != 0)
	{
		glDeleteBuffers(1, &m_buffer);
		m_buffer = 0;
	}
}

//	valid () : is this buffer valid to be used?
// If the buffer has not been created, it will be removed.
bool gpu::ConstantBuffer::valid ( void )
{
	return m_buffer != 0;
}
//	getGlIndex() : returns index of resource in OpenGL
glHandle gpu::ConstantBuffer::getGlIndex ( void )
{
	return m_buffer;
}

static void _AllocateBufferSize ( const uint64_t data_size, const gpu::TransferStyle style )
{
	if (style == gpu::kTransferStatic)
		glBufferData( GL_UNIFORM_BUFFER, (GLsizeiptr)data_size, NULL, GL_STATIC_DRAW );
	else if (style == gpu::kTransferDynamic)
		glBufferData( GL_UNIFORM_BUFFER, (GLsizeiptr)data_size, NULL, GL_DYNAMIC_DRAW );
	else if (style == gpu::kTransferStream)
		glBufferData( GL_UNIFORM_BUFFER, (GLsizeiptr)data_size, NULL, GL_STREAM_DRAW );
}

//	init ( data ) : initializes a constant buffer with data
int	gpu::ConstantBuffer::init ( void* data, const uint64_t data_size, const TransferStyle style )
{
	glGenBuffers( 1, &m_buffer );
	glBindBuffer( GL_UNIFORM_BUFFER, m_buffer );
	_AllocateBufferSize(data_size, style);

	if (data != NULL)
	{
		upload(data, data_size, style);
	}
	
	return 0;
}

//	upload( data, data_size, transfer ) : initializes and upload a constant buffer with data
int	gpu::ConstantBuffer::upload ( void* data, const uint64_t data_size, const TransferStyle style )
{
	if (m_buffer == 0)
	{
		init(data, data_size, style);
	}
	else
	{
		glBindBuffer( GL_UNIFORM_BUFFER, m_buffer );
		_AllocateBufferSize(data_size, style);
	}

	glBufferSubData( GL_UNIFORM_BUFFER, 0, (GLsizeiptr)data_size, data );

	return 0;
}

//	free() : destroys any allocated buffer, if existing.
int	gpu::ConstantBuffer::free ( void )
{
	glDeleteBuffers(1, &m_buffer);
	m_buffer = 0;

	return 0;
}