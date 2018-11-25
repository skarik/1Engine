//#include "renderer/system/glSystem.h"
//#include "renderer/system/glMainSystem.h"
#include "renderer/gpuw/Textures.h"
#include "renderer/gpuw/Internal/Enums.h"
#include "renderer/gpuw/Error.h"
#include "renderer/gpuw/Buffers.h"

#include "core/exceptions.h"
#include "core/math/Math.h"

#include "renderer/ogl/GLCommon.h"

#include <algorithm>

//gpu::Texture::Texture ( void )
//{
//	m_texture = 0;
//	m_type = core::gfx::tex::kTextureTypeNone;
//}
//gpu::Texture::~Texture ( void )
//{
//	// Free texture on death
//	if (m_texture != 0)
//	{
//		glDeleteTextures(1, &m_texture);
//	}
//}

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
				m_glformat = gpu::internal::ArEnumToGL(textureFormat);
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
				m_glformat = gpu::internal::ArEnumToGL(textureFormat);
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
				m_glformat = gpu::internal::ArEnumToGL(textureFormat);
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
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, buffer.nativePtr());
	glTextureSubImage2D(m_texture, level, 0, 0, m_width, m_height, m_glformat, m_gltype, NULL);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, 0);

	return gpu::kError_SUCCESS;
}

//gpu::WOFrameAttachment::WOFrameAttachment ( void )
//{
//	m_texture = 0;
//	m_type = core::gfx::tex::kTextureTypeNone;
//}
//gpu::WOFrameAttachment::~WOFrameAttachment ( void )
//{
//	// Free texture on death
//	if (m_texture != 0)
//	{
//		glDeleteRenderbuffers(1, &m_texture);
//	}
//}

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
//
//
//glHandle gpu::TextureAllocate(
//	const glEnum textureType,
//	const glEnum textureFormat, 
//	const uint width, const uint height, const uint depth
//)
//{
//	GL_ACCESS;
//	glHandle texture = 0;
//
//	glGenTextures( 1, &texture );
//
//	if ( textureType == Texture2D )
//	{
//		// Bind the texture object
//		glBindTexture( GL_TEXTURE_2D, texture );
//		if ( textureFormat > __COLOR_FORMAT_RGB_MIN && textureFormat < __COLOR_FORMAT_RGB_MAX )
//			glTexImage2D( GL_TEXTURE_2D, 0, GL.Enum((eColorFormat)textureFormat), width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0 );
//		else if ( textureFormat > __COLOR_FORMAT_RGBA_MIN && textureFormat < __COLOR_FORMAT_RGBA_MAX )
//			glTexImage2D( GL_TEXTURE_2D, 0, GL.Enum((eColorFormat)textureFormat), width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0 );
//		else if ( textureFormat > DepthNone && textureFormat < __DEPTH_FORMAT_MAX )
//			glTexImage2D( GL_TEXTURE_2D, 0, GL.Enum((eDepthFormat)textureFormat), width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0 );
//		else if ( textureFormat > StencilNone && textureFormat < __STENCIL_FORMAT_MAX )
//			glTexImage2D( GL_TEXTURE_2D, 0, GL.Enum((eStencilFormat)textureFormat), width, height, 0, GL_STENCIL_INDEX, GL_UNSIGNED_BYTE, 0 );
//	}
//
//	return texture;
//}
//int gpu::TextureSampleSettings(
//	const glEnum textureType,
//	const glHandle texture,
//	const glEnum repeatX, const glEnum repeatY, const glEnum repeatZ,
//	const glEnum sampleMinify, const glEnum sampleMagnify
//)
//{
//	GL_ACCESS;
//
//	if ( textureType == Texture2D )
//	{
//		// Bind the texture object
//		glBindTexture( GL_TEXTURE_2D, texture );
//		// Change the texture repeat
//		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL.Enum((eWrappingType)repeatX) );
//		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL.Enum((eWrappingType)repeatY) );
//		// Change the filtering
//		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL.Enum((eSamplingFilter)sampleMinify) );
//		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL.Enum((eSamplingFilter)sampleMagnify) );
//	}
//
//	return 0;
//}
//int gpu::TextureFree(
//	const glHandle texture
//)
//{
//	glHandle tex = texture;
//	glDeleteTextures( 1, &tex );
//
//	return 0;
//}
//
//// Create a write-only buffer
//glHandle gpu::TextureBufferAllocate(
//	const glEnum textureType,
//	const glEnum textureFormat, 
//	const uint width, const uint height, const uint depth
//)
//{
//	GL_ACCESS;
//	glHandle buffer = 0;
//
//	glGenRenderbuffers( 1, &buffer );
//
//	if ( textureType == Texture2D )
//	{
//		glBindRenderbuffer( GL_RENDERBUFFER, buffer );
//
//		if ( textureFormat > ColorNone && textureFormat < __COLOR_FORMAT_MAX )
//			glRenderbufferStorage( GL_RENDERBUFFER, GL.Enum((eColorFormat)textureFormat), width, height );
//		else if ( textureFormat > DepthNone && textureFormat < __DEPTH_FORMAT_MAX )
//			glRenderbufferStorage( GL_RENDERBUFFER, GL.Enum((eDepthFormat)textureFormat), width, height );
//		else if ( textureFormat > StencilNone && textureFormat < __STENCIL_FORMAT_MAX )
//			glRenderbufferStorage( GL_RENDERBUFFER, GL.Enum((eStencilFormat)textureFormat), width, height );
//
//	}
//
//	return buffer;
//}
//// Free write-only buffer
//int gpu::TextureBufferFree(
//	const glHandle texture
//)
//{
//	glHandle tex = texture;
//	glDeleteRenderbuffers( 1, &tex );
//
//	return 0;
//}