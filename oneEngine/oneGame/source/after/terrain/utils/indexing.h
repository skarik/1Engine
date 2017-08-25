
#ifndef _AFTER_TERRAIN_UTILS_INDEXING_H_
#define _AFTER_TERRAIN_UTILS_INDEXING_H_

#include "core/types/types.h"

namespace Terrain
{
	namespace Indexing
	{

		// Convert a Linear array index to a 3D box index
		// Assumes a structure size of 32x32x32
		FORCE_INLINE void LinearToXyz ( const uint index, char& out_x, char& out_y, char& out_z )
		{
			out_x = index % 32;
			out_y = (index / 32) % 32;
			out_z = index / 1024;
		}

		// Convert a 3D box index to a linear array index
		// Assumes a structure size of 32x32x32
		FORCE_INLINE uint XyzToLinear ( const char x, const char y, const char z )
		{
			return x + y*32 + z*1024;
		}

	};
};

#endif//_AFTER_TERRAIN_UTILS_INDEXING_H_