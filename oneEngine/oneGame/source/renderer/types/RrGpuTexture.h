#ifndef _GL_TEXTURE_STRUCTURE_H_
#define _GL_TEXTURE_STRUCTURE_H_

#include "core/types/types.h"
#include "renderer/types/types.h"
#include "renderer/types/textureFormats.h"

struct RrGpuTexture
{
	glHandle	texture;
	glEnum		format;

	RrGpuTexture( void ) : texture(0), format(0) {}
	RrGpuTexture( const glHandle _texture, const glEnum _format )
		: texture(_texture), format(_format)
	{
		;
	}
};

// make a function/class/whatever to generate a color, depth, or stencil texture

#endif