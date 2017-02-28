#include "renderer/system/glSystem.h"
#include "renderer/system/glMainSystem.h"
#include "renderer/gpuw/Textures.h"

glHandle GPU::TextureAllocate(
	const glEnum textureType,
	const glEnum textureFormat, 
	const uint width, const uint height, const uint depth
)
{
	GL_ACCESS;
	glHandle texture = 0;

	glGenTextures( 1, &texture );

	if ( textureType == Texture2D )
	{
		// Bind the texture object
		glBindTexture( GL_TEXTURE_2D, texture );
		if ( textureFormat > __COLOR_FORMAT_RGB_MIN && textureFormat < __COLOR_FORMAT_RGB_MAX )
			glTexImage2D( GL_TEXTURE_2D, 0, GL.Enum((eColorFormat)textureFormat), width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0 );
		else if ( textureFormat > __COLOR_FORMAT_RGBA_MIN && textureFormat < __COLOR_FORMAT_RGBA_MAX )
			glTexImage2D( GL_TEXTURE_2D, 0, GL.Enum((eColorFormat)textureFormat), width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0 );
		else if ( textureFormat > DepthNone && textureFormat < __DEPTH_FORMAT_MAX )
			glTexImage2D( GL_TEXTURE_2D, 0, GL.Enum((eDepthFormat)textureFormat), width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0 );
		else if ( textureFormat > StencilNone && textureFormat < __STENCIL_FORMAT_MAX )
			glTexImage2D( GL_TEXTURE_2D, 0, GL.Enum((eStencilFormat)textureFormat), width, height, 0, GL_STENCIL_INDEX, GL_UNSIGNED_BYTE, 0 );
	}

	return texture;
}
int GPU::TextureSampleSettings(
	const glEnum textureType,
	const glHandle texture,
	const glEnum repeatX, const glEnum repeatY, const glEnum repeatZ,
	const glEnum sampleMinify, const glEnum sampleMagnify
)
{
	GL_ACCESS;

	if ( textureType == Texture2D )
	{
		// Bind the texture object
		glBindTexture( GL_TEXTURE_2D, texture );
		// Change the texture repeat
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL.Enum((eWrappingType)repeatX) );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL.Enum((eWrappingType)repeatY) );
		// Change the filtering
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL.Enum((eSamplingFilter)sampleMinify) );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL.Enum((eSamplingFilter)sampleMagnify) );
	}

	return 0;
}
int GPU::TextureFree(
	const glHandle texture
)
{
	glHandle tex = texture;
	glDeleteTextures( 1, &tex );

	return 0;
}

// Create a write-only buffer
glHandle GPU::TextureBufferAllocate(
	const glEnum textureType,
	const glEnum textureFormat, 
	const uint width, const uint height, const uint depth
)
{
	GL_ACCESS;
	glHandle buffer = 0;

	glGenRenderbuffers( 1, &buffer );

	if ( textureType == Texture2D )
	{
		glBindRenderbuffer( GL_RENDERBUFFER, buffer );

		if ( textureFormat > ColorNone && textureFormat < __COLOR_FORMAT_MAX )
			glRenderbufferStorage( GL_RENDERBUFFER, GL.Enum((eColorFormat)textureFormat), width, height );
		else if ( textureFormat > DepthNone && textureFormat < __DEPTH_FORMAT_MAX )
			glRenderbufferStorage( GL_RENDERBUFFER, GL.Enum((eDepthFormat)textureFormat), width, height );
		else if ( textureFormat > StencilNone && textureFormat < __STENCIL_FORMAT_MAX )
			glRenderbufferStorage( GL_RENDERBUFFER, GL.Enum((eStencilFormat)textureFormat), width, height );

	}

	return buffer;
}