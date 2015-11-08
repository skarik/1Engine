
#ifndef _S_TERRAIN_GRASS_H_
#define _S_TERRAIN_GRASS_H_

// Includes
#include "core/math/Vector3d.h"
#include "after/types/terrain/IndexInformation.h"

namespace Terrain
{
	// Structure
	struct grass_t
	{
		grass_t ( void )
			: block(indexInformation_t())
		{
			;
		}

		Vector3d			position;
		indexInformation_t	block;
		unsigned short		type;

		bool operator== ( const grass_t& right )
		{
			return ( position==right.position && type==right.type );
		}
	};
};


#endif