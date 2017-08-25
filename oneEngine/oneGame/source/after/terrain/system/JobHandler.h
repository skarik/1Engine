
#ifndef _AFTER_TERRAIN_SYSTEM_JOB_HANDLER_
#define _AFTER_TERRAIN_SYSTEM_JOB_HANDLER_

#include "core/types.h"

class CVoxelTerrain;

namespace Terrain
{
	class JobHandler
	{
	public:
		explicit				JobHandler ( CVoxelTerrain* n_terrain );
								~JobHandler ( void );

	};
}

#endif//_AFTER_TERRAIN_SYSTEM_JOB_HANDLER_