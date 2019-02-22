#include "gpuw/gpuw_common.h"
#ifdef GPU_API_OPENGL

#include "./Buffers.h"
#include "gpuw/Public/Error.h"
#include "./ogl/GLCommon.h"
#include "core/debug.h"

#include <algorithm>

gpu::Buffer::Buffer ( void ) :
	m_bufferType(kBufferTypeUnknown),
	m_buffer(0),
	m_elementSize(0),
	m_format(kFormatUndefined)
{}

//	valid () : is this buffer valid to be used?
bool gpu::Buffer::valid ( void )
{
	return m_buffer != 0;
}
//	getGlIndex() : returns index of resource in OpenGL
gpuHandle gpu::Buffer::nativePtr ( void )
{
	return (gpuHandle)m_buffer;
}

static void _AllocateBufferSize ( const GLuint buffer, const uint64_t data_size, const gpu::TransferStyle style )
{
	if (style == gpu::kTransferStatic)
		//glNamedBufferStorage(buffer, (GLsizeiptr)std::max<uint64_t>(data_size, 4096), NULL, GL_MAP_WRITE_BIT); // Attempting to fix Quadro issues
		glNamedBufferStorage(buffer, (GLsizeiptr)data_size, NULL, GL_MAP_WRITE_BIT);
	//else if (style == gpu::kTransferDynamic)
	//	glNamedBufferStorage(buffer, (GLsizeiptr)data_size, NULL, GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT);
	else if (style == gpu::kTransferStream)
		//glNamedBufferStorage(buffer, (GLsizeiptr)std::max<uint64_t>(data_size, 4096), NULL, GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT); // Attempting to fix Quadro issues
		glNamedBufferStorage(buffer, (GLsizeiptr)data_size, NULL, GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT);
}

//	init ( data ) : initializes a constant buffer with data
int	gpu::Buffer::initAsData ( Device* device, const uint64_t data_size )
{
	ARCORE_ASSERT(data_size > 0);

	m_bufferType = kBufferTypeGeneralUse;
	m_format = kFormatUndefined;

	glCreateBuffers(1, &m_buffer);

	_AllocateBufferSize(m_buffer, data_size, kTransferStream);
	
	return kError_SUCCESS;
}

//	initAsVertexBuffer( device, format, element_count ) : Initializes as a vertex buffer.
int gpu::Buffer::initAsVertexBuffer ( Device* device, Format format, const uint64_t element_count )
{
	ARCORE_ASSERT(element_count > 0);

	m_bufferType = kBufferTypeVertex;
	m_elementSize = (unsigned int)gpu::FormatGetByteStride(format);
	m_format = format;

	glCreateBuffers(1, &m_buffer);

	_AllocateBufferSize(m_buffer, m_elementSize * element_count, kTransferStream);

	return kError_SUCCESS;
}

//	initAsIndexBuffer( device, format, element_count ) : Initializes as an index buffer.
int gpu::Buffer::initAsIndexBuffer ( Device* device, IndexFormat format, const uint64_t element_count )
{
	ARCORE_ASSERT(element_count > 0);

	m_bufferType = kBufferTypeIndex;
	m_elementSize = (format == kIndexFormatUnsigned16) ? 2 : 4;
	m_format = (format == kIndexFormatUnsigned16) ? kFormatR16UInteger : kFormatR32UInteger;

	glCreateBuffers(1, &m_buffer);

	_AllocateBufferSize(m_buffer, m_elementSize * element_count, kTransferStream);

	return kError_SUCCESS;
}

//	initAsData( device, data_size ) : Initializes as a constant buffer.
int gpu::Buffer::initAsConstantBuffer ( Device* device, const uint64_t data_size )
{
	ARCORE_ASSERT(data_size > 0);

	m_bufferType = kBufferTypeConstant;
	m_format = kFormatUndefined;
	
	glCreateBuffers(1, &m_buffer);

	_AllocateBufferSize(m_buffer, data_size, kTransferStream);

	return kError_SUCCESS;
}

//	initAsStructuredBuffer( device, data_size ) : Initializes as a data buffer.
// Data is uploaded separately through map/unmap or upload.
int gpu::Buffer::initAsStructuredBuffer ( Device* device, const uint64_t data_size )
{
	ARCORE_ASSERT(data_size > 0);

	m_bufferType = kBufferTypeStructured;
	m_format = kFormatUndefined;

	glCreateBuffers(1, &m_buffer);

	_AllocateBufferSize(m_buffer, data_size, kTransferStream);

	return kError_SUCCESS;
}

void* gpu::Buffer::map ( Device* device, const TransferStyle style )
{
	ARCORE_ASSERT(m_buffer != NIL);
	//todo: correct flags
	return glMapNamedBuffer(m_buffer, GL_WRITE_ONLY);
}
int gpu::Buffer::unmap ( Device* device )
{
	ARCORE_ASSERT(m_buffer != NIL);
	glUnmapNamedBuffer(m_buffer);
	//glMemoryBarrier(GL_ELEMENT_ARRAY_BARRIER_BIT | GL_BUFFER_UPDATE_BARRIER_BIT); // attempting to fix Quadro issues

	return kError_SUCCESS;
}

//	upload( data, data_size, transfer ) : upload a buffer with data
int	gpu::Buffer::upload ( Device* device, void* data, const  uint64_t data_size, const TransferStyle style )
{
	ARCORE_ASSERT(m_buffer != NIL);

	glNamedBufferSubData(m_buffer, 0, (GLsizeiptr)data_size, data);
	//glMemoryBarrier(GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT | GL_BUFFER_UPDATE_BARRIER_BIT); // attempting to fix Quadro issues

	return 0;
}

//	upload( data, data_size, transfer ) : initializes and upload a constant buffer with data
int	gpu::Buffer::uploadElements ( Device* device, void* data, const  uint64_t element_count, const TransferStyle style )
{
	ARCORE_ASSERT(m_buffer != NIL);
	ARCORE_ASSERT(m_elementSize != 0);

	glNamedBufferSubData(m_buffer, 0, (GLsizeiptr)(m_elementSize * element_count), data);

	return 0;
}

//	free() : destroys any allocated buffer, if existing.
int	gpu::Buffer::free ( Device* device )
{
	glDeleteBuffers(1, &m_buffer);
	m_buffer = 0;
	m_bufferType = kBufferTypeUnknown;
	m_format = kFormatUndefined;

	return 0;
}

//	getFormat() : returns underlying format of the data, if applicable.
gpu::Format gpu::Buffer::getFormat ( void )
{
	return m_format;
}

//	getBufferType() : returns the buffer type.
gpu::BufferType gpu::Buffer::getBufferType ( void )
{
	return m_bufferType;
}

#endif