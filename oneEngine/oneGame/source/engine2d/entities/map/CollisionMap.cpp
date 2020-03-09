#include "CollisionMap.h"

#include "engine2d/entities/map/TileMap.h"

using namespace Engine2D;

CollisionMap::CollisionMap ( void )
	: CGameBehavior()
{
	m_mesh.indices = NULL;
	m_mesh.position = NULL;
}

CollisionMap::~CollisionMap ( void )
{
	delete[] m_mesh.indices;
	delete[] m_mesh.position;
}

void CollisionMap::Update ( void )
{
	;
}

static ETileCollisionType getCollisionType ( uint16_t collisionOverride, ETileCollisionType originalCollision )
{
	if ( collisionOverride == kTileCollisionOverrideDefault )
	{
		return originalCollision;
	}
	else if ( collisionOverride == kTileCollisionOverrideNoCollision )
	{
		return kTileCollisionTypeNone;
	}
	else if ( collisionOverride == kTileCollisionOverrideCollideBlock )
	{
		return kTileCollisionTypeImpassable;
	}
	return kTileCollisionTypeNone;
}

void CollisionMap::Rebuild ( void )
{
	delete[] m_mesh.indices;
	delete[] m_mesh.position;
	delete[] m_mesh.normal;

	m_mesh.indexNum = 0;
	m_mesh.vertexNum = 0;

	// Predict amount of needed verticles
	for ( uint i = 0; i < m_tilemap->m_tiles.size(); ++i )
	{
		// Grab the tile type for creating the mesh
		const tilesetEntry_t tile_type = m_tilemap->m_tileset->tiles[m_tilemap->m_tiles[i].type];
		// Grab the tile collision type
		ETileCollisionType tile_collision = getCollisionType(m_tilemap->m_tiles[i].collision_override, tile_type.collision);

		// Based on the collision type, add to the triangle count.
		switch (tile_collision)
		{
		case kTileCollisionTypeImpassable:
			m_mesh.indexNum += 6;
			m_mesh.vertexNum += 4;
			break;
		}
	}

	// Skip if no mesh to build
	if (m_mesh.indexNum == 0 || m_mesh.vertexNum == 0)
	{
		m_mesh.indices = NULL;
		m_mesh.position = NULL;
		m_mesh.normal = NULL;
		return;
	}

	// Allocate mesh data
	m_mesh.indices = new uint16_t [m_mesh.indexNum];
	m_mesh.position = new Vector3f [m_mesh.vertexNum];
	m_mesh.normal = new Vector3f [m_mesh.vertexNum];

	// Create the physics mesh

	int indx_count = 0;
	int vert_count = 0;

	// Set up constant tables
	const Vector2f position_offsets [4] = {
		Vector2f(0,0),
		Vector2f(1,0),
		Vector2f(0,1),
		Vector2f(1,1),
	};
	const Vector2f tile_size_reference = Vector2f( (Real)m_tilemap->m_tileset->tilesize_x, (Real)m_tilemap->m_tileset->tilesize_y );

	for ( uint i = 0; i < m_tilemap->m_tiles.size(); ++i )
	{
		// Grab the tile for quick ref
		auto& tile = m_tilemap->m_tiles[i];
		// Grab the tile type for creating the mesh
		const tilesetEntry_t tile_type = m_tilemap->m_tileset->tiles[tile.type];
		// Grab the tile collision type
		ETileCollisionType tile_collision = getCollisionType(tile.collision_override, tile_type.collision);

		// Based on the collision type, add to the triangle count.
		switch (tile_collision)
		{
		case kTileCollisionTypeImpassable:
			{
				// Create the 4 vertices
				for ( int v = 0; v < 4; ++v )
				{
					// Set positions
					m_mesh.position[vert_count+v].x = ((Real)tile.x) + position_offsets[v].x * tile_type.atlas_w * m_tilemap->m_tileset->tilesize_x;
					m_mesh.position[vert_count+v].y = ((Real)tile.y) + position_offsets[v].y * tile_type.atlas_h * m_tilemap->m_tileset->tilesize_y;
					//m_mesh.vertices[vert_count+v].z = (Real)layer;
					m_mesh.position[vert_count+v].z = 0.0F;

					// Set up other values
					m_mesh.normal[vert_count+v] = Vector3f(0,0,0);
				}

				m_mesh.indices[indx_count + 0] = vert_count + 0;
				m_mesh.indices[indx_count + 1] = vert_count + 1;
				m_mesh.indices[indx_count + 2] = vert_count + 2;

				m_mesh.indices[indx_count + 3] = vert_count + 2;
				m_mesh.indices[indx_count + 4] = vert_count + 1;
				m_mesh.indices[indx_count + 5] = vert_count + 3;

				indx_count += 6;
				vert_count += 4;
			}
			break;
		}
	}
}