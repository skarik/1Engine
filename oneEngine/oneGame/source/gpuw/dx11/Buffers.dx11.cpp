#include "gpuw/gpuw_common.h"
#ifdef GPU_API_DIRECTX11

#include "./Device.dx11.h"
#include "./Buffers.dx11.h"

#include "gpuw/Public/Error.h"
#include "./gpu.h"
#include "core/debug.h"

#include <algorithm>

gpu::Buffer::Buffer ( void ) :
	m_bufferType(kBufferTypeUnknown),
	m_buffer(0),
	m_elementSize(0),
	m_format(kFormatUndefined),
	m_srv(NULL),
	m_uav(NULL)
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

/*static void _AllocateBufferSize ( const GLuint buffer, const uint64_t data_size, const gpu::TransferStyle style )
{
	if (style == gpu::kTransferStatic)
		//glNamedBufferStorage(buffer, (GLsizeiptr)std::max<uint64_t>(data_size, 4096), NULL, GL_MAP_WRITE_BIT); // Attempting to fix Quadro issues
		glNamedBufferStorage(buffer, (GLsizeiptr)data_size, NULL, GL_MAP_WRITE_BIT);
	//else if (style == gpu::kTransferDynamic)
	//	glNamedBufferStorage(buffer, (GLsizeiptr)data_size, NULL, GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT);
	else if (style == gpu::kTransferStream)
		//glNamedBufferStorage(buffer, (GLsizeiptr)std::max<uint64_t>(data_size, 4096), NULL, GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT); // Attempting to fix Quadro issues
		glNamedBufferStorage(buffer, (GLsizeiptr)data_size, NULL, GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT);
}*/

//	init ( data ) : initializes a general buffer with data. Can be used to load textures
int	gpu::Buffer::initAsData ( Device* device, const uint64_t data_size )
{
	ARCORE_ASSERT(data_size > 0);

	m_bufferType = kBufferTypeGeneralUse;
	m_format = kFormatUndefined;

	D3D11_BUFFER_DESC bufferInfo = {};
	bufferInfo.Usage = D3D11_USAGE_DEFAULT;
	bufferInfo.ByteWidth = (UINT)data_size;
	bufferInfo.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	bufferInfo.CPUAccessFlags = 0;
	bufferInfo.MiscFlags = D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS;
	bufferInfo.StructureByteStride = 0;

	device->getNative()->CreateBuffer(&bufferInfo, NULL, (ID3D11Buffer**)&m_buffer);

	//_AllocateBufferSize(m_buffer, data_size, kTransferStream);
	
	return kError_SUCCESS;
}

//	initAsVertexBuffer( device, format, element_count ) : Initializes as a vertex buffer.
int gpu::Buffer::initAsVertexBuffer ( Device* device, Format format, const uint64_t element_count )
{
	ARCORE_ASSERT(element_count > 0);

	m_bufferType = kBufferTypeVertex;
	m_elementSize = (unsigned int)gpu::FormatGetByteStride(format);
	m_format = format;

	D3D11_BUFFER_DESC bufferInfo = {};
	bufferInfo.Usage = D3D11_USAGE_DEFAULT;
	bufferInfo.ByteWidth = (UINT)(m_elementSize * element_count);
	bufferInfo.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferInfo.CPUAccessFlags = 0;
	bufferInfo.MiscFlags = 0;
	bufferInfo.StructureByteStride = 0;

	device->getNative()->CreateBuffer(&bufferInfo, NULL, (ID3D11Buffer**)&m_buffer);

	//_AllocateBufferSize(m_buffer, m_elementSize * element_count, kTransferStream);

	return kError_SUCCESS;
}

//	initAsIndexBuffer( device, format, element_count ) : Initializes as an index buffer.
int gpu::Buffer::initAsIndexBuffer ( Device* device, IndexFormat format, const uint64_t element_count )
{
	ARCORE_ASSERT(element_count > 0);

	m_bufferType = kBufferTypeIndex;
	m_elementSize = (format == kIndexFormatUnsigned16) ? 2 : 4;
	m_format = (format == kIndexFormatUnsigned16) ? kFormatR16UInteger : kFormatR32UInteger;

	D3D11_BUFFER_DESC bufferInfo = {};
	bufferInfo.Usage = D3D11_USAGE_DEFAULT;
	bufferInfo.ByteWidth = (UINT)(m_elementSize * element_count);
	bufferInfo.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferInfo.CPUAccessFlags = 0;
	bufferInfo.MiscFlags = 0;
	bufferInfo.StructureByteStride = 0;

	device->getNative()->CreateBuffer(&bufferInfo, NULL, (ID3D11Buffer**)&m_buffer);

	//_AllocateBufferSize(m_buffer, m_elementSize * element_count, kTransferStream);

	return kError_SUCCESS;
}

//	initAsData( device, data_size ) : Initializes as a constant buffer.
int gpu::Buffer::initAsConstantBuffer ( Device* device, const uint64_t data_size )
{
	ARCORE_ASSERT(data_size > 0);

	m_bufferType = kBufferTypeConstant;
	m_format = kFormatUndefined;
	
	D3D11_BUFFER_DESC bufferInfo = {};
	bufferInfo.Usage = D3D11_USAGE_DEFAULT;
	bufferInfo.ByteWidth = (UINT)data_size;
	bufferInfo.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferInfo.CPUAccessFlags = 0;
	bufferInfo.MiscFlags = 0;
	bufferInfo.StructureByteStride = 0;

	device->getNative()->CreateBuffer(&bufferInfo, NULL, (ID3D11Buffer**)&m_buffer);

	//_AllocateBufferSize(m_buffer, data_size, kTransferStream);

	return kError_SUCCESS;
}

//	initAsStructuredBuffer( device, data_size ) : Initializes as a data buffer.
// Data is uploaded separately through map/unmap or upload.
int gpu::Buffer::initAsStructuredBuffer ( Device* device, const uint64_t data_size )
{
	ARCORE_ASSERT(data_size > 0);

	m_bufferType = kBufferTypeStructured;
	m_format = kFormatUndefined;

	D3D11_BUFFER_DESC bufferInfo = {};
	bufferInfo.Usage = D3D11_USAGE_DEFAULT;
	bufferInfo.ByteWidth = (UINT)data_size;
	bufferInfo.BindFlags = D3D11_BIND_SHADER_RESOURCE; // SBuffers bound as SRVs for readonly-SSBO parity.
	bufferInfo.CPUAccessFlags = 0;
	bufferInfo.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS | D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	bufferInfo.StructureByteStride = 0;

	device->getNative()->CreateBuffer(&bufferInfo, NULL, (ID3D11Buffer**)&m_buffer);

	// Create SRV for the buffer
	D3D11_SHADER_RESOURCE_VIEW_DESC srvInfo = {};
	srvInfo.Format = DXGI_FORMAT_R8_UINT;
	srvInfo.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	srvInfo.BufferEx.FirstElement = 0;
	srvInfo.BufferEx.NumElements = (UINT)data_size;
	srvInfo.BufferEx.Flags = D3D11_BUFFEREX_SRV_FLAG_RAW;
	device->getNative()->CreateShaderResourceView((ID3D11Buffer*)m_buffer, &srvInfo, (ID3D11ShaderResourceView**)&m_srv);

	return kError_SUCCESS;
}

void* gpu::Buffer::map ( Device* device, const TransferStyle style )
{
	ARCORE_ASSERT(m_buffer != NIL);

	D3D11_MAPPED_SUBRESOURCE resource;
	device->getNativeContext()->Map((ID3D11Buffer*)m_buffer, 0, D3D11_MAP_WRITE, 0, &resource); 
	//todo: correct flags
	return resource.pData;
}
int gpu::Buffer::unmap ( Device* device )
{
	ARCORE_ASSERT(m_buffer != NIL);
	// TODO: getNative cannot be used here, most likely.
	device->getNativeContext()->Unmap((ID3D11Buffer*)m_buffer, 0);
	
	return kError_SUCCESS;
}

//	upload( data, data_size, transfer ) : upload a buffer with data
int	gpu::Buffer::upload ( Device* device, void* data, const  uint64_t data_size, const TransferStyle style )
{
	ARCORE_ASSERT(m_buffer != NIL);

	void* data_target = map(device, style);
		memcpy(data_target, data, data_size);
	unmap(device);

	return kError_SUCCESS;
}

//	upload( data, data_size, transfer ) : initializes and upload a constant buffer with data
int	gpu::Buffer::uploadElements ( Device* device, void* data, const  uint64_t element_count, const TransferStyle style )
{
	ARCORE_ASSERT(m_buffer != NIL);
	ARCORE_ASSERT(m_elementSize != 0);

	return upload(device, data, m_elementSize * element_count, style);
}

//	free() : destroys any allocated buffer, if existing.
int	gpu::Buffer::free ( Device* device )
{
	static_cast<ID3D11Buffer*>(m_buffer)->Release();
	if (m_srv)
		static_cast<ID3D11ShaderResourceView*>(m_srv)->Release();
	if (m_uav)
		static_cast<ID3D11UnorderedAccessView*>(m_uav)->Release();

	m_buffer = NULL;
	m_srv = NULL;
	m_uav = NULL;

	m_bufferType = kBufferTypeUnknown;
	m_format = kFormatUndefined;

	return kError_SUCCESS;
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

#endif//GPU_API_DIRECTX11