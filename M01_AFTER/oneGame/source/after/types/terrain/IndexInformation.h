
#ifndef _S_TERRAIN_INDEX_INFORMATION_H_
#define _S_TERRAIN_INDEX_INFORMATION_H_

// Includes
#include "core/types/types.h"
//#include "world/BlockTypes.h"
#include "after/types/terrain/BlockData.h"

namespace Terrain
{
	//struct terra_b;
	//union terra_b;

	struct indexInformation_t
	{
		char		x_index;
		char		y_index;
		char		z_index;
		terra_b*	block;

		indexInformation_t ( void ) 
			: x_index(-1), y_index(-1), z_index(-1), block(NULL)
		{
			;
		}

		bool valid ( void ) {
			return x_index != -1 && y_index != -1 && z_index != -1 && block != NULL;
		}
	};
};

#endif
