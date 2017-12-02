#ifndef ENGINE2D_COLLISION_MAP_H_
#define ENGINE2D_COLLISION_MAP_H_

#include <vector>

#include "core/types/ModelData.h"
#include "core/containers/arstring.h"
#include "engine/behavior/CGameBehavior.h"

namespace Engine2D
{
	class TileMap;

	class CollisionMap : public CGameBehavior
	{
	public:
		ENGINE2D_API explicit		CollisionMap ( void );
		ENGINE2D_API 				~CollisionMap ( void );

		//=========================================//

		ENGINE2D_API void			Update ( void );

		//=========================================//

		//		Rebuild ()
		// Rebuilds the meshes used to render the map.
		ENGINE2D_API void			Rebuild ( void );

	public:
		TileMap*				m_tilemap;
		arModelPhysicsData		m_mesh;
	};
}

#endif//ENGINE2D_COLLISION_MAP_H_