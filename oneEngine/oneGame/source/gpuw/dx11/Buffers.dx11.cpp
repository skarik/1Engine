#include "gpuw/gpuw_common.h"
#ifdef GPU_API_DIRECTX11

#include "./Device.dx11.h"
#include "./Buffers.dx11.h"
#include "./Internal/Enums.dx11.h"

#include "gpuw/Public/Error.h"
#include "./gpu.h"
#include "core/debug.h"
#include "core/exceptions.h"

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

//	initAsVertexBuffer( device, format, element_count ) : Initializes as a vertex buffer.
int gpu::Buffer::initAsVertexBuffer ( Device* device, Format format, const uint64_t element_count )
{
	ARCORE_ASSERT(element_count > 0);
	if (device == NULL) device = getDevice();
	HRESULT result;

	m_bufferType = kBufferTypeVertex;
	m_elementSize = (unsigned int)gpu::FormatGetByteStride(format);
	m_format = format;

	D3D11_BUFFER_DESC bufferInfo = {};
	bufferInfo.Usage = D3D11_USAGE_DYNAMIC;
	bufferInfo.ByteWidth = (UINT)(m_elementSize * element_count);
	bufferInfo.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferInfo.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferInfo.MiscFlags = 0;
	bufferInfo.StructureByteStride = m_elementSize;

	result = device->getNative()->CreateBuffer(&bufferInfo, NULL, (ID3D11Buffer**)&m_buffer);
	if (FAILED(result))
	{
		throw core::OutOfMemoryException(); // TODO: Handle this better.
		m_buffer = NULL;
		return gpu::kErrorFormatUnsupported;
	}

	return kError_SUCCESS;
}

//	initAsIndexBuffer( device, format, element_count ) : Initializes as an index buffer.
int gpu::Buffer::initAsIndexBuffer ( Device* device, IndexFormat format, const uint64_t element_count )
{
	ARCORE_ASSERT(element_count > 0);
	if (device == NULL) device = getDevice();
	HRESULT result;

	m_bufferType = kBufferTypeIndex;
	m_elementSize = (format == kIndexFormatUnsigned16) ? 2 : 4;
	m_format = (format == kIndexFormatUnsigned16) ? kFormatR16UInteger : kFormatR32UInteger;

	D3D11_BUFFER_DESC bufferInfo = {};
	bufferInfo.Usage = D3D11_USAGE_DYNAMIC;
	bufferInfo.ByteWidth = (UINT)(m_elementSize * element_count);
	bufferInfo.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferInfo.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferInfo.MiscFlags = 0;
	bufferInfo.StructureByteStride = 0;

	result = device->getNative()->CreateBuffer(&bufferInfo, NULL, (ID3D11Buffer**)&m_buffer);
	if (FAILED(result))
	{
		throw core::OutOfMemoryException(); // TODO: Handle this better.
		m_buffer = NULL;
		return gpu::kErrorFormatUnsupported;
	}

	return kError_SUCCESS;
}

//	initAsData( device, data_size ) : Initializes as a constant buffer.
int gpu::Buffer::initAsConstantBuffer ( Device* device, const uint64_t data_size )
{
	ARCORE_ASSERT(data_size > 0);
	if (device == NULL) device = getDevice();
	HRESULT result;

	m_bufferType = kBufferTypeConstant;
	m_format = kFormatUndefined;
	
	D3D11_BUFFER_DESC bufferInfo = {};
	bufferInfo.Usage = D3D11_USAGE_DYNAMIC;
	bufferInfo.ByteWidth = (UINT)data_size;
	bufferInfo.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferInfo.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferInfo.MiscFlags = 0;
	bufferInfo.StructureByteStride = 0;

	result = device->getNative()->CreateBuffer(&bufferInfo, NULL, (ID3D11Buffer**)&m_buffer);
	if (FAILED(result))
	{
		throw core::OutOfMemoryException(); // TODO: Handle this better.
		m_buffer = NULL;
		return gpu::kErrorFormatUnsupported;
	}

	return kError_SUCCESS;
}

//	initAsStructuredBuffer( device, data_size ) : Initializes as a data buffer.
int gpu::Buffer::initAsStructuredBuffer ( Device* device, const uint64_t data_size )
{
	ARCORE_ASSERT(data_size > 0);
	if (device == NULL) device = getDevice();
	HRESULT result;

	m_bufferType = kBufferTypeStructured;
	m_format = kFormatUndefined;

	D3D11_BUFFER_DESC bufferInfo = {};
	bufferInfo.Usage = D3D11_USAGE_DYNAMIC;
	bufferInfo.ByteWidth = (UINT)data_size;
	bufferInfo.BindFlags = D3D11_BIND_SHADER_RESOURCE; // SBuffers bound as SRVs for readonly-SSBO parity.
	bufferInfo.CPUAccessFlags = 0;
	bufferInfo.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS | D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	bufferInfo.StructureByteStride = 0;

	result = device->getNative()->CreateBuffer(&bufferInfo, NULL, (ID3D11Buffer**)&m_buffer);
	if (FAILED(result))
	{
		throw core::OutOfMemoryException(); // TODO: Handle this better.
		m_buffer = NULL;
		return gpu::kErrorFormatUnsupported;
	}

	// Create SRV for the buffer
	D3D11_SHADER_RESOURCE_VIEW_DESC srvInfo = {};
	srvInfo.Format = DXGI_FORMAT_R8_UINT;
	srvInfo.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	srvInfo.BufferEx.FirstElement = 0;
	srvInfo.BufferEx.NumElements = (UINT)data_size;
	srvInfo.BufferEx.Flags = D3D11_BUFFEREX_SRV_FLAG_RAW;

	result = device->getNative()->CreateShaderResourceView((ID3D11Buffer*)m_buffer, &srvInfo, (ID3D11ShaderResourceView**)&m_srv);
	if (FAILED(result))
	{
		throw core::OutOfMemoryException(); // TODO: Handle this better.
	}

	return kError_SUCCESS;
}

//	initAsIndirectArgs( device, data_size ) : Initializes as a data buffer, able to be used for indirect args.
int gpu::Buffer::initAsIndirectArgs ( Device* device, const uint64_t data_size )
{
	ARCORE_ASSERT(data_size > 0);
	if (device == NULL) device = getDevice();
	HRESULT result;

	m_bufferType = kBufferTypeIndirectArgs;
	m_format = kFormatUndefined;

	D3D11_BUFFER_DESC bufferInfo = {};
	bufferInfo.Usage = D3D11_USAGE_DYNAMIC;
	bufferInfo.ByteWidth = (UINT)data_size;
	bufferInfo.BindFlags = D3D11_BIND_SHADER_RESOURCE;// | D3D11_BIND_UNORDERED_ACCESS;
	bufferInfo.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferInfo.MiscFlags = D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS;
	bufferInfo.StructureByteStride = 0;

	result = device->getNative()->CreateBuffer(&bufferInfo, NULL, (ID3D11Buffer**)&m_buffer);
	if (FAILED(result))
	{
		throw core::OutOfMemoryException(); // TODO: Handle this better.
		m_buffer = NULL;
		return gpu::kErrorFormatUnsupported;
	}

	return kError_SUCCESS;
}

//	initAsTextureBuffer( device, format, element_width, element_height ) : Initializes as a typed data buffer. Can be used to load textures.
int	 gpu::Buffer::initAsTextureBuffer ( Device* device, core::gfx::tex::arColorFormat format, const uint64_t element_width, const uint64_t element_height )
{
	ARCORE_ASSERT(element_width > 0 && element_height > 0);
	if (device == NULL) device = getDevice();
	HRESULT result;

	m_bufferType = kBufferTypeTexture;
	m_elementSize = (unsigned int)core::gfx::tex::getColorFormatByteSize(format);
	m_format = ArFormatToGPUFormat(format);

	// Internally, Texture buffers are little different, since DX11 doesn't allow copying from Buffer to Texture.
	// Instead of implementated by a Buffer object, texture buffers are implementated by Texture objects.

	D3D11_TEXTURE2D_DESC		txd;
	txd.Width = (UINT)element_width;
	txd.Height = (UINT)element_height;
	txd.MipLevels = 1;
	txd.ArraySize = 1;
	txd.Format = gpu::internal::ArEnumToDx(format, false, true);
	txd.SampleDesc.Count = 1;
	txd.SampleDesc.Quality = 0;
	txd.Usage = D3D11_USAGE_DYNAMIC;
	txd.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	txd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	txd.MiscFlags = 0; 

	result = device->getNative()->CreateTexture2D(&txd, NULL, (ID3D11Texture2D**)&m_buffer);
	if (FAILED(result))
	{
		throw core::OutOfMemoryException(); // TODO: Handle this better.
		m_buffer = NULL;
		return gpu::kErrorFormatUnsupported;
	}

	return kError_SUCCESS;
}

void* gpu::Buffer::map ( Device* device, const TransferStyle style )
{
	ARCORE_ASSERT(m_buffer != NIL);
	if (device == NULL) device = getDevice();
	HRESULT result;

	D3D11_MAPPED_SUBRESOURCE resource;
	result = device->getNativeContext()->Map((ID3D11Buffer*)m_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource); 
	if (FAILED(result))
	{
		throw core::OutOfMemoryException(); // TODO: Handle this better.
	}

	//todo: correct flags
	return resource.pData;
}
int gpu::Buffer::unmap ( Device* device )
{
	ARCORE_ASSERT(m_buffer != NIL);
	if (device == NULL) device = getDevice();

	device->getNativeContext()->Unmap((ID3D11Buffer*)m_buffer, 0);
	
	return kError_SUCCESS;
}

//	upload( data, data_size, transfer ) : upload a buffer with data
int	gpu::Buffer::upload ( Device* device, void* data, const  uint64_t data_size, const TransferStyle style )
{
	ARCORE_ASSERT(m_buffer != NIL);
	if (device == NULL) device = getDevice();

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
	if (m_buffer)
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