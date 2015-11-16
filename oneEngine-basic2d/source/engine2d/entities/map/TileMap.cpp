
#include "core-ext/transform/CTransform.h"
#include "TileMap.h"
#include <algorithm>
#include <thread>

using namespace Engine2D;

void TileMap::Rebuild ( void )
{
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
	for ( int i = 0; i < m_tiles.size(); ++i )
	{
		// Check if there's a new layer
		if ( current_depth != m_tiles[i].depth )
		{	// Since we're at a new layer, we should build the previous layer
			// Start the thread for building that layer
			layer_build_threads.push_back( std::thread( &TileMap::RebuildMesh, this, current_depth, current_depth_start_tile, i-current_depth_start_tile+1 ) );
			// And set the options for the next layer to build
			current_depth_start_tile = i; // Next layer starts at the current tile
			current_depth = m_tiles[i].depth; // Layer depth is as given
		}
	}

	// Wait for all the threads to finish
	for ( int i = 0; i < layer_build_threads.size(); ++i )
	{
		// Tell the program to stop until the thread has finished
		layer_build_threads[i].join();
	}
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
	for ( int i = start_offset; i < m_tiles.size(); ++i )
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
			model.vertices[model.vertexNum+v].x = ((Real)m_tiles[i].x) + position_offsets[v].x * tile_type.atlas_w;
			model.vertices[model.vertexNum+v].y = ((Real)m_tiles[i].y) + position_offsets[v].y * tile_type.atlas_h;
			model.vertices[model.vertexNum+v].z = (Real)layer;

			// Set up the UV's
			model.vertices[model.vertexNum+v].u = (tile_type.atlas_x + position_offsets[v].x * tile_type.atlas_w) / (Real)m_tileset->atlassize_x;
			model.vertices[model.vertexNum+v].v = (tile_type.atlas_y + position_offsets[v].y * tile_type.atlas_h) / (Real)m_tileset->atlassize_y;
		}

		model.vertexNum += 4;
	}

	// Lock the result storage and copy our mesh pointers to it

	// Unlock the storage
}