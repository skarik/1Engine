#ifndef _RENDER2D_NORMAL_MAP_GENERATION_H_
#define _RENDER2D_NORMAL_MAP_GENERATION_H_

#include "core/types.h"
#include "core/math/Math3d.h"
#include "renderer/types/pixelFormat.h"

namespace Render2D
{
	namespace Preprocess
	{
		//		GenerateNormalMap
		// Given an image, default normal, and output buffer, generates a default normal map.
		// The normals are pillow-shaded based on distance to the transparent edges.
		RENDER2D_API void GenerateNormalMap (
			const pixel_t* i_pixel_data, pixel_t* o_normal_map, 
			const uint n_image_width, const uint n_image_height,
			const Vector3d& n_default_normal = Vector3d(0,0,1)
		);
	};
};

#endif//_RENDER2D_PALETTE_TO_LUT_H_