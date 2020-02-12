#include "gpuw/gpuw_common.h"
#ifdef GPU_API_VULKAN

#include "./Device.vk.h"
#include "./Buffers.vk.h"
#include "gpuw/Public/Error.h"
#include "./gpu.h"
#include "core/debug.h"

gpu::Buffer::Buffer ( void ) :
	m_bufferType(kBufferTypeUnknown),
	m_buffer(NULL),
	m_elementSize(0),
	m_bufferSize(0),
	m_format(kFormatUndefined)
{}

//	valid () : is this buffer valid to be used?
bool gpu::Buffer::valid ( void )
{
	return m_buffer != NULL;
}
//	getGlIndex() : returns index of resource in OpenGL
gpuHandle gpu::Buffer::nativePtr ( void )
{
	return (gpuHandle)m_buffer;
}

//	init ( data ) : initializes a constant buffer with data
int	gpu::Buffer::initAsData ( Device* device, const uint64_t data_size )
{
	ARCORE_ASSERT(data_size > 0);

	m_bufferType = kBufferTypeGeneralUse;
	m_format = kFormatUndefined;
	m_bufferSize = data_size;

	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = m_bufferSize;
	bufferInfo.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	vkCreateBuffer((VkDevice)device->getNative(), &bufferInfo, NULL, (VkBuffer*)&m_buffer);

	/*VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	vkAllocateMemory((VkDevice)device->getNative(), &allocInfo, nullptr, (VkDeviceMemory*)&m_bufferMemory);*/
	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements((VkDevice)device->getNative(), (VkBuffer)m_buffer, &memRequirements);
	m_bufferMemory = device->allocateMemory(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, memRequirements.size, 0);

	vkBindBufferMemory((VkDevice)device->getNative(), (VkBuffer)m_buffer, (VkDeviceMemory)m_bufferMemory, 0);

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
	m_bufferSize = m_elementSize * element_count;

	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = m_bufferSize;
	bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	vkCreateBuffer((VkDevice)device->getNative(), &bufferInfo, NULL, (VkBuffer*)&m_buffer);

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements((VkDevice)device->getNative(), (VkBuffer)m_buffer, &memRequirements);
	m_bufferMemory = device->allocateMemory(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, memRequirements.size, 0);

	vkBindBufferMemory((VkDevice)device->getNative(), (VkBuffer)m_buffer, (VkDeviceMemory)m_bufferMemory, 0);

	return kError_SUCCESS;
}

//	initAsIndexBuffer( device, format, element_count ) : Initializes as an index buffer.
int gpu::Buffer::initAsIndexBuffer ( Device* device, IndexFormat format, const uint64_t element_count )
{
	ARCORE_ASSERT(element_count > 0);

	m_bufferType = kBufferTypeIndex;
	m_elementSize = (format == kIndexFormatUnsigned16) ? 2 : 4;
	m_format = (format == kIndexFormatUnsigned16) ? kFormatR16UInteger : kFormatR32UInteger;
	m_bufferSize = m_elementSize * element_count;

	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = m_bufferSize;
	bufferInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	vkCreateBuffer((VkDevice)device->getNative(), &bufferInfo, NULL, (VkBuffer*)&m_buffer);

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements((VkDevice)device->getNative(), (VkBuffer)m_buffer, &memRequirements);
	m_bufferMemory = device->allocateMemory(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, memRequirements.size, 0);

	vkBindBufferMemory((VkDevice)device->getNative(), (VkBuffer)m_buffer, (VkDeviceMemory)m_bufferMemory, 0);

	return kError_SUCCESS;
}

//	initAsData( device, data_size ) : Initializes as a constant buffer.
int gpu::Buffer::initAsConstantBuffer ( Device* device, const uint64_t data_size )
{
	ARCORE_ASSERT(data_size > 0);

	m_bufferType = kBufferTypeConstant;
	m_format = kFormatUndefined;
	m_bufferSize = data_size;

	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = m_bufferSize;
	bufferInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	vkCreateBuffer((VkDevice)device->getNative(), &bufferInfo, NULL, (VkBuffer*)&m_buffer);

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements((VkDevice)device->getNative(), (VkBuffer)m_buffer, &memRequirements);
	m_bufferMemory = device->allocateMemory(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, memRequirements.size, 0);

	vkBindBufferMemory((VkDevice)device->getNative(), (VkBuffer)m_buffer, (VkDeviceMemory)m_bufferMemory, 0);

	return kError_SUCCESS;
}

//	initAsStructuredBuffer( device, data_size ) : Initializes as a data buffer.
// Data is uploaded separately through map/unmap or upload.
int gpu::Buffer::initAsStructuredBuffer ( Device* device, const uint64_t data_size )
{
	ARCORE_ASSERT(data_size > 0);

	m_bufferType = kBufferTypeStructured;
	m_format = kFormatUndefined;
	m_bufferSize = data_size;

	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = m_bufferSize;
	bufferInfo.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	vkCreateBuffer((VkDevice)device->getNative(), &bufferInfo, NULL, (VkBuffer*)&m_buffer);

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements((VkDevice)device->getNative(), (VkBuffer)m_buffer, &memRequirements);
	m_bufferMemory = device->allocateMemory(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, memRequirements.size, 0);

	vkBindBufferMemory((VkDevice)device->getNative(), (VkBuffer)m_buffer, (VkDeviceMemory)m_bufferMemory, 0);

	return kError_SUCCESS;
}

void* gpu::Buffer::map ( Device* device, const TransferStyle style )
{
	ARCORE_ASSERT(m_buffer != NULL);

	void* data = NULL;
	vkMapMemory((VkDevice)device->getNative(), (VkDeviceMemory)m_bufferMemory, 0, m_bufferSize, 0, &data);
	return data;
}
int gpu::Buffer::unmap ( Device* device )
{
	ARCORE_ASSERT(m_buffer != NULL);
	vkUnmapMemory((VkDevice)device->getNative(), (VkDeviceMemory)m_bufferMemory);
	return kError_SUCCESS;
}

//	upload( data, data_size, transfer ) : upload a buffer with data
int	gpu::Buffer::upload ( Device* device, void* data, const  uint64_t data_size, const TransferStyle style )
{
	ARCORE_ASSERT(m_buffer != NULL);

	void* data_target = map(device, style);
		memcpy(data_target, data, data_size);
	unmap(device);

	return 0;
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
	vkFreeMemory((VkDevice)device->getNative(), (VkDeviceMemory)m_bufferMemory, NULL);
	vkDestroyBuffer((VkDevice)device->getNative(), (VkBuffer)m_buffer, NULL);
	m_buffer = NULL;
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