
#include "core-ext/transform/CTransform.h"
#include "TileMap.h"
#include "render2d/object/TileMapLayer.h"

#include "renderer/debug/CDebugDrawer.h"

#include <algorithm>
#include <thread>


using namespace Engine2D;

TileMap::TileMap ( void )
	: CGameBehavior ( )
{
	m_sprite_file = "";
	m_tileset = NULL;
}
TileMap::~TileMap ( void )
{
	;
}
void TileMap::Update ( void )
{
	; // What does this need to do on update? xD

	/*Debug::Drawer->DrawLine( Vector3d(0,0,0), Vector3d(0,0,32), Color(0,0,1) );
	Debug::Drawer->DrawLine( Vector3d(0,0,0), Vector3d(0,0,-32), Color(0,1,1) );
	for ( uint i = 0; i < m_tiles.size(); ++i )
	{
		Debug::Drawer->DrawLine( Vector3d(0,0,0), Vector3d(m_tiles[i].x,m_tiles[i].y,m_tiles[i].depth) );
	}*/
}


void TileMap::SetSpriteFile ( const char* n_sprite_file )
{
	// Set sprite file
	m_sprite_file = n_sprite_file;
}
void TileMap::SetTileset ( Tileset* tileset_to_use )
{
	// Set tileset info
	m_tileset = tileset_to_use;
}

//===============================================================================================//

void TileMap::SetDebugTileMap ( const uint n_map_w, const uint n_map_h )
{
	// Set tile types
	if ( m_tileset->tiles.empty() )
	{
		tilesetEntry_t tiletype;

		// Make a tile type out of every tile available
		for ( int iy = 0; iy < m_tileset->tilecount_y; ++iy )
		{
			for ( int ix = 0; ix < m_tileset->tilecount_x; ++ix )
			{
				tiletype.type = TILE_DEFAULT;
				tiletype.atlas_x = ix;
				tiletype.atlas_y = iy;
				tiletype.atlas_w = 1;
				tiletype.atlas_h = 1;

				m_tileset->tiles.push_back( tiletype );
			}
		}
	}

	// Build map
	for ( uint ix = 0; ix < n_map_w; ++ix )
	{
		for ( uint iy = 0; iy < n_map_h; ++iy )
		{
			mapTile_t tile;
			tile.type = rand() % ( m_tileset->tilecount_x * m_tileset->tilecount_y );
			tile.x = ix * m_tileset->tilesize_x;
			tile.y = iy * m_tileset->tilesize_y;
			tile.depth = 0;
			m_tiles.push_back( tile );
		}
	}
}

//===============================================================================================//

void TileMap::Rebuild ( void )
{
	printf( "TileMap::Rebuild called.\n" );

	if ( m_tiles.empty() )
	{
		// No tiles? Then fuck off, yo.
		throw Core::InvalidCallException();
	}
	
	// Sort all the tiles by depth to start with (should be fairly quick)
	struct tileComparator_t {
		bool operator() (const mapTile_t& i,const mapTile_t& j)
		{
			return (i.depth<j.depth);
		}
	} tile_comparator;
	std::sort( m_tiles.begin(), m_tiles.end(), tile_comparator );

	// Keep track of the building threads. Each layer will be a separate mesh, so we have more than enough memory for each call.
	std::vector<std::thread> layer_build_threads;

	// Going to build in layers of depth. Start with the 0th tile
	int current_depth_start_tile = 0;
	int current_depth = m_tiles[current_depth_start_tile].depth;
	// Loop through all the tiles
	for ( uint i = 0; i < m_tiles.size(); ++i )
	{
		// Check if there's a new layer, or if we're at the very end
		if ( current_depth != m_tiles[i].depth || (i == m_tiles.size() - 1) )
		{	// Since we're at a new layer, we should build the previous layer
			// Start the thread for building that layer
			layer_build_threads.push_back( std::thread( &TileMap::RebuildMesh, this, current_depth, current_depth_start_tile, i-current_depth_start_tile+1 ) );
			// And set the options for the next layer to build
			current_depth_start_tile = i; // Next layer starts at the current tile
			current_depth = m_tiles[i].depth; // Layer depth is as given
		}
	}

	// Wait for all the threads to finish
	for ( uint i = 0; i < layer_build_threads.size(); ++i )
	{
		// Tell the program to stop until the thread has finished
		layer_build_threads[i].join();
	}

	// Use the stored meshes to make the TileMapLayer renderable objects
	for ( uint i = 0; i < m_meshstorage.size(); ++i )
	{
		printf( " +Creating layer ???\n" );

		Renderer::TileMapLayer* render_layer = new Renderer::TileMapLayer ();
		render_layer->SetSpriteFile( m_sprite_file );
		render_layer->SetLayer( &m_meshstorage[i] );
	}

	printf( " +Rebuild finished.\n" );
}

void TileMap::RebuildMesh ( int layer, int start_offset, int predictive_tile_count )
{
	// Start with creating the mesh
	ModelData model;
	model.vertices = new CModelVertex [predictive_tile_count * 4];
	model.triangles = new CModelTriangle [predictive_tile_count * 2];
	model.vertexNum = 0;
	model.triangleNum = 0;

	// Set up constant tables
	const Vector2d position_offsets [4] = {
		Vector2d(0,0),
		Vector2d(1,0),
		Vector2d(0,1),
		Vector2d(1,1),
	};
	const Vector2d tile_size_reference = Vector2d( (Real)m_tileset->tilesize_x, (Real)m_tileset->tilesize_y );

	// Loop through the tiles
	for ( uint i = start_offset; i < m_tiles.size(); ++i )
	{
		// Hit the end of the layer yet?
		if ( m_tiles[i].depth != layer ) {
			break; // Yea, we're leaving. Peace out nigga.
		}
		// No? Then let's fucking keep wrecking shit.

		// Link up the triangles first
		model.triangles[model.triangleNum+0].vert[0] = model.vertexNum + 0;
		model.triangles[model.triangleNum+0].vert[1] = model.vertexNum + 1;
		model.triangles[model.triangleNum+0].vert[2] = model.vertexNum + 2;

		model.triangles[model.triangleNum+1].vert[0] = model.vertexNum + 2;
		model.triangles[model.triangleNum+1].vert[1] = model.vertexNum + 1;
		model.triangles[model.triangleNum+1].vert[2] = model.vertexNum + 3;

		model.triangleNum += 2;

		// Grab the tile type for creating the mesh
		const tilesetEntry_t tile_type = m_tileset->tiles[m_tiles[i].type];

		// Create the 4 vertices
		for ( int v = 0; v < 4; ++v )
		{
			// Set positions
			model.vertices[model.vertexNum+v].x = ((Real)m_tiles[i].x) + position_offsets[v].x * tile_type.atlas_w * m_tileset->tilesize_x;
			model.vertices[model.vertexNum+v].y = ((Real)m_tiles[i].y) + position_offsets[v].y * tile_type.atlas_h * m_tileset->tilesize_y;
			model.vertices[model.vertexNum+v].z = (Real)layer;

			// Set up the UV's
			model.vertices[model.vertexNum+v].u = ((tile_type.atlas_x + position_offsets[v].x * tile_type.atlas_w) * m_tileset->tilesize_x) / (Real)m_tileset->atlassize_x;
			model.vertices[model.vertexNum+v].v = ((tile_type.atlas_y + position_offsets[v].y * tile_type.atlas_h) * m_tileset->tilesize_y) / (Real)m_tileset->atlassize_y;

			// Set up other values
			model.vertices[model.vertexNum+v].r = 1.0F;
			model.vertices[model.vertexNum+v].g = 1.0F;
			model.vertices[model.vertexNum+v].b = 1.0F;
			model.vertices[model.vertexNum+v].a = 1.0F;
		}

		model.vertexNum += 4;
	}

	// Lock the result storage and copy our mesh pointers to it
	m_mut_meshstorage.lock();
	m_meshstorage.push_back( model );

	// Unlock the storage
	m_mut_meshstorage.unlock();
}