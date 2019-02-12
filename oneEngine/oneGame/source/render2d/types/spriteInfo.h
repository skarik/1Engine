//===============================================================================================//
// sprintInfo.h
//
// Contains definitions for individual pixel data storage
//===============================================================================================//
#ifndef _RENDER2D_SPRITE_INFO_H_
#define _RENDER2D_SPRITE_INFO_H_

#include "core/types/types.h"
#include "core/types/float.h"
#include "core/math/vect2d_template.h"

class RrTexture;

struct spriteInfo_t
{
	Vector2i	framesize;
	uint16_t	frames;

	Vector2i	fullsize;

	RrTexture*	tex_sprite;
	RrTexture*	tex_lookup;

public:
	spriteInfo_t ( void )
		: framesize(0,0), frames(0), fullsize(0,0),
		tex_sprite(NULL),
		tex_lookup(NULL)
	{
		;
	}
};


#endif//_RENDER2D_SPRITE_INFO_H_