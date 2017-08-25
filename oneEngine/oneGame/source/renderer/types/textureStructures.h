#ifndef _RENDERER_TEXTURE_STRUCTURES_H_
#define _RENDERER_TEXTURE_STRUCTURES_H_

#include "core/types/types.h"
#include "renderer/types/types.h"
#include "renderer/types/textureFormats.h"

//===============================================================================================//
// Texture info structures
//===============================================================================================//

// Texture info struct
struct tTextureInfo
{	
	glHandle		index;

	eTextureType	type;
	eColorFormat	internalFormat;
	eMipmapGenerationStyle	mipmapStyle;
	eSamplingFilter	filter;

	unsigned int width;
	unsigned int height;
	unsigned int depth;
	unsigned int levels;	// mipmap levels

	eWrappingType repeatX;
	eWrappingType repeatY;
	eWrappingType repeatZ;

	// TODO: Implement:
	eOrientation flipX;
	eOrientation flipY;

	unsigned int userdata;
	void*		userpdata;

	tTextureInfo ( void )
		: index(0)
	{
		;
	}
};
// Texture state struct
struct tTextureState
{
	unsigned int level_base;
	unsigned int level_max;
};

namespace Textures
{
	// IMAGE INFO
	struct timgInfo
	{
		// Image size format information:

		uint16_t	width;
		uint16_t	height;

		uint32_t	internalFormat;

		// Specific for getting and setting animation data:

		uint8_t		framecount;
		uint8_t		xdivs;	
		uint8_t		ydivs;
	};
}

#endif//_RENDERER_TEXTURE_FORMATS_H_