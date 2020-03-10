#ifndef M04_COLLISION_MAP_RENDERER_H_
#define M04_COLLISION_MAP_RENDERER_H_

#include "renderer/object/CStreamedRenderable3D.h"

namespace Engine2D
{
	class TileMap;
	class Tileset;
	class CollisionMap;
}

namespace M04
{
	class CollisionMapRenderer : public CStreamedRenderable3D
	{
	public:
		explicit				CollisionMapRenderer ( void );
								~CollisionMapRenderer ( void );

		//	EndRender() : Post-render work.
		// Used to recreate the rendered mesh at the last minute.
		bool					EndRender ( void ) override;

		bool					Render ( const rrRenderParams* params ) override;

	public:
		Engine2D::CollisionMap*
							m_collision;
		bool				m_drawSolids;
		bool				m_drawWireframe;

	private:
		uint32_t			m_max_vertex;
		uint32_t			m_max_index;
	};
};

#endif//M04_COLLISION_MAP_RENDERER_H_