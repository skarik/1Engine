#ifndef RENDER2D_NORMAL_MAP_GENERATION_H_
#define RENDER2D_NORMAL_MAP_GENERATION_H_

#include "core/types.h"
#include "core/math/Math3d.h"
#include "core/gfx/pixelFormat.h"

namespace render2d
{
	namespace preprocess
	{
		//	GenerateNormalMap( sprite, o_normal_map, width, height, center_normal )
		// Given an image, default normal, and output buffer, generates a default normal map.
		// The normals are pillow-shaded based on distance to the transparent edges.
		RENDER2D_API void		GenerateNormalMap ( const core::gfx::arPixel* i_pixel_data,
													core::gfx::arPixel* o_normal_map, 
													const uint n_image_width,
													const uint n_image_height,
													const Vector3f& n_default_normal = Vector3f(0,0,1)
													);
	};
};

#endif//RENDER2D_PALETTE_TO_LUT_H_