
#include "MapIO.h"

#include "core/debug/CDebugConsole.h"

#include "engine2d/entities/map/TileMap.h"
#include "engine2d/entities/Area2DBase.h"
#include "engine2d/entities/AreaTeleport.h"
#include "engine2d/entities/AreaTrigger.h"

#include "m04/states/MapInformation.h"

//		Save ( )
// Saves data to the file, using anything that's not null
void M04::MapIO::Save ( void )
{
	Debug::Console->PrintMessage( "Saving a M04 map file with M04::MapIO...\n" );

	// Start by writing the map header
	{
		mapio_header_t header;
		strncpy( header.signature, "M04p", 4 );
		header.vmajor = 0;
		header.vminor = 0;
		fwrite( &header, sizeof(mapio_header_t), 1, m_file );
		Debug::Console->PrintMessage( "\t+Wrote file header.\n" );
	}

	// Write map information.
	if ( m_mapinfo )
	{
		// Write the topper
		mapio_section_start_t topper;
		strcpy( topper.name, "M04::MapInformation" );
		topper.size = sizeof(M04::MapInformation);
		fwrite( &topper, sizeof(mapio_section_start_t), 1, m_file );
		// Write the map information
		fwrite( m_mapinfo, sizeof(M04::MapInformation), 1, m_file );

		Debug::Console->PrintMessage( "\t+Wrote M04::MapInformation.\n" );
	}

	// Write tilemap information
	if ( m_tilemap )
	{
		// Write the topper
		mapio_section_start_t topper;
		strcpy( topper.name, "Engine2D::TileMap" );
		topper.size = 256 + sizeof(uint32_t) + sizeof(mapTile_t)*m_tilemap->m_tiles.size();
		fwrite( &topper, sizeof(mapio_section_start_t), 1, m_file );

		// Write out the information
		fwrite( m_tilemap->m_tileset_file.c_str(), 256, 1, m_file ); // Write out the tileset file being used
		uint32_t tilecount = m_tilemap->m_tiles.size();
		fwrite( &tilecount, sizeof(uint32_t), 1, m_file );
		for ( uint i = 0; i < tilecount; ++i )
		{
			fwrite( &m_tilemap->m_tiles[i], sizeof(mapTile_t), 1, m_file );
		}

		Debug::Console->PrintMessage( "\t+Wrote Engine2D::TileMap.\n" );
	}

	// Write area information
	if ( m_io_areas )
	{
		for ( auto area : Engine2D::Area2D::Areas() )
		{
			// Write the topper
			string type = area->GetTypeName();

			mapio_section_start_t topper;
			strcpy( topper.name, type.c_str() );

			topper.size = sizeof(Rect) + 128;
			if ( type == "AreaTeleport" )
				topper.size += 257 + sizeof(Color);
			else if ( type == "AreaTrigger" )
				topper.size += 256;

			fwrite( &topper, sizeof(mapio_section_start_t), 1, m_file );

			// Write out the information
			fwrite( &area->m_rect, sizeof(Rect), 1, m_file ); // Write out the rect
			fwrite( area->m_name.c_str(), 128, 1, m_file ); // Write the area's name id
			if ( type == "AreaTeleport" )
			{
				fwrite( ((Engine2D::AreaTeleport*)area)->target_area, 128, 1, m_file );
				fwrite( ((Engine2D::AreaTeleport*)area)->target_room, 128, 1, m_file );
				fwrite( &((Engine2D::AreaTeleport*)area)->type, 1, 1, m_file );
				fwrite( &((Engine2D::AreaTeleport*)area)->fade_color, sizeof(Color), 1, m_file );
			}
			else if ( type == "AreaTrigger" )
			{
				fwrite( ((Engine2D::AreaTrigger*)area)->m_target_actor_type, 128, 1, m_file );
				fwrite( ((Engine2D::AreaTrigger*)area)->m_target_actor_name, 128, 1, m_file );
			}
		}
	}
}
		
//		Load ( )
// Loads data from the file, filling anything that's not null
void M04::MapIO::Load ( void )
{
	Debug::Console->PrintMessage( "Loading a M04 map file with M04::MapIO...\n" );

	// Perform prelim setups
	if ( m_tilemap )
	{
		// clear map
		m_tilemap->m_tiles.clear();
		// clear tileset
		if ( m_tilemap->m_tileset ) delete m_tilemap->m_tileset;
		m_tilemap->m_tileset = NULL;
	}

	// Start by reading the map header
	mapio_header_t header;
	{
		fread( &header, sizeof(mapio_header_t), 1, m_file );
		if ( strncmp( header.signature, "M04p", 4 ) != 0 )
		{
			Debug::Console->PrintError( "\t+Bad M04p signature.\n" );
			return;
		}
		printf( "\t+File version %d.%d\n", header.vmajor, header.vminor );
	}

	// Loop through the file
	while ( !feof( m_file ) )
	{
		mapio_section_start_t topper;
		fread( &topper, sizeof(mapio_section_start_t), 1, m_file );
		if ( strncmp( topper.signature, "$TOP", 4 ) != 0 )
		{
			Debug::Console->PrintError( "\t+Bad M04p topper signature.\n" );
			return;
		}
		// Check for invalid start (it happens since feof isn't triggered until an invalid read)
		if ( feof( m_file ) ) {
			Debug::Console->PrintWarning( "\t+End of file.\n" );
			break;
		}

		// Load in a new section based on the size given
		char* buffer = NULL;
		if ( topper.size > 0 ) {
			buffer = new char[topper.size];
			fread( buffer, 1, topper.size, m_file );
		}

		// Do something with the buffer based on the given information
		printf( "\t+Read in \"%s\"\n", topper.name );

		// Check for M04::MapInformation structure
		if ( strcmp( topper.name, "M04::MapInformation" ) == 0 )
		{
			// Copy the data over
			if ( m_mapinfo ) {
				memcpy( m_mapinfo, buffer, sizeof(M04::MapInformation) );
			}
		}
		// Check for Engine2D::TileMap structure
		if ( strcmp( topper.name, "Engine2D::TileMap" ) == 0 )
		{
			if ( m_tilemap )
			{
				// Load in the name of the file
				memcpy( (void*)m_tilemap->m_tileset_file.c_str(), buffer, 256 );
				// Load in that tileset
				m_tilemap->SetTilesetFile( m_tilemap->m_tileset_file );

				// Grab the number of tiles to load
				uint32_t tilecount = 0;
				memcpy( &tilecount, buffer+256, sizeof(uint32_t) );

				// Set up the reader loop
				size_t offset = 256 + sizeof(uint32_t);
				for ( uint i = 0; i < tilecount; ++i )
				{
					mapTile_t tile;
					memcpy( &tile, buffer+offset, sizeof(mapTile_t) );
					offset += sizeof(mapTile_t);
					// Add the new loaded tile to the list
					m_tilemap->m_tiles.push_back(tile);
				}
			}
		}
		// Check for area structures
		if ( strcmp( topper.name, "Area2DBase" ) == 0 )
		{
			if ( m_io_areas )
			{
				Engine2D::Area2DBase* area = new Engine2D::Area2DBase;
				area->RemoveReference();

				memcpy( &area->m_rect, buffer, sizeof(Rect) );
				memcpy( (char*)area->m_name.c_str(), buffer+sizeof(Rect), 128 );
			}
		}
		if ( strcmp( topper.name, "AreaTeleport" ) == 0 )
		{
			if ( m_io_areas )
			{
				Engine2D::AreaTeleport* area = new Engine2D::AreaTeleport;
				area->RemoveReference();

				memcpy( &area->m_rect, buffer, sizeof(Rect) );
				memcpy( (char*)area->m_name.c_str(), buffer+sizeof(Rect), 128 );

				size_t offset = 128 + sizeof(Rect);
				memcpy( &((Engine2D::AreaTeleport*)area)->target_area, buffer+offset, 128 ); offset += 128;
				memcpy( &((Engine2D::AreaTeleport*)area)->target_room, buffer+offset, 128 ); offset += 128;
				memcpy( &((Engine2D::AreaTeleport*)area)->type, buffer+offset, 1 ); offset += 1;
				memcpy( &((Engine2D::AreaTeleport*)area)->fade_color, buffer+offset, sizeof(Color) );
			}
		}
		if ( strcmp( topper.name, "AreaTrigger" ) == 0 )
		{
			if ( m_io_areas )
			{
				Engine2D::AreaTrigger* area = new Engine2D::AreaTrigger;
				area->RemoveReference();

				memcpy( &area->m_rect, buffer, sizeof(Rect) );
				memcpy( (char*)area->m_name.c_str(), buffer+sizeof(Rect), 128 );

				size_t offset = 128 + sizeof(Rect);
				memcpy( &((Engine2D::AreaTrigger*)area)->m_target_actor_type, buffer+offset, 128 ); offset += 128;
				memcpy( &((Engine2D::AreaTrigger*)area)->m_target_actor_name, buffer+offset, 128 );
			}
		}

		// Free the buffer information
		if ( buffer != NULL ) {
			delete [] buffer;
		}
		buffer = NULL;
	}

	// Perform ending cleanup
	if ( m_tilemap ) {
		m_tilemap->Rebuild();
	}
}