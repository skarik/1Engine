#include <algorithm>
#include <thread>
#include <map>

#include "core/math/Math.h"
#include "core/math/Math3d.h"

#include "TileMap.h"

#include "core/system/io/CBinaryFile.h"
#include "core-ext/system/io/Resources.h"
#include "core-ext/system/io/osf.h"
#include "core-ext/transform/Transform.h"

#include "renderer/debug/RrDebugDrawer.h"

#include "render2d/object/TileMapLayer.h"

using namespace Engine2D;

const uint32_t STATEFLAG_ACTIVE			= 0x0001;
const uint32_t STATEFLAG_WANTS_REBUILD	= 0x0002;

TileMap::TileMap ( void )
	: CGameBehavior ( ), m_state_flags(STATEFLAG_ACTIVE)
{
	m_sprite_file = "";
	m_tileset = NULL;
}
TileMap::~TileMap ( void )
{
	// Clear out the mesh data
	for ( uint i = 0; i < m_meshstorage.size(); ++i)
	{
		delete[] m_meshstorage[i].position;
		delete[] m_meshstorage[i].color;
		delete[] m_meshstorage[i].texcoord0;
		delete[] m_meshstorage[i].indices;
	}
	// Clear out the render layers that are currently there
	for ( uint i = 0; i < m_render_layers.size(); ++i )
	{
		delete m_render_layers[i];
	}
	m_render_layers.clear();
}
void TileMap::Update ( void )
{
	; // What does this need to do on update? xD

	//debug::Drawer->DrawLine( Vector3f(0,0,0), Vector3f(0,0,32), Color(0,0,1) );
	//debug::Drawer->DrawLine( Vector3f(0,0,0), Vector3f(0,0,-32), Color(0,1,1) );
	//for ( uint i = 0; i < m_tiles.size(); ++i )
	//{
	//	debug::Drawer->DrawLine( Vector3f(0,0,0), Vector3f(m_tiles[i].x,m_tiles[i].y,m_tiles[i].depth) );
	//}
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

template <typename Integer>
void _split_to_integer ( char * buffer, Integer& outIntegerX, Integer& outIntegerY )
{
	// Split into two numbers
	size_t offset = 0;
	while ( offset < strlen(buffer) ) if ( isspace(buffer[offset++]) ) break;
	buffer[offset-1] = '\0'; // Split the string
	// Convert the string to values
	outIntegerX = atoi( buffer );
	outIntegerY = atoi( buffer + offset );
}

//		SetTilesetFile ( cstring tileset_file )
// Sets the tileset file.
//	n_tileset_file			- resource path to the tileset info file
// Internally calls SetSpriteFile and SetTileset. Both sets of needed data are specified by the tileset file
void TileMap::SetTilesetFile ( const char* n_tileset_file )
{
	// Create a new tileset
	if ( m_tileset == NULL ) {
		m_tileset = new Engine2D::Tileset ();
	}

	// Clear out existing data:
	m_tileset->tiles.clear();

	// Save filename
	m_tileset_file = n_tileset_file;

	// Open the resources
	FILE* p_tilsetfile = core::Resources::Open( n_tileset_file, "rb" );
	CBinaryFile tileset_file ( p_tilsetfile ); // Will close the file when exits scope
	io::OSFReader loader ( p_tilsetfile ); // Create the OSF loader and begin on the merry excursion through the tileset file

	io::OSFEntryInfo entry;
	do
	{
		loader.GetNext( entry );
		if ( entry.type == io::kOSFEntryTypeObject )
		{
			arstring<64> base_key ( entry.name );
			if ( base_key.compare("sprite") )
			{
				loader.GoInto( entry );
				// Load in the sprite keyvalue
				do
				{
					loader.GetNext( entry );
					// Grab all the entry
					if ( entry.type == io::kOSFEntryTypeNormal )
					{
						arstring<64> key ( entry.name );
						if ( key.compare("file") )
						{	// Got a value, set it
							SetSpriteFile( entry.value );
						}
					}
				}
				while ( entry.type != io::kOSFEntryTypeEnd );
			}
			else if ( base_key.compare("atlas") )
			{
				loader.GoInto( entry );
				// Load in the sprite keyvalue
				do
				{
					loader.GetNext( entry );
					// Grab all the entry
					if ( entry.type == io::kOSFEntryTypeNormal )
					{
						arstring<64> key ( entry.name );
						// Load in the full size of the sprite
						if ( key.compare("size") )
						{	// Split into two numbers
							_split_to_integer( entry.value, m_tileset->atlassize_x, m_tileset->atlassize_y );
						}
						// Load in the count of shit
						else if ( key.compare("count") )
						{	// Split into two numbers
							_split_to_integer( entry.value, m_tileset->tilecount_x, m_tileset->tilecount_y );
						}
						// Load in the tile size
						else if ( key.compare("tilesize") )
						{	// Split into two numbers
							_split_to_integer( entry.value, m_tileset->tilesize_x, m_tileset->tilesize_y );
						}
					}
				}
				while ( entry.type != io::kOSFEntryTypeEnd );
			}
			else if ( base_key.compare("tile") )
			{
				loader.GoInto( entry );
				// Create new tiletype
				tilesetEntry_t tiletype = {};
				// Set default values
				tiletype.atlas_x = 0;
				tiletype.atlas_y = 0;
				tiletype.atlas_w = 1;
				tiletype.atlas_h = 1;
				tiletype.type = TILE_DEFAULT;
				tiletype.collision = kTileCollisionTypeNone;
				// Load in the tile object
				do
				{
					loader.GetNext( entry );
					// Grab all the regular entries
					if ( entry.type == io::kOSFEntryTypeNormal )
					{
						arstring<64> key ( entry.name );
						// Set new tile type
						if ( key.compare("type") )
						{
							if ( strcmp(entry.value,"simple") == 0 )
								tiletype.type = TILE_DEFAULT;
							else if ( strcmp(entry.value,"autotile") == 0 )
								tiletype.type = TILE_AUTOTILE;
							else if ( strcmp(entry.value,"autowall") == 0 )
								tiletype.type = TILE_AUTOWALL;
							else if ( strcmp(entry.value,"randomized") == 0 )
								tiletype.type = TILE_RANDOMIZED;
							else if ( strcmp(entry.value,"prop") == 0 )
								tiletype.type = TILE_PROP;
						}
						// Set new tile coordinates
						else if ( key.compare("atlas") )
						{
							// Split into two numbers
							_split_to_integer( entry.value, tiletype.atlas_x, tiletype.atlas_y );
						}
						// Set new tile size
						else if ( key.compare("size") )
						{
							// Split into two numbers
							_split_to_integer( entry.value, tiletype.atlas_w, tiletype.atlas_h );
						}
						// Modify wanted collision for the tileset
						else if ( key.compare("collision") )
						{
							if ( strcmp(entry.value,"impassable") == 0 || strcmp(entry.value,"impassible") == 0 )
								tiletype.collision = kTileCollisionTypeImpassable;
						}
						// End normal entry check
					}
					// Grab all the compound entries
					else if ( entry.type == io::kOSFEntryTypeObject )
					{
						arstring<64> key ( entry.name );
						// Autotile entry? Need to set autotile information
						if ( key.compare("autotile") )
						{
							// Start by setting all autotile information to 'zero'
							for ( int i = 0; i < MAPTILE_AUTOTILE_SIZE; ++i ) {
								tiletype.autotile_0[i] = 255;
								tiletype.autotile_1[i] = 255;
							}
							// autotile_N[4] is the properties for the default tile. It should always be set

							// Regardless, we now go into the entry
							loader.GoInto( entry );
							do
							{
								loader.GetNext( entry );
								// Grab all the entries available
								if ( entry.type == io::kOSFEntryTypeNormal )
								{
									key = entry.name;
									uint entry_lookup = (uint)atoi(key); // Convert the name into the index in the table
									if ( entry_lookup < 16 ) {
										// Load the entry into the given table index
										_split_to_integer( entry.value, tiletype.autotile_0[entry_lookup], tiletype.autotile_1[entry_lookup] );
									}
								}
							}
							while ( entry.type != io::kOSFEntryTypeEnd );

							// End by setting all unused autotile information to autotile4
							for ( int i = 0; i < 16; ++i ) {
								if ( tiletype.autotile_0[i] == 255 || tiletype.autotile_1[i] == 255 ) {
									tiletype.autotile_0[i] = tiletype.autotile_0[4];
									tiletype.autotile_1[i] = tiletype.autotile_1[4];
								}
							}
						}
						// Randomized entry? Need to set autotile information
						else if ( key.compare("choices") )
						{
							// start by setting tile count to zero
							tiletype.autotile_0[0] = 0;
							tiletype.autotile_1[0] = 0;

							// Go into the entry
							loader.GoInto( entry );
							do
							{
								loader.GetNext( entry );
								// Grab all the entries available
								if ( entry.type == io::kOSFEntryTypeNormal )
								{
									key = entry.name;
									uint entry_count = (uint)atoi(key); // Convert the name into the duplicates count
									uint8_t nx, ny;
									_split_to_integer( entry.value, nx, ny );
									for ( uint count = 0; count < entry_count; ++count )
									{	// Load the entry into the given table index X amount of times
										tiletype.autotile_0[++tiletype.autotile_0[0]] = nx;
										tiletype.autotile_1[++tiletype.autotile_1[0]] = ny;
									}
								}
							}
							while ( entry.type != io::kOSFEntryTypeEnd );
						}
						// Prop entry? Need to load in the pieces for the prop
						else if ( key.compare("pieces") )
						{
							// Start by setting all autotile information to 'zero'
							for ( int i = 0; i < MAPTILE_AUTOTILE_SIZE; ++i ) {
								tiletype.autotile_0[i] = 255;
								tiletype.autotile_1[i] = 255;
							}

							// Go into the entry
							loader.GoInto( entry );
							do
							{
								loader.GetNext( entry );
								// Grab all the entries available
								if ( entry.type == io::kOSFEntryTypeNormal )
								{
									key = entry.name;
									uint entry_lookup = (uint)atoi(key); // Convert the name into the index in the table
									if ( entry_lookup < 16 ) {
										// Load the entry into the given table index
										_split_to_integer( entry.value, tiletype.autotile_0[entry_lookup], tiletype.autotile_1[entry_lookup] );
									}
								}
							}
							while ( entry.type != io::kOSFEntryTypeEnd );
						}
						// End compound entry check
					}
					// End loader
				}
				while ( entry.type != io::kOSFEntryTypeEnd );
				m_tileset->tiles.push_back( tiletype );
			}
		}
	}
	while ( entry.type != io::kOSFEntryTypeEoF );
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
	if ( m_tiles.empty() )
	{
		for ( uint ix = 0; ix < n_map_w; ++ix )
		{
			for ( uint iy = 0; iy < n_map_h; ++iy )
			{
				mapTile_t tile = {0};
				tile.type = 0;//rand() % ( m_tileset->tiles.size() );
				tile.x = ix * m_tileset->tilesize_x;
				tile.y = iy * m_tileset->tilesize_y;
				tile.depth = 0;
				m_tiles.push_back( tile );
			}
		}
	}
}

//===============================================================================================//

void TileMap::Rebuild ( void )
{
	// Check state first
	if ( (m_state_flags & STATEFLAG_ACTIVE) == 0 )
	{
		m_state_flags |= STATEFLAG_WANTS_REBUILD;
		return;
	}

	if ( m_tiles.empty() )
	{
		// Clear out the render layers that are currently there
		for ( uint i = 0; i < m_render_layers.size(); ++i )
		{
			delete m_render_layers[i];
		}
		m_render_layers.clear();
		// No tiles? Then fuck off, yo.
		throw core::InvalidCallException();
	}

	// Clear out the meshes storage
	for ( uint i = 0; i < m_meshstorage.size(); ++i )
	{
		delete[] m_meshstorage[i].position;
		delete[] m_meshstorage[i].color;
		delete[] m_meshstorage[i].texcoord0;
		delete[] m_meshstorage[i].indices;
	}
	m_meshstorage.clear();
	m_meshstorage_layer.clear();
	
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
			int predictive_tile_count = (i-current_depth_start_tile+1)*4;
			layer_build_threads.push_back( std::thread( &TileMap::RebuildMesh, this, current_depth, current_depth_start_tile, predictive_tile_count ) );
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

	// Clear out the render layers that are currently there
	for ( uint i = 0; i < m_render_layers.size(); ++i )
	{
		delete m_render_layers[i];
	}
	m_render_layers.clear();

	// Use the stored meshes to make the TileMapLayer renderable objects
	m_render_layers.resize( m_meshstorage.size(), NULL );
	for ( uint i = 0; i < m_meshstorage.size(); ++i )
	{
		renderer::TileMapLayer* render_layer = new renderer::TileMapLayer (); // This needs to be stored and deleted.
		render_layer->SetSpriteFile( m_sprite_file, NULL ); // TODO: Use the previous layer's sprite file.
		render_layer->SetLayer( &m_meshstorage[i] );
		render_layer->source_layer_id = m_meshstorage_layer[i];

		m_render_layers.push_back( render_layer );
	}
}

void TileMap::RebuildMesh ( int layer, int start_offset, int predictive_tile_count )
{
	// Start with creating the mesh
	arModelData model;
	model.position = new Vector3f [predictive_tile_count * 4];
	model.color = new Vector4f [predictive_tile_count * 4];
	model.texcoord0 = new Vector3f [predictive_tile_count * 4];
	model.indices = new uint16_t [predictive_tile_count * 5];
	model.vertexNum = 0;
	model.indexNum = 0;

	// Set up constant tables
	const Vector2f position_offsets [4] = {
		Vector2f(0,0),
		Vector2f(1,0),
		Vector2f(0,1),
		Vector2f(1,1),
	};
	const Vector2f tile_size_reference = Vector2f( (Real)m_tileset->tilesize_x, (Real)m_tileset->tilesize_y );

#	define FULL_SPEED_AHEAD
#	ifdef  FULL_SPEED_AHEAD
	std::map<Vector2i,uint32_t> tiletype_map;
	// Store a quick lookup of the map to speed up the autotile system
	for ( uint i = start_offset; i < m_tiles.size(); ++i )
	{
		// Hit the end of the layer yet?
		if ( m_tiles[i].depth != layer ) {
			break; // Yea, we're leaving. Peace out nigga.
		}
		// The tile needs lookup data? Then we store it.
		const tilesetEntry_t tile_type = m_tileset->tiles[m_tiles[i].type];
		if ( tile_type.type == TILE_RANDOMIZED || tile_type.type == TILE_DEFAULT || 
		     tile_type.type == TILE_AUTOTILE || tile_type.type == TILE_AUTOWALL )
		{
			tiletype_map[Vector2i(m_tiles[i].x,m_tiles[i].y)] = m_tiles[i].type;
		}
	}
#	endif

	// Loop through the tiles
	for ( uint i = start_offset; i < m_tiles.size(); ++i )
	{
		// Hit the end of the layer yet?
		if ( m_tiles[i].depth != layer ) {
			break; // Yea, we're leaving. Peace out nigga.
		}
		// No? Then let's fucking keep wrecking shit.

		// Grab the tile type for creating the mesh
		const tilesetEntry_t tile_type = m_tileset->tiles[m_tiles[i].type];

		// Do different types of meshes based on the tile type
		if ( tile_type.type == TILE_RANDOMIZED || tile_type.type == TILE_DEFAULT )
		{
			// Link up the triangles first
			model.indices[model.indexNum + 0] = model.vertexNum + 0;
			model.indices[model.indexNum + 1] = model.vertexNum + 1;
			model.indices[model.indexNum + 2] = model.vertexNum + 2;
			model.indices[model.indexNum + 3] = model.vertexNum + 3;
			model.indices[model.indexNum + 4] = 0xFFFF;

			model.indexNum += 5;

			// Create the 4 vertices
			for ( int v = 0; v < 4; ++v )
			{
				// Set positions
				model.position[model.vertexNum+v].x = ((Real)m_tiles[i].x) + position_offsets[v].x * tile_type.atlas_w * m_tileset->tilesize_x;
				model.position[model.vertexNum+v].y = ((Real)m_tiles[i].y) + position_offsets[v].y * tile_type.atlas_h * m_tileset->tilesize_y;
				model.position[model.vertexNum+v].z = (Real)layer;

				// Set up other values
				model.color[model.vertexNum+v] = Vector4f(1.0F, 1.0F, 1.0F, 1.0F);
			}

			// Set the vertex UVs
			if ( tile_type.type == TILE_RANDOMIZED )
			{
				uint rindex = (rand() % tile_type.autotile_0[0]) + 1;
				for ( int v = 0; v < 4; ++v ) {
					model.texcoord0[model.vertexNum+v].x = ((tile_type.autotile_0[rindex] + position_offsets[v].x * tile_type.atlas_w) * m_tileset->tilesize_x) / (Real)m_tileset->atlassize_x;
					model.texcoord0[model.vertexNum+v].y = ((tile_type.autotile_1[rindex] + position_offsets[v].y * tile_type.atlas_h) * m_tileset->tilesize_y) / (Real)m_tileset->atlassize_y;
				}
			}
			else
			{
				for ( int v = 0; v < 4; ++v ) {
					model.texcoord0[model.vertexNum+v].x = ((tile_type.atlas_x + position_offsets[v].x * tile_type.atlas_w) * m_tileset->tilesize_x) / (Real)m_tileset->atlassize_x;
					model.texcoord0[model.vertexNum+v].y = ((tile_type.atlas_y + position_offsets[v].y * tile_type.atlas_h) * m_tileset->tilesize_y) / (Real)m_tileset->atlassize_y;
				}
			}

			model.vertexNum += 4;
		}
		else if ( tile_type.type == TILE_PROP )
		{
			for ( int o = 0; o < 9; ++o )
			{
				// Check for valid tile set
				if ( tile_type.autotile_0[o] != 255 )
				{
					// Link up the triangles first
					model.indices[model.indexNum + 0] = model.vertexNum + 0;
					model.indices[model.indexNum + 1] = model.vertexNum + 1;
					model.indices[model.indexNum + 2] = model.vertexNum + 2;
					model.indices[model.indexNum + 3] = model.vertexNum + 3;
					model.indices[model.indexNum + 4] = 0xFFFF;

					model.indexNum += 5;

					int offset_y = o % 3;
					int offset_x = o / 3;

					// Create the 4 vertices
					for ( int v = 0; v < 4; ++v )
					{
						// Set positions
						model.position[model.vertexNum+v].x = ((Real)m_tiles[i].x) + (position_offsets[v].x + (Real)offset_x) * tile_type.atlas_w * m_tileset->tilesize_x;
						model.position[model.vertexNum+v].y = ((Real)m_tiles[i].y) + (position_offsets[v].y + (Real)offset_y) * tile_type.atlas_h * m_tileset->tilesize_y;
						model.position[model.vertexNum+v].z = (Real)layer;

						// Set up other values
						model.color[model.vertexNum+v] = Vector4f(1.0F, 1.0F, 1.0F, 1.0F);

						// Set UVs for the vertex
						model.texcoord0[model.vertexNum+v].x = ((tile_type.autotile_0[o] + position_offsets[v].x * tile_type.atlas_w) * m_tileset->tilesize_x) / (Real)m_tileset->atlassize_x;
						model.texcoord0[model.vertexNum+v].y = ((tile_type.autotile_1[o] + position_offsets[v].y * tile_type.atlas_h) * m_tileset->tilesize_y) / (Real)m_tileset->atlassize_y;
					}

					model.vertexNum += 4;
				}
			}
		}
		else if ( tile_type.type == TILE_AUTOTILE || tile_type.type == TILE_AUTOWALL )
		{
			// Declare check space
			uint32_t	bordering [9];
			bool		checkgrid [9];
			Real		offset_top = 0;
			Real		offset_bottom = 0;

			// Lambda: build the border info
			auto border = [&]( int ox, int oy ) -> uint32_t
			{
				int tx = m_tiles[i].x + ox * m_tileset->tilesize_x;
				int ty = m_tiles[i].y + oy * m_tileset->tilesize_y;
#	ifndef FULL_SPEED_AHEAD
				for ( uint j = start_offset; j < m_tiles.size(); ++j )
				{
					// Hit the end of the layer yet?
					if ( m_tiles[j].depth != layer ) {
						break; // Yea, we're leaving. Peace out nigga.
					}
					// Otherwise check the tile
					if ( m_tiles[j].x == tx && m_tiles[j].y == ty ) {
						return m_tiles[j].type; 
					}
				}
#	else //FULL_SPEED_AHEAD
				auto lookupResult = tiletype_map.find(Vector2i(tx,ty));
				if ( lookupResult != tiletype_map.end() )
				{
					return lookupResult->second;
				}
#	endif//FULL_SPEED_AHEAD
				return -1;//m_tiles[i].type;
			};
			// Lambda: check the border for mismatch
			auto check = [&]( int o ) -> bool
			{
				return bordering[o] != bordering[4];
			};

			// build the border data
			if ( tile_type.type == TILE_AUTOTILE )
			{	// Autotile does a normal sample
				bordering[0] = border(-1,-1);
				bordering[1] = border( 0,-1);
				bordering[2] = border(+1,-1);
				bordering[3] = border(-1, 0);
				bordering[4] = m_tiles[i].type;
				bordering[5] = border(+1, 0);
				bordering[6] = border(-1,+1);
				bordering[7] = border( 0,+1);
				bordering[8] = border(+1,+1);
			}
			else if ( tile_type.type == TILE_AUTOWALL )
			{	// Autowall does a slow sample, looking for lowest wall position
				// Get current centered tile
				bordering[4] = m_tiles[i].type;
				// Need to go down to the bottom tile.
				int offset = 0;
				do {
					offset += 1;
					bordering[7] = border( 0,+offset);
				}
				while ( bordering[7] == bordering[4] && offset < 8 );
				offset -= 1;
				// At that offset, do left and right checks
				bordering[0] = border(-1,-1);
				bordering[1] = border( 0,-1);
				bordering[2] = border(+1,-1);
				bordering[3] = border(-1,+offset);
				bordering[5] = border(+1,+offset);
				bordering[6] = border(-1,+1);
				bordering[7] = border( 0,+1);
				bordering[8] = border(+1,+1);
				// Change the offset
				offset_top		= -1.0F * (offset + 1) * m_tileset->tilesize_y;
				offset_bottom	= -1.0F * (offset + 0) * m_tileset->tilesize_y;
			}

			// Build mismatch grid
			for ( int o = 0; o < 9; ++o )
				checkgrid[o] = check(o);

			// Building 4 quads for the single tile to Divide and Conquer.
			// The following are the LUTs for different mesh generation types
			const uint8_t quad_lookup [][4] = {
				{0,1,3,4},
				{1,2,4,5},
				{3,4,6,7},
				{4,5,7,8}
			};
			const uint8_t tri_lookup [][3] = {
				{0,1,3},
				{1,2,5},
				{3,6,7},
				{5,7,8}
			};
			const uint8_t tri_index_lookup [][8] = {
				{4,12,1,1,3,3,0,0},
				{4,1,11,1,5,2,5,2},
				{4,3,10,3,7,6,7,6},
				{4,5,7,8, 9,5,7,8}
			};
			const uint8_t quint_lookup [][5] = {
				{0,1,2,3,6},
				{0,1,2,5,8},
				{0,3,6,7,8},
				{2,5,6,7,8}
			};
			const uint8_t quint_index_lookup [][32] = {	// TODO
				{0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0},
				{0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0},
				{0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0},
				{0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0}
			};

			// Create bitmasks in first pass
			uint8_t bitmask [4] = {0,0,0,0};
			for ( int c = 0; c < 4; ++c )
				for ( int b = 0; b < 3; ++b )
					bitmask[c] |= (checkgrid[tri_lookup[c][b]]) ? (1 << b) : 0;

			// Loop through each corner of the quad
			for ( int c = 0; c < 4; ++c )
			{
				// Link up the triangles first
				model.indices[model.indexNum + 0] = model.vertexNum + 0;
				model.indices[model.indexNum + 1] = model.vertexNum + 1;
				model.indices[model.indexNum + 2] = model.vertexNum + 2;
				model.indices[model.indexNum + 3] = model.vertexNum + 3;
				model.indices[model.indexNum + 4] = 0xFFFF;

				model.indexNum += 5;

				// Do lookup for the corners
				uint rindex = 4;
				rindex = tri_index_lookup[c][bitmask[c]];

				// If bitmask is 4, maybe keep looking through the triangles to find one that isn't 4
				if ( rindex == 4 )
				{
					for ( int b = 0; b < 4; ++b )
					{
						if ( tri_index_lookup[b][bitmask[b]] != 4 )
							rindex = tri_index_lookup[b][bitmask[b]];
					}
				}

				// Create the 4 vertices
				for ( int v = 0; v < 4; ++v )
				{
					// Set positions
					model.position[model.vertexNum+v].x = ((Real)m_tiles[i].x) + (position_offsets[v].x+position_offsets[c].x) * tile_type.atlas_w * m_tileset->tilesize_x * 0.5F;
					model.position[model.vertexNum+v].y = ((Real)m_tiles[i].y) + (position_offsets[v].y+position_offsets[c].y) * tile_type.atlas_h * m_tileset->tilesize_y * 0.5F;
					model.position[model.vertexNum+v].z = (Real)layer + math::lerp((position_offsets[v].y+position_offsets[c].y) * 0.5F, offset_top, offset_bottom);

					// Set up other values
					model.color[model.vertexNum+v] = Vector4f(1.0F, 1.0F, 1.0F, 1.0F);

					// Set up default debug UVs
					model.texcoord0[model.vertexNum+v].x = ((tile_type.autotile_0[rindex] + (position_offsets[v].x+position_offsets[c].x)*0.5F * tile_type.atlas_w) * m_tileset->tilesize_x) / (Real)m_tileset->atlassize_x;
					model.texcoord0[model.vertexNum+v].y = ((tile_type.autotile_1[rindex] + (position_offsets[v].y+position_offsets[c].y)*0.5F * tile_type.atlas_h) * m_tileset->tilesize_y) / (Real)m_tileset->atlassize_y;
				}

				model.vertexNum += 4;
			}
		}
		else
		{
			throw core::NotYetImplementedException();
		}
	};

	// Lock the result storage and copy our mesh pointers to it
	m_mut_meshstorage.lock();
	m_meshstorage.push_back( model );
	m_meshstorage_layer.push_back( layer );

	// Unlock the storage
	m_mut_meshstorage.unlock();
}


//===============================================================================================//

//		ProcessPause ()
// Prevents any additional thread work on this object. Turns into "save requests" mode
void TileMap::ProcessPause ( void )
{
	m_state_flags &= ~STATEFLAG_ACTIVE;
}

//		ProcessResume ()
// Resumes work on this object. Executes any saved requests.
void TileMap::ProcessResume ( void )
{
	// Set active
	m_state_flags |= STATEFLAG_ACTIVE;
	// Check states
	if ( m_state_flags & STATEFLAG_WANTS_REBUILD )
	{
		Rebuild();
	}
	// Reset to active state
	m_state_flags = STATEFLAG_ACTIVE;
}