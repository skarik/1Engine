#include "gpuw/Texture.h"
#include "gpuw/Internal/Enums.h"
#include "gpuw/Error.h"
#include "gpuw/Buffers.h"

#include "core/exceptions.h"
#include "core/math/Math.h"

#include "gpuw/ogl/GLCommon.h"

#include <algorithm>


bool gpu::Texture::valid ( void )
{
	return m_texture != 0;
}
gpuHandle gpu::Texture::nativePtr ( void )
{
	return (gpuHandle)m_texture;
}

int gpu::Texture::allocate (
	const core::gfx::tex::arTextureType textureType,
	const core::gfx::tex::arColorFormat textureFormat, 
	const uint width, const uint height, const uint depth, const uint levels
)
{
	uint allocatedLevels = levels;
	if (allocatedLevels == 0) {
		// Generate correct number of mipmaps down to 1x1:
		allocatedLevels = (uint16_t) std::max<int>( 1, math::log2( std::max<uint>(width, std::max<uint>(height, depth)) ) + 1 );
	}

	GLenum target = GL_INVALID_ENUM;
	target = gpu::internal::ArEnumToGL(textureType);

	if (m_texture == 0)
	{
		m_type = textureType;
		// Create texture
		glCreateTextures(target, 1, &m_texture);
		if (m_texture != 0)
		{
			// Allocate storage
			switch (textureType)
			{
			case core::gfx::tex::kTextureType1D:
				glTextureStorage1D(m_texture, allocatedLevels, gpu::internal::ArEnumToGL(textureFormat), width);
				m_width = width;
				m_height = 1;
				m_depth = 1;
				m_levels = allocatedLevels;
				m_glcomponent = gpu::internal::ArColorFormatToGlComponentType(textureFormat);
				m_gltype = gpu::internal::ArColorFormatToGlDataType(textureFormat);
				break;
			case core::gfx::tex::kTextureType1DArray:
			case core::gfx::tex::kTextureType2D:
			case core::gfx::tex::kTextureTypeCube:
				glTextureStorage2D(m_texture, allocatedLevels, gpu::internal::ArEnumToGL(textureFormat), width, height);
				m_width = width;
				m_height = height;
				m_depth = 1;
				m_levels = allocatedLevels;
				m_glcomponent = gpu::internal::ArColorFormatToGlComponentType(textureFormat);
				m_gltype = gpu::internal::ArColorFormatToGlDataType(textureFormat);
				break;
			case core::gfx::tex::kTextureType2DArray:
			case core::gfx::tex::kTextureType3D:
			case core::gfx::tex::kTextureTypeCubeArray:
				glTextureStorage3D(m_texture, allocatedLevels, gpu::internal::ArEnumToGL(textureFormat), width, height, depth);
				m_width = width;
				m_height = height;
				m_depth = depth;
				m_levels = allocatedLevels;
				m_glcomponent = gpu::internal::ArColorFormatToGlComponentType(textureFormat);
				m_gltype = gpu::internal::ArColorFormatToGlDataType(textureFormat);
				break;
			}
		}
		else
		{
			throw core::OutOfMemoryException(); // TODO: Handle this better.
			return gpu::kErrorOutOfMemory;
		}
	}

	// Lot of things can go wrong with textures, doesn't hurt to check status here.
	ARCORE_ASSERT(glGetError() == GLenum(0));

	return gpu::kError_SUCCESS;
}

//	free() : destroys any allocated texture, if existing.
int gpu::Texture::free ( void )
{
	glDeleteTextures(1, &m_texture);
	m_texture = 0;

	return gpu::kError_SUCCESS;
}

int gpu::Texture::upload ( gpu::Buffer& buffer, const uint level )
{
	ARCORE_ASSERT(glGetError() == GLenum(0));

	const uint level_divisor = (1 << level);
	GLsizei upload_width, upload_height;

	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, (GLuint)buffer.nativePtr());
	switch (m_type)
	{
	case core::gfx::tex::kTextureType1D:
		ARCORE_ERROR("Not implemented");
		break;
	case core::gfx::tex::kTextureType1DArray:
	case core::gfx::tex::kTextureType2D:
	case core::gfx::tex::kTextureTypeCube:
		upload_width  = std::max<GLsizei>(1, m_width / level_divisor);
		upload_height = std::max<GLsizei>(1, m_height / level_divisor);
		glTextureSubImage2D(m_texture, level, 0, 0, upload_width, upload_height, m_glcomponent, m_gltype, NULL);
		break;
	case core::gfx::tex::kTextureType2DArray:
	case core::gfx::tex::kTextureType3D:
	case core::gfx::tex::kTextureTypeCubeArray:
		ARCORE_ERROR("Not implemented");
		break;
	}
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

	// Lot of things can go wrong with textures, and this is a slow process here. It doesn't hurt to check status here.
	ARCORE_ASSERT(glGetError() == GLenum(0));

	return gpu::kError_SUCCESS;
}


bool gpu::WOFrameAttachment::valid ( void )
{
	return m_texture != 0;
}
gpuHandle gpu::WOFrameAttachment::nativePtr ( void )
{
	return (gpuHandle)m_texture;
}

int gpu::WOFrameAttachment::allocate (
	const core::gfx::tex::arTextureType textureType,
	const core::gfx::tex::arColorFormat textureFormat, 
	const uint width, const uint height, const uint depth, const uint levels
)
{
	if (textureType != core::gfx::tex::kTextureType2D)
	{
		throw core::InvalidArgumentException(); // TODO: Handle this better.
		return gpu::kErrorBadArgument;
	}

	if (m_texture == 0)
	{
		m_type = textureType;
		// Create texture
		glCreateRenderbuffers(1, &m_texture);
		if (m_texture != 0)
		{
			// Allocate storage:
			glNamedRenderbufferStorage(m_texture, gpu::internal::ArEnumToGL(textureFormat), width, height);
		}
		else
		{
			throw core::OutOfMemoryException(); // TODO: Handle this better.
			return gpu::kErrorOutOfMemory;
		}
	}
	return gpu::kError_SUCCESS;
}

//	free() : destroys any allocated texture, if existing.
int gpu::WOFrameAttachment::free ( void )
{
	glDeleteRenderbuffers(1, &m_texture);
	m_texture = 0;

	return gpu::kError_SUCCESS;
}