#ifndef CORE_GFX_TEXTURE_STRUCTURES_H_
#define CORE_GFX_TEXTURE_STRUCTURES_H_

#include "core/types/types.h"
#include "core/gfx/textureFormats.h"

namespace core {
namespace gfx {
namespace tex {

	//===============================================================================================//
	// Texture info structures
	//===============================================================================================//

	// Texture info struct
	struct arTextureInfo
	{	
		//glHandle		index;

		arTextureType	type;
		arColorFormat	internalFormat;
		arMipmapGenerationStyle	mipmapStyle;
		arSamplingFilter	filter;

		uint32_t	width;
		uint32_t	height;
		uint32_t	depth;
		uint32_t	levels;	// mipmap levels

		arWrappingType	repeatX;
		arWrappingType	repeatY;
		arWrappingType	repeatZ;

		// TODO: Implement:
		arOrientation	flipX;
		arOrientation	flipY;

		uint32_t	userdata;
		void*		userpdata;

		arTextureInfo ( void )
			//: index(0)
		{
			;
		}
	};

	// Texture state struct
	struct arTextureState
	{
		uint32_t	level_base;
		uint32_t	level_max;
	};

	// IMAGE INFO
	//struct timgInfo
	struct arImageInfo
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

}}}

#endif//CORE_GFX_TEXTURE_STRUCTURES_H_