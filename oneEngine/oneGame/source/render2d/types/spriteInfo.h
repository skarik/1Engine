//===============================================================================================//
// spriteInfo.h
//
// Contains definitions for individual pixel data storage
//===============================================================================================//
#ifndef RENDER2D_SPRITE_INFO_H_
#define RENDER2D_SPRITE_INFO_H_

#include "core/types/types.h"
#include "core/types/float.h"
#include "core/math/vect2d_template.h"

class RrTexture;

namespace render2d
{
	struct rrSpriteInfo
	{
		Vector2i	framesize;
		uint16_t	frames;

		Vector2i	fullsize;

		RrTexture*	tex_sprite;
		RrTexture*	tex_lookup;

		rrSpriteInfo ( void )
			: framesize(0,0), frames(0), fullsize(0,0),
			tex_sprite(NULL),
			tex_lookup(NULL)
			{}
	};
}

#endif//RENDER2D_SPRITE_INFO_H_