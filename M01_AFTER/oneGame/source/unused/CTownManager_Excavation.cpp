
#include "CTownManager.h"
#include "CRegionManager.h"

#include "CVoxelTerrain.h"
#include "CTerraGenDefault.h"

#include "random_vector.h"

//	CreateFirstPass
// Runs in the generation side thread of the terrain thread.
// Will create physical objects and blocks for the town in the given position.
void CTownManager::CreateFirstPass ( CBoob* pBoob, const RangeVector& position )
{
	// Need to load all towns needing generation in the surrounding area.
	LoadTowns( position, 7 );

	Vector3d deltaPos;

	// Loop through towns. If in range, then excavate.
	for ( uint i = 0; i < townMap.size(); ++i )
	{
		//if ( fabs((ftype)position.z-townMap[i].entry.position.z) < townMap[i].entry.height )
		//{
			//if ( Vector2d((ftype)position.x-townMap[i].entry.position.x,(ftype)position.y-townMap[i].entry.position.y).sqrMagnitude() < sqr(townMap[i].entry.radius) )
		deltaPos.x = (ftype)position.x-townMap[i].entry.position.x;
		deltaPos.y = (ftype)position.y-townMap[i].entry.position.y;
		deltaPos.z = (ftype)position.z-townMap[i].entry.position.z;
		if ( deltaPos.x < townMap[i].entry.bb_max.x && deltaPos.x > townMap[i].entry.bb_min.x &&
			 deltaPos.y < townMap[i].entry.bb_max.y && deltaPos.y > townMap[i].entry.bb_min.y &&
			 deltaPos.z < townMap[i].entry.bb_max.z && deltaPos.z > townMap[i].entry.bb_min.z )
		{
			// Generate the town here.
			CBinaryFile townFile;
			sPattern townPattern;
			TownOpenFile( townMap[i].entry.position, townFile, false );

			//if ( townFile.IsOpen() )
			{
				TownReadPattern( townFile, &townPattern );
				/*if ( !townPattern.buildings.empty() )
				{
					switch ( townPattern.buildings[0].genstyle )
					{*/
				switch ( townMap[i].entry.townType )
				{
					case World::TOWN_TYPE_OUTDATED0:
					//case 200:
						CreateRoads_1p( townMap[i].entry, townPattern, pBoob, position );
						CreateBuildings_1p( townMap[i].entry, townPattern, pBoob, position );
						break;
					case World::TOWN_TYPE_OUTDATED1:
					//case 199:
						CreateRoads_1p( townMap[i].entry, townPattern, pBoob, position );
						Terrain::terra_t	block;
						block.raw = 0;
						block.block = EB_WOOD;
						block.light = 7;
						for ( uint b = 0; b < townPattern.buildings.size(); ++b ) {
							generator->SB_Cube( pBoob,position,townPattern.buildings[b].box,block );
						}
						break;
					//case 1:
					default:
						CreateRoads_1p( townMap[i].entry, townPattern, pBoob, position );
						// Place instances 
						for ( uint n = 0; n < townPattern.instances.size(); ++n ) {
							ExcavateInstance( townPattern, townPattern.instances[n], pBoob, position );
						}
						// Loop through buildings and place floorplans
						for ( uint b = 0; b < townPattern.buildings.size(); ++b ) {
							ExcavateFloorplan( townPattern, townPattern.buildings[b], pBoob, position );
						}
						break;
				}
					//}
				//}

				//}
			}
		}
	}

	// Loop through roads.
		// Create level area for road OR create road supports.
		// Excavate tunnels.
	// Loop through buildings.
		// Level off area for buildings.
		// Excavate for buildings.
		// Create building supports.
		// Create buildings.
}

//  CreateRoads_1p
// First pass of roads. Creates the initial terrain for roads.
void CTownManager::CreateRoads_1p ( const sTownEntry& townEntry, const sPattern& townPattern, CBoob* pBoob, const RangeVector& position )
{
	Terrain::terra_t	block;
	Ray					actualPos;
	World::eCultureType dominantCulture;
	Vector3d			townCenter;

	// Set block defaults
	block.raw = 0;
	block.block = EB_NONE;
	block.light = 7;
	
	// Get dominant culture
	dominantCulture = World::GetDominantCulture( townPattern.properties.culture );

	// Set town center
	//townCenter = Vector3d( townEntry.position.x+0.5f, townEntry.position.y+0.5f, townEntry.position.z+0.5f );
	townCenter = townPattern.landmarks[0].position;	// 0th landmark is always town center

	// Excavate out the roads
	block.block = EB_NONE;
	for ( uint j = 0; j < townPattern.roads.size(); ++j )
	{
		actualPos = townPattern.roads[j].location;
		actualPos.pos.z += (townPattern.roads[j].size/28.0f)*0.75f;
		generator->SB_Line( pBoob,position, actualPos, townPattern.roads[j].size/28.0f, block );
	}
	// Now, add supports for the roads
	for ( uint j = 0; j < townPattern.roads.size(); ++j )
	{
		actualPos = townPattern.roads[j].location;

		switch ( townPattern.roads[j].terra ) {
			case TER_ISLANDS:
				if ( townPattern.roads[j].elevation < townPattern.roads[j].location.pos.z-0.2f ) {
					block.block = EB_SAND;
				}
				else {
					block.block = EB_DIRT;
				}
				break;
			case TER_DESERT:
				block.block = EB_SAND;
				break;
			default:
				block.block = EB_DIRT;
		}

		if ( townPattern.roads[j].elevation < townPattern.roads[j].location.pos.z-0.6f ) {
			// High off the ground supports
			ftype temp_calc;
			switch ( dominantCulture )
			{
			case World::CULTURE_EUROPEAN:
				// European use stone supports
				block.block = EB_STONE;
				
				actualPos.pos.z -= 1.3f/32.0f;
				generator->SB_RoadLine( pBoob,position, actualPos, townPattern.roads[j].size/60.0f, townPattern.roads[j].size/32.0f, block );
				actualPos.pos.z -= townPattern.roads[j].size/32.0f;

				// They have supports that go straight down here and there
				if ( j%5 == 0 ) {
					actualPos.dir.x = 0;
					actualPos.dir.y = 0;
					actualPos.dir.z = (townPattern.roads[j].elevation-0.2f - actualPos.pos.z);
					generator->SB_Line( pBoob,position, actualPos, townPattern.roads[j].size/72.0f, block );
				}
				break;
			case World::CULTURE_ASIAN:
				// Asian use wood supports
				block.block = EB_WOOD;

				actualPos.pos.z -= 1.3f/32.0f;
				//generator->SB_Line( pBoob,position, actualPos, townPattern.roads[j].size/32.0f, block );
				temp_calc = std::min<ftype>( townPattern.roads[j].size/64.0f, 2.3f/32.0f );
				generator->SB_RoadLine( pBoob,position, actualPos, townPattern.roads[j].size/60.0f, temp_calc, block );
				
				actualPos.pos.z -= temp_calc;

				// They have thinner supports that go to the center or go down here and there
				if ( j%6 == 0 ) {
					if ( j%18 == 0 ) {
						actualPos.dir.x = 0;
						actualPos.dir.y = 0;
						actualPos.dir.z = (townPattern.roads[j].elevation-0.2f - actualPos.pos.z);
						generator->SB_Line( pBoob,position, actualPos, townPattern.roads[j].size/72.0f, block );
					}
					else {
						actualPos.dir = townCenter - actualPos.pos;
						actualPos.dir.z += ((townPattern.roads[j].elevation-0.2f - actualPos.pos.z)-actualPos.dir.z)*0.2f - 0.5f;
						ftype mag = actualPos.dir.magnitude();
						if ( mag > 2.0f ) {
							actualPos.dir = actualPos.dir.normal() * ((mag*0.25f)+1.5f);
						}
						generator->SB_Line( pBoob,position, actualPos, townPattern.roads[j].size/84.0f, block );
					}
				}
				break;
			case World::CULTURE_ARCANA:
				// Arcana also use wood supports if sand
				if ( block.block == EB_SAND ) {
					block.block = EB_WOOD;
				}

				actualPos.pos.z -= townPattern.roads[j].size/32.0f;
				generator->SB_Line( pBoob,position, actualPos, townPattern.roads[j].size/32.0f, block );

				// They have a rare support that kind of follows the path down
				if ( j%11 == 0 ) {
					actualPos.dir.normal();
					actualPos.dir.z = (townPattern.roads[j].elevation-0.2f - actualPos.pos.z);
					actualPos.dir.x *= actualPos.dir.z*0.5f;
					actualPos.dir.y *= actualPos.dir.z*0.5f;
					generator->SB_Line( pBoob,position, actualPos, townPattern.roads[j].size/48.0f, block );
				}
				break;
			}//

			// And make it a walkway
			{
				if ( dominantCulture != World::CULTURE_ARCANA )
				{
					if ( block.block == EB_DIRT ) {
						block.block = EB_WOOD;
					}	
				}

				Vector3d tangent = townPattern.roads[j].location.dir.cross( Vector3d::up ).normal();

				actualPos = townPattern.roads[j].location;
				actualPos.pos += tangent*(townPattern.roads[j].size/64.0f);
				actualPos.pos.z += 1.4f/32.0f;
				generator->SB_Ray( pBoob,position, actualPos, block );
				
				actualPos = townPattern.roads[j].location;
				actualPos.pos -= tangent*(townPattern.roads[j].size/64.0f);
				actualPos.pos.z += 1.4f/32.0f;
				generator->SB_Ray( pBoob,position, actualPos, block );
			}
		}
		else {
			// Ground supports
			switch ( dominantCulture )
			{
			case World::CULTURE_EUROPEAN:
				// European use gravel for side of roads
				if ( block.block == EB_SAND ) {
					block.block = EB_GRAVEL;
				}
				break;
			}//
			// Build the external support
			actualPos.pos.z -= townPattern.roads[j].size/64.0f;
			generator->SB_Line( pBoob,position, actualPos, townPattern.roads[j].size/64.0f, block );
		}
	}
	// Excavate out the roads again
	block.block = EB_NONE;
	for ( uint j = 0; j < townPattern.roads.size(); ++j )
	{
		actualPos = townPattern.roads[j].location;
		//actualPos.pos.z += (townPattern.roads[j].size/64.0f)*0.65f;
		actualPos.pos.z += std::max<ftype>( (townPattern.roads[j].size/64.0f)*0.65f, 3.4f/32.0f );
		//generator->SB_Line( pBoob,position, actualPos, townPattern.roads[j].size/64.0f, block );
		generator->SB_RoadLine( pBoob, position, actualPos, townPattern.roads[j].size/96.0f, actualPos.pos.z-townPattern.roads[j].location.pos.z, block );
	}
	// Now, lay down the roads
	for ( uint j = 0; j < townPattern.roads.size(); ++j )
	{
		actualPos = townPattern.roads[j].location;
		//actualPos.pos.z -= 4.0f/32.0f;
		if ( townPattern.roads[j].importance > 2.8f ) {
			block.block = EB_STONEBRICK;
		}
		else if ( townPattern.roads[j].importance > 1.4f ) {
			if ( dominantCulture == World::CULTURE_ASIAN ) {
				block.block = EB_WOOD;
			}
			else {
				block.block = EB_GRAVEL;
			}
		}
		else {
			block.block = EB_DIRT;
		}
		//generator->SB_Line( pBoob,position, actualPos, 4.0f/32.0f, block );
		//generator->SB_Line( pBoob,position, actualPos, 0.63f/32.0f, block );
		//generator->SB_RoadLine( pBoob,position, actualPos, 1.5f/32.0f, 1.7f/32.0f, block );
		generator->SB_RoadLine( pBoob,position, actualPos, townPattern.roads[j].size/64.0f, 1.7f/32.0f, block );
	}
}

//  CreateRoads_1p
// First pass of buildings. Creates the initial terrain for buildings.
void CTownManager::CreateBuildings_1p ( const sTownEntry& townEntry, const sPattern& townPattern, CBoob* pBoob, const RangeVector& position )
{
	Terrain::terra_t	block;

	// Set block defaults
	block.raw = 0;
	block.block = EB_NONE;
	block.light = 7;

	// First create building supports
	for ( uint j = 0; j < townPattern.buildings.size(); ++j )
	{
		Maths::Cubic actualCube = townPattern.buildings[j].box;
		if ( townPattern.buildings[j].box.position.z-0.5f > townPattern.buildings[j].elevation )
		{
			block.block = EB_WOOD;
			actualCube.size.z = 1.0f/32.0f;
			actualCube.position.z -= actualCube.size.z;

			generator->SB_Cube( pBoob,position,actualCube,block );
		}
		else
		{
			block.block = EB_DIRT;
			actualCube.size.z = std::max<ftype>( actualCube.position.z-townPattern.buildings[j].elevation, 3.0f/32.0f );
			actualCube.position.z -= actualCube.size.z;
			generator->SB_Cube( pBoob,position,actualCube,block );
		}
	}

	// straight creation of buildings
	for ( uint j = 0; j < townPattern.buildings.size(); ++j )
	{
		Maths::Cubic actualCube = townPattern.buildings[j].box;

		// Create basic building shell
		if ( townPattern.buildings[j].box.position.z-0.5f > townPattern.buildings[j].elevation )
		{
			actualCube.size.x -= 4.0f/32.0f;
			actualCube.size.y -= 4.0f/32.0f;
			actualCube.position.x += 2.0f/32.0f;
			actualCube.position.y += 2.0f/32.0f;
			actualCube.size.z += 1.0f/32.0f;
			actualCube.position.z -= 1.0f/32.0f;
		}
		else {
			actualCube.size.x -= 2.0f/32.0f;
			actualCube.size.y -= 2.0f/32.0f;
			actualCube.position.x += 1.0f/32.0f;
			actualCube.position.y += 1.0f/32.0f;
			actualCube.size.z += 1.0f/32.0f;
			actualCube.position.z -= 1.0f/32.0f;
		}
		block.block = EB_WOOD;
		generator->SB_Cube( pBoob,position, actualCube, block );

		// Excavate building inside
		block.block = EB_NONE;
		actualCube.size.x -= 2.0f/32.0f;
		actualCube.size.y -= 2.0f/32.0f;
		actualCube.position.x += 1.0f/32.0f;
		actualCube.position.y += 1.0f/32.0f;
		
		actualCube.position.z += 1.0f/32.0f;
		actualCube.size.z -= 2.0f/32.0f;

		generator->SB_Cube( pBoob,position, actualCube, block );

		// Excavate building doorway
		Ray doorwayRay ( townPattern.buildings[j].entrance+townPattern.buildings[j].entrance_dir*0.5f, -townPattern.buildings[j].entrance_dir*1.5f );

		doorwayRay.pos.z += 2.3f/32.0f;
		generator->SB_RoadLine( pBoob,position, doorwayRay,0.4f/32.0f,2.3f/32.0f, block );
	}


}



//	ExcavateFloorplan
// Excavates floorplan of given building
void CTownManager::ExcavateFloorplan ( const CTownManager::sPattern& townPattern, const CTownManager::sPatternBuilding& buildingEntry, CBoob* pBoob, const RangeVector& position )
{
	// Currently only loads in floorplan for building 0

	// Load in the building.
	//string fname = ".res/terra/floorplans/" + buildingEntry.floorplans[0].name + ".png";
	arstring<256> fname ( ".res/terra/floorplans/" );
	fname += buildingEntry.floorplans[0].name;
	fname += ".png";
	ImageToFloorPlan( fname );

	// Loop through the building floor plan and place blocks
	Vector3d floorplanPosition;
	Terrain::terra_t block;
	block.raw = 0;
	block.light = 7; // need to load and place components instead
	/*for ( uint x = 0; x < floor_plan_width; ++x ) {
		for ( uint y = 0; y < floor_plan_height; ++y ) {
			//generator->SB_Line
			floorplanPosition.x = buildingEntry.box.position.x + x/32.0f;
			floorplanPosition.y = buildingEntry.box.position.y + y/32.0f;
			floorplanPosition.z = buildingEntry.box.position.z;
			switch ( floor_plan_buffer[x+y*floor_plan_width] ) {
			case World::eFloorplanValue::FLR_InteriorFlooring:
				block.block = EB_WOOD;
				generator->SB_Line( pBoob,position, Ray(floorplanPosition,Vector3d(0,0,0.01f)), 0.01f, block );
				break;
			case World::eFloorplanValue::FLR_WallExternal:
				block.block = EB_STONEBRICK;
				generator->SB_Line( pBoob,position, Ray(floorplanPosition,Vector3d(0,0,0.14f)), 0.01f, block );
				break;
			case World::eFloorplanValue::FLR_WallInterior:
				block.block = EB_WOOD;
				generator->SB_Line( pBoob,position, Ray(floorplanPosition,Vector3d(0,0,0.14f)), 0.01f, block );
				break;
			case World::eFloorplanValue::FLR_InteriorTable:
				block.block = EB_WOOD;
				generator->SB_Line( pBoob,position, Ray(floorplanPosition,Vector3d(0,0,0.09f)), 0.01f, block );
				break;
			}
		}
	}*/


	random_vector<World::eCultureType> cultureSampler;
	// Push back all cultures with town probabilities
	/*for ( uint i = 0; i < 3; ++i ) {
		cultureSampler.push_back( (World::eCultureType)i, sqr(townPattern.properties.culture[i]) );
	}*/
	//cultureSampler.push_back( World::CULTURE_EUROPEAN, 1.0 );
	cultureSampler.push_back( World::CULTURE_EUROPEAN, 0.001f+townPattern.properties.culture[0] );
	cultureSampler.push_back( World::CULTURE_ASIAN, 0.001f+townPattern.properties.culture[1] );

	// First, generate general values.
	int iWallHeight, iRoofOffset, x, y, z;
	iWallHeight = (int)GetCultureWallHeight( cultureSampler.get_target_value(GetExcavationFloorplanResult(buildingEntry,1)) );

	// Excavate area
	for ( int i = 0; i < iWallHeight+1; ++i ) {
		component_plan_buffer[i].raw = 0;
		component_plan_buffer[i].light = 7;
		component_plan_buffer[i].block = EB_NONE;
	}
	component_plan_x = 1;	component_plan_y = 1;	component_plan_z = iWallHeight+1;
	for ( x = 0; x < floor_plan_width; ++x ) {
		for ( y = 0; y < floor_plan_height; ++y ) {
			if ( (World::eFloorplanValue)floor_plan_buffer[x+y*floor_plan_width] != World::eFloorplanValue::FLR_None ) {
				floorplanPosition.x = buildingEntry.box.position.x + x/32.0f;
				floorplanPosition.y = buildingEntry.box.position.y + y/32.0f;
				floorplanPosition.z = buildingEntry.box.position.z;
				generator->SB_PasteVXG( pBoob, position, floorplanPosition, RangeVector(component_plan_x,component_plan_y,component_plan_z), component_plan_buffer );
			}
		}
	}

	// Create walls
	LoadComponent( cultureSampler.get_target_value(GetExcavationFloorplanResult(buildingEntry,2)), "wall", GetExcavationRandomInt(buildingEntry,2) );

	for ( x = 0; x < floor_plan_width; ++x ) {
		for ( y = 0; y < floor_plan_height; ++y ) {
			if ( (World::eFloorplanValue)floor_plan_buffer[x+y*floor_plan_width] == World::eFloorplanValue::FLR_WallExternal ) {
				floorplanPosition.x = buildingEntry.box.position.x + x/32.0f;
				floorplanPosition.y = buildingEntry.box.position.y + y/32.0f;
				floorplanPosition.z = buildingEntry.box.position.z;
				generator->SB_PasteVXG( pBoob, position, floorplanPosition, RangeVector(component_plan_x,component_plan_y,std::min<uint>(component_plan_z,iWallHeight)), component_plan_buffer );
			}
		}
	}

	// Create corner walls
	LoadComponent( cultureSampler.get_target_value(GetExcavationFloorplanResult(buildingEntry,3)), "wall", 0 );
	// Copy corner wall over.
	memcpy( component_plan_temp, component_plan_buffer, sizeof(Terrain::terra_t)*component_plan_x*component_plan_y*component_plan_z );
	
	// Loop through floorplan to find corner external walls
	for ( x = 0; x < floor_plan_width; ++x ) {
		for ( y = 0; y < floor_plan_height; ++y ) {
			if ( (World::eFloorplanValue)floor_plan_buffer[x+y*floor_plan_width] == World::eFloorplanValue::FLR_WallExternal ) {
				bool hDir = false, vDir = false;
				if ( (x > 0) && (World::eFloorplanValue)floor_plan_buffer[x-1+y*floor_plan_width] == World::eFloorplanValue::FLR_WallExternal ) {
					hDir = true;
				}
				else if ( (x < floor_plan_width-1) && (World::eFloorplanValue)floor_plan_buffer[x+1+y*floor_plan_width] == World::eFloorplanValue::FLR_WallExternal ) {
					hDir = true;
				}
				if ( hDir ) {
					if ( (y > 0) && (World::eFloorplanValue)floor_plan_buffer[x+(y-1)*floor_plan_width] == World::eFloorplanValue::FLR_WallExternal ) {
						vDir = true;
					}
					else if ( (y < floor_plan_height-1) && (World::eFloorplanValue)floor_plan_buffer[x+(y+1)*floor_plan_width] == World::eFloorplanValue::FLR_WallExternal ) {
						vDir = true;
					}
					if ( vDir ) {
						floorplanPosition.x = buildingEntry.box.position.x + x/32.0f;
						floorplanPosition.y = buildingEntry.box.position.y + y/32.0f;
						floorplanPosition.z = buildingEntry.box.position.z;
						generator->SB_PasteVXG( pBoob, position, floorplanPosition, RangeVector(component_plan_x,component_plan_y,std::min<uint>(component_plan_z,iWallHeight)), component_plan_buffer );
					}
				}
			}
		}
	}

	// Load floors
	LoadComponent( cultureSampler.get_target_value(GetExcavationFloorplanResult(buildingEntry,4)), "floor", GetExcavationRandomInt(buildingEntry,4) );

	// Paste floors
	for ( x = 0; x < floor_plan_width; ++x ) {
		for ( y = 0; y < floor_plan_height; ++y ) {
			if ( (World::eFloorplanValue)floor_plan_buffer[x+y*floor_plan_width] == World::eFloorplanValue::FLR_InteriorFlooring ) {
				floorplanPosition.x = buildingEntry.box.position.x + x/32.0f;
				floorplanPosition.y = buildingEntry.box.position.y + y/32.0f;
				floorplanPosition.z = buildingEntry.box.position.z - 1/32.0f;
				generator->SB_PasteVXGOffset( pBoob, position, floorplanPosition, RangeVector(component_plan_x,component_plan_y,component_plan_z), RangeVector(1,1,1), RangeVector(1-x,1-y,0), component_plan_buffer );
			}
			else if ( (World::eFloorplanValue)floor_plan_buffer[x+y*floor_plan_width] != World::eFloorplanValue::FLR_None ) {
				floorplanPosition.x = buildingEntry.box.position.x + x/32.0f;
				floorplanPosition.y = buildingEntry.box.position.y + y/32.0f;
				floorplanPosition.z = buildingEntry.box.position.z - 1/32.0f;
				generator->SB_PasteVXGOffset( pBoob, position, floorplanPosition, RangeVector(component_plan_x,component_plan_y,component_plan_z), RangeVector(1,1,1), RangeVector(0,0,0), component_plan_buffer );
			}
		}
	}

	// Load roof
	LoadComponent( cultureSampler.get_target_value(GetExcavationFloorplanResult(buildingEntry,5)), "roof", GetExcavationRandomInt(buildingEntry,5) );
	// Get roof offset
	iRoofOffset = (uint)GetCultureRoofOffset( cultureSampler.get_target_value(GetExcavationFloorplanResult(buildingEntry,5)) );

	// Copy roof to temp
	//memcpy( component_plan_temp, component_plan_buffer, sizeof(Terrain::terra_t)*component_plan_x*component_plan_y*component_plan_z );

	// Find some empty spots
	for ( x = 0; x < component_plan_x; ++x ) {
		for ( y = 0; y < component_plan_y; ++y ) {
			for ( z = 0; z < component_plan_z; ++z ) {
				if ( component_plan_buffer[x+y*component_plan_x+z*component_plan_x*component_plan_y].block == EB_NONE ) {
					component_plan_buffer[x+y*component_plan_x+z*component_plan_x*component_plan_y].block = EB_IGNORE;
				}
				else {
					z = component_plan_z;
				}
			}
		}
	}

	// Paste upper roof
	for ( x = 0; x < floor_plan_width; ++x ) {
		for ( y = 0; y < floor_plan_height; ++y ) {
			if (( (World::eFloorplanValue)floor_plan_buffer[x+y*floor_plan_width] == World::eFloorplanValue::FLR_WallExternal )|| 
				( (World::eFloorplanValue)floor_plan_buffer[x+y*floor_plan_width] == World::eFloorplanValue::FLR_DoorwayExternal )) {
				floorplanPosition.x = buildingEntry.box.position.x + x/32.0f;
				floorplanPosition.y = buildingEntry.box.position.y + y/32.0f;
				floorplanPosition.z = buildingEntry.box.position.z + iWallHeight/32.0f;
				generator->SB_PasteVXGOffset( pBoob, position, floorplanPosition, RangeVector(1,1,iWallHeight), RangeVector(1,1,std::min<uint>(component_plan_z,iWallHeight)), RangeVector(0,0,0), component_plan_temp );
			}
		}
	}
	// Paste ceiling
	for ( x = 0; x < floor_plan_width; ++x ) {
		for ( y = 0; y < floor_plan_height; ++y ) {
			if ( (World::eFloorplanValue)floor_plan_buffer[x+y*floor_plan_width] != World::eFloorplanValue::FLR_None ) {
				floorplanPosition.x = buildingEntry.box.position.x + x/32.0f;
				floorplanPosition.y = buildingEntry.box.position.y + y/32.0f;
				floorplanPosition.z = buildingEntry.box.position.z + iWallHeight/32.0f;
				generator->SB_PasteVXGOffset( pBoob, position, floorplanPosition, RangeVector(component_plan_x,component_plan_y,component_plan_z), RangeVector(1,1,component_plan_z), RangeVector(0,0,0), component_plan_buffer );
			}
		}
	}

	// Paste roof, first on right side
	for ( x = 0; x < floor_plan_width; ++x ) {
		for ( y = 0; y < floor_plan_height; ++y ) {
			if ( x > floor_plan_width/2 ) {
				if ( (World::eFloorplanValue)floor_plan_buffer[x+y*floor_plan_width] == World::eFloorplanValue::FLR_WallExternal ) {
					bool vDir = false;
					if ( (y > 0) && (World::eFloorplanValue)floor_plan_buffer[x+(y-1)*floor_plan_width] == World::eFloorplanValue::FLR_WallExternal ) {
						vDir = true;
					}
					else if ( (y < floor_plan_height-1) && (World::eFloorplanValue)floor_plan_buffer[x+(y+1)*floor_plan_width] == World::eFloorplanValue::FLR_WallExternal ) {
						vDir = true;
					}
					if ( vDir )
					{
						floorplanPosition.x = buildingEntry.box.position.x + (x-(component_plan_x-1)+iRoofOffset)/32.0f;
						floorplanPosition.y = buildingEntry.box.position.y + y/32.0f;
						floorplanPosition.z = buildingEntry.box.position.z + iWallHeight/32.0f;
						generator->SB_PasteVXGOffset( pBoob, position, floorplanPosition,
							RangeVector(component_plan_x,component_plan_y,component_plan_z),
							RangeVector(component_plan_x,1,component_plan_z),
							RangeVector(floor_plan_width-component_plan_x-(x-(component_plan_x))-1,0-y,0),
							component_plan_buffer );
						/*if ( y < floor_plan_height/2 ) {
							for ( int yy = 0; yy < component_plan_x; ++yy ) {
								floorplanPosition.y = buildingEntry.box.position.y + (y+yy-1-iRoofOffset)/32.0f;
								floorplanPosition.x = buildingEntry.box.position.x + ((x+(component_plan_x-yy-1))-(component_plan_x-1)+iRoofOffset)/32.0f;
								generator->SB_PasteVXGOffset( pBoob, position, floorplanPosition,
									RangeVector(component_plan_x,component_plan_y,component_plan_z),
									RangeVector(1,1,component_plan_z),
									//RangeVector(floor_plan_width-component_plan_x-((x-(component_plan_x-yy-1))-(component_plan_x))-1,0-(y+yy-1),0),
									//RangeVector(floor_plan_width-component_plan_x-((x-(component_plan_x-yy-1))-(component_plan_x))-1, 0-(y+yy-1), 0),
									RangeVector(floor_plan_width-component_plan_x-(x-component_plan_x)-yy-2, 0-(y+yy-1-iRoofOffset), 0),
									component_plan_buffer );
							}
						}
						else {
							generator->SB_PasteVXGOffset( pBoob, position, floorplanPosition,
								RangeVector(component_plan_x,component_plan_y,component_plan_z),
								RangeVector(component_plan_x,1,component_plan_z),
								RangeVector(floor_plan_width-component_plan_x-(x-(component_plan_x))-1,0-y,0),
								component_plan_buffer );
						}*/
					}
				}
			}
		}
	}
	// Paste roof, on left side
	ComponentFlipX();
	for ( x = 0; x < floor_plan_width; ++x ) {
		for ( y = 0; y < floor_plan_height; ++y ) {
			if ( x < floor_plan_width/2 ) {
				if ( (World::eFloorplanValue)floor_plan_buffer[x+y*floor_plan_width] == World::eFloorplanValue::FLR_WallExternal ) {
					bool vDir = false;
					if ( (y > 0) && (World::eFloorplanValue)floor_plan_buffer[x+(y-1)*floor_plan_width] == World::eFloorplanValue::FLR_WallExternal ) {
						vDir = true;
					}
					else if ( (y < floor_plan_height-1) && (World::eFloorplanValue)floor_plan_buffer[x+(y+1)*floor_plan_width] == World::eFloorplanValue::FLR_WallExternal ) {
						vDir = true;
					}
					if ( vDir )
					{
						floorplanPosition.x = buildingEntry.box.position.x + (signed(x)-signed(iRoofOffset))/32.0f;
						floorplanPosition.y = buildingEntry.box.position.y + y/32.0f;
						floorplanPosition.z = buildingEntry.box.position.z + iWallHeight/32.0f;
						generator->SB_PasteVXGOffset( pBoob, position, floorplanPosition,
							RangeVector(component_plan_x,component_plan_y,component_plan_z),
							RangeVector(component_plan_x,1,component_plan_z),
							RangeVector(component_plan_x-x,0-y,0),
							component_plan_buffer );
					}
				}
			}
		}
	}
	ComponentRotateClockwise();
	// Paste roof, on top side
	for ( x = 0; x < floor_plan_width; ++x ) {
		for ( y = 0; y < floor_plan_height; ++y ) {
			if ( y < floor_plan_width/2 ) {
				if ( (World::eFloorplanValue)floor_plan_buffer[x+y*floor_plan_width] == World::eFloorplanValue::FLR_WallExternal ) {
					bool hDir = false;
					if ( (x > 0) && (World::eFloorplanValue)floor_plan_buffer[x-1+y*floor_plan_width] == World::eFloorplanValue::FLR_WallExternal ) {
						hDir = true;
					}
					else if ( (x < floor_plan_width-1) && (World::eFloorplanValue)floor_plan_buffer[x+1+y*floor_plan_width] == World::eFloorplanValue::FLR_WallExternal ) {
						hDir = true;
					}
					if ( hDir )
					{
						floorplanPosition.x = buildingEntry.box.position.x + x/32.0f;
						floorplanPosition.y = buildingEntry.box.position.y + (signed(y)-signed(iRoofOffset))/32.0f;
						floorplanPosition.z = buildingEntry.box.position.z + iWallHeight/32.0f;
						generator->SB_PasteVXGOffset( pBoob, position, floorplanPosition,
							RangeVector(component_plan_x,component_plan_y,component_plan_z),
							RangeVector(1,component_plan_y,component_plan_z),
							RangeVector(0-x,component_plan_y-y,0),
							component_plan_buffer );
					}
				}
			}
		}
	}
	// Paste roof, on bottom side
	ComponentFlipY();
	for ( x = 0; x < floor_plan_width; ++x ) {
		for ( y = 0; y < floor_plan_height; ++y ) {
			if ( y > floor_plan_width/2 ) {
				if ( (World::eFloorplanValue)floor_plan_buffer[x+y*floor_plan_width] == World::eFloorplanValue::FLR_WallExternal ) {
					bool hDir = false;
					if ( (x > 0) && (World::eFloorplanValue)floor_plan_buffer[x-1+y*floor_plan_width] == World::eFloorplanValue::FLR_WallExternal ) {
						hDir = true;
					}
					else if ( (x < floor_plan_width-1) && (World::eFloorplanValue)floor_plan_buffer[x+1+y*floor_plan_width] == World::eFloorplanValue::FLR_WallExternal ) {
						hDir = true;
					}
					if ( hDir )
					{
						floorplanPosition.x = buildingEntry.box.position.x + x/32.0f;
						floorplanPosition.y = buildingEntry.box.position.y + (y-(component_plan_y-1)+iRoofOffset)/32.0f;
						floorplanPosition.z = buildingEntry.box.position.z + iWallHeight/32.0f;
						generator->SB_PasteVXGOffset( pBoob, position, floorplanPosition,
							RangeVector(component_plan_x,component_plan_y,component_plan_z),
							RangeVector(1,component_plan_y,component_plan_z),
							RangeVector(0-x,floor_plan_height-component_plan_y-(y-(component_plan_y))-1,0),
							component_plan_buffer );
					}
				}
			}
		}
	}
	// Paste corners
	ComponentFlipY();
	ComponentFlipX();
	ComponentMakeFold();
	for ( x = 0; x < floor_plan_width; ++x ) {
		for ( y = 0; y < floor_plan_height; ++y ) {
			bool hDir = false, vDir = false;
			if ( y < floor_plan_width/2 ) {
				if ( x < floor_plan_height/2 ) { // Top left corner
					if ( (x > 0) && (World::eFloorplanValue)floor_plan_buffer[x-1+y*floor_plan_width] == World::eFloorplanValue::FLR_WallExternal ) {
						hDir = true;
					}
					else if ( (x < floor_plan_width-1) && (World::eFloorplanValue)floor_plan_buffer[x+1+y*floor_plan_width] == World::eFloorplanValue::FLR_WallExternal ) {
						hDir = true;
					}
					if ( hDir ) {
						if ( (y > 0) && (World::eFloorplanValue)floor_plan_buffer[x+(y-1)*floor_plan_width] == World::eFloorplanValue::FLR_WallExternal ) {
							vDir = true;
						}
						else if ( (y < floor_plan_height-1) && (World::eFloorplanValue)floor_plan_buffer[x+(y+1)*floor_plan_width] == World::eFloorplanValue::FLR_WallExternal ) {
							vDir = true;
						}
						if ( vDir ) {
							floorplanPosition.x = buildingEntry.box.position.x + (x-iRoofOffset)/32.0f;
							floorplanPosition.y = buildingEntry.box.position.y + (y-iRoofOffset)/32.0f;
							floorplanPosition.z = buildingEntry.box.position.z + iWallHeight/32.0f;
							generator->SB_PasteVXG( pBoob, position, floorplanPosition, RangeVector(component_plan_x,component_plan_y,component_plan_z), component_plan_buffer );
						}
					}
				}
			}
		}
	}
	ComponentFlipY();
	for ( x = 0; x < floor_plan_width; ++x ) {
		for ( y = 0; y < floor_plan_height; ++y ) {
			bool hDir = false, vDir = false;
			if ( y > floor_plan_width/2 ) {
				if ( x < floor_plan_height/2 ) { // Top left corner
					if ( (x > 0) && (World::eFloorplanValue)floor_plan_buffer[x-1+y*floor_plan_width] == World::eFloorplanValue::FLR_WallExternal ) {
						hDir = true;
					}
					else if ( (x < floor_plan_width-1) && (World::eFloorplanValue)floor_plan_buffer[x+1+y*floor_plan_width] == World::eFloorplanValue::FLR_WallExternal ) {
						hDir = true;
					}
					if ( hDir ) {
						if ( (y > 0) && (World::eFloorplanValue)floor_plan_buffer[x+(y-1)*floor_plan_width] == World::eFloorplanValue::FLR_WallExternal ) {
							vDir = true;
						}
						else if ( (y < floor_plan_height-1) && (World::eFloorplanValue)floor_plan_buffer[x+(y+1)*floor_plan_width] == World::eFloorplanValue::FLR_WallExternal ) {
							vDir = true;
						}
						if ( vDir ) {
							floorplanPosition.x = buildingEntry.box.position.x + (x-iRoofOffset)/32.0f;
							floorplanPosition.y = buildingEntry.box.position.y + (y-component_plan_y+iRoofOffset+1)/32.0f;
							floorplanPosition.z = buildingEntry.box.position.z + iWallHeight/32.0f;
							generator->SB_PasteVXG( pBoob, position, floorplanPosition, RangeVector(component_plan_x,component_plan_y,component_plan_z), component_plan_buffer );
						}
					}
				}
			}
		}
	}
	ComponentFlipX();
	for ( x = 0; x < floor_plan_width; ++x ) {
		for ( y = 0; y < floor_plan_height; ++y ) {
			bool hDir = false, vDir = false;
			if ( y > floor_plan_width/2 ) {
				if ( x > floor_plan_height/2 ) { // Bottom right corner
					if ( (x > 0) && (World::eFloorplanValue)floor_plan_buffer[x-1+y*floor_plan_width] == World::eFloorplanValue::FLR_WallExternal ) {
						hDir = true;
					}
					else if ( (x < floor_plan_width-1) && (World::eFloorplanValue)floor_plan_buffer[x+1+y*floor_plan_width] == World::eFloorplanValue::FLR_WallExternal ) {
						hDir = true;
					}
					if ( hDir ) {
						if ( (y > 0) && (World::eFloorplanValue)floor_plan_buffer[x+(y-1)*floor_plan_width] == World::eFloorplanValue::FLR_WallExternal ) {
							vDir = true;
						}
						else if ( (y < floor_plan_height-1) && (World::eFloorplanValue)floor_plan_buffer[x+(y+1)*floor_plan_width] == World::eFloorplanValue::FLR_WallExternal ) {
							vDir = true;
						}
						if ( vDir ) {
							floorplanPosition.x = buildingEntry.box.position.x + (x-component_plan_x+iRoofOffset+1)/32.0f;
							floorplanPosition.y = buildingEntry.box.position.y + (y-component_plan_y+iRoofOffset+1)/32.0f;
							floorplanPosition.z = buildingEntry.box.position.z + iWallHeight/32.0f;
							generator->SB_PasteVXG( pBoob, position, floorplanPosition, RangeVector(component_plan_x,component_plan_y,component_plan_z), component_plan_buffer );
						}
					}
				}
			}
		}
	}
	ComponentFlipY();
	for ( x = 0; x < floor_plan_width; ++x ) {
		for ( y = 0; y < floor_plan_height; ++y ) {
			bool hDir = false, vDir = false;
			if ( y < floor_plan_width/2 ) {
				if ( x > floor_plan_height/2 ) { // Bottom right corner
					if ( (x > 0) && (World::eFloorplanValue)floor_plan_buffer[x-1+y*floor_plan_width] == World::eFloorplanValue::FLR_WallExternal ) {
						hDir = true;
					}
					else if ( (x < floor_plan_width-1) && (World::eFloorplanValue)floor_plan_buffer[x+1+y*floor_plan_width] == World::eFloorplanValue::FLR_WallExternal ) {
						hDir = true;
					}
					if ( hDir ) {
						if ( (y > 0) && (World::eFloorplanValue)floor_plan_buffer[x+(y-1)*floor_plan_width] == World::eFloorplanValue::FLR_WallExternal ) {
							vDir = true;
						}
						else if ( (y < floor_plan_height-1) && (World::eFloorplanValue)floor_plan_buffer[x+(y+1)*floor_plan_width] == World::eFloorplanValue::FLR_WallExternal ) {
							vDir = true;
						}
						if ( vDir ) {
							floorplanPosition.x = buildingEntry.box.position.x + (x-component_plan_x+iRoofOffset+1)/32.0f;
							floorplanPosition.y = buildingEntry.box.position.y + (y-iRoofOffset)/32.0f;
							floorplanPosition.z = buildingEntry.box.position.z + iWallHeight/32.0f;
							generator->SB_PasteVXG( pBoob, position, floorplanPosition, RangeVector(component_plan_x,component_plan_y,component_plan_z), component_plan_buffer );
						}
					}
				}
			}
		}
	}

	// Load wall and other internals
	LoadComponent( cultureSampler.get_target_value(GetExcavationFloorplanResult(buildingEntry,6)), "wall", GetExcavationRandomInt(buildingEntry,6) );
	// Paste internal walls
	for ( x = 0; x < floor_plan_width; ++x ) {
		for ( y = 0; y < floor_plan_height; ++y ) {
			if ( (World::eFloorplanValue)floor_plan_buffer[x+y*floor_plan_width] == World::eFloorplanValue::FLR_WallInterior ) {
				floorplanPosition.x = buildingEntry.box.position.x + x/32.0f;
				floorplanPosition.y = buildingEntry.box.position.y + y/32.0f;
				floorplanPosition.z = buildingEntry.box.position.z;
				generator->SB_PasteVXG( pBoob, position, floorplanPosition, RangeVector(component_plan_x,component_plan_y,std::min<uint>(component_plan_z,iWallHeight)), component_plan_buffer );
			}
			else if ( (World::eFloorplanValue)floor_plan_buffer[x+y*floor_plan_width] == World::eFloorplanValue::FLR_InteriorTable ) {
				floorplanPosition.x = buildingEntry.box.position.x + x/32.0f;
				floorplanPosition.y = buildingEntry.box.position.y + y/32.0f;
				floorplanPosition.z = buildingEntry.box.position.z;
				generator->SB_PasteVXG( pBoob, position, floorplanPosition, RangeVector(component_plan_x,component_plan_y,std::min<uint>(component_plan_z,2)), component_plan_buffer );
			}
		}
	}

	// Load doorway
	LoadComponent( cultureSampler.get_target_value(GetExcavationFloorplanResult(buildingEntry,7)), "doorway", GetExcavationRandomInt(buildingEntry,7) );

	// Paste all doorways on X
	for ( x = 0; x < floor_plan_width; ++x ) {
		for ( y = 0; y < floor_plan_height; ++y ) {
			if ( (World::eFloorplanValue)floor_plan_buffer[x+y*floor_plan_width] == World::eFloorplanValue::FLR_DoorwayExternal ) {
				if ( (World::eFloorplanValue)floor_plan_buffer[x+1+y*floor_plan_width] == World::eFloorplanValue::FLR_DoorwayExternal ) {
					floorplanPosition.x = buildingEntry.box.position.x + (((signed)x)-(component_plan_x/2)+1)/32.0f;
					floorplanPosition.y = buildingEntry.box.position.y + y/32.0f;
					floorplanPosition.z = buildingEntry.box.position.z - 1/32.0f;
					generator->SB_PasteVXG( pBoob, position, floorplanPosition, RangeVector(component_plan_x,component_plan_y,std::min<uint>(component_plan_z,iWallHeight+1)), component_plan_buffer );
				}
			}
		}
	}
	// Paste all doorways on Y
	ComponentRotateClockwise();
	for ( x = 0; x < floor_plan_width; ++x ) {
		for ( y = 0; y < floor_plan_height; ++y ) {
			if ( (World::eFloorplanValue)floor_plan_buffer[x+y*floor_plan_width] == World::eFloorplanValue::FLR_DoorwayExternal ) {
				if ( (World::eFloorplanValue)floor_plan_buffer[x+(y+1)*floor_plan_width] == World::eFloorplanValue::FLR_DoorwayExternal ) {
					floorplanPosition.x = buildingEntry.box.position.x + x/32.0f;
					floorplanPosition.y = buildingEntry.box.position.y + (((signed)y)-(component_plan_y/2)+1)/32.0f;
					floorplanPosition.z = buildingEntry.box.position.z - 1/32.0f;
					generator->SB_PasteVXG( pBoob, position, floorplanPosition, RangeVector(component_plan_x,component_plan_y,std::min<uint>(component_plan_z,iWallHeight+1)), component_plan_buffer );
				}
			}
		}
	}

	// Load internal doorway
	LoadComponent( cultureSampler.get_target_value(GetExcavationFloorplanResult(buildingEntry,8)), "doorway", GetExcavationRandomInt(buildingEntry,8) );

	// Paste all doorways on X
	for ( x = 0; x < floor_plan_width; ++x ) {
		for ( y = 0; y < floor_plan_height; ++y ) {
			if ( (World::eFloorplanValue)floor_plan_buffer[x+y*floor_plan_width] == World::eFloorplanValue::FLR_DoorwayInterior ) {
				if ( (World::eFloorplanValue)floor_plan_buffer[x+1+y*floor_plan_width] == World::eFloorplanValue::FLR_DoorwayInterior ) {
					floorplanPosition.x = buildingEntry.box.position.x + (((signed)x)-(component_plan_x/2)+1)/32.0f;
					floorplanPosition.y = buildingEntry.box.position.y + y/32.0f;
					floorplanPosition.z = buildingEntry.box.position.z - 1/32.0f;
					generator->SB_PasteVXG( pBoob, position, floorplanPosition, RangeVector(component_plan_x,component_plan_y,std::min<uint>(component_plan_z,iWallHeight+1)), component_plan_buffer );
				}
			}
		}
	}
	// Paste all doorways on Y
	ComponentRotateClockwise();
	for ( x = 0; x < floor_plan_width; ++x ) {
		for ( y = 0; y < floor_plan_height; ++y ) {
			if ( (World::eFloorplanValue)floor_plan_buffer[x+y*floor_plan_width] == World::eFloorplanValue::FLR_DoorwayInterior ) {
				if ( (World::eFloorplanValue)floor_plan_buffer[x+(y+1)*floor_plan_width] == World::eFloorplanValue::FLR_DoorwayInterior ) {
					floorplanPosition.x = buildingEntry.box.position.x + x/32.0f;
					floorplanPosition.y = buildingEntry.box.position.y + (((signed)y)-(component_plan_y/2)+1)/32.0f;
					floorplanPosition.z = buildingEntry.box.position.z - 1/32.0f;
					generator->SB_PasteVXG( pBoob, position, floorplanPosition, RangeVector(component_plan_x,component_plan_y,std::min<uint>(component_plan_z,iWallHeight+1)), component_plan_buffer );
				}
			}
		}
	}

}

ftype CTownManager::GetExcavationFloorplanResult ( const sPatternBuilding& buildingEntry, int seed )
{
	return (generator->noise_hhf->Unnormalize(
			generator->noise_hhf->Get3D(buildingEntry.box.position.x-buildingEntry.box.position.z,
										buildingEntry.box.position.y+buildingEntry.box.position.z,
										generator->vMainSamplerOffset.x + 3.12f*seed ) ) + 0.5f );
}

int	CTownManager::GetExcavationRandomInt ( const sPatternBuilding& buildingEntry, int seed )
{
	return -abs((int)(200*generator->noise_hhf->Unnormalize(
		generator->noise_hhf->Get3D(buildingEntry.box.position.x+buildingEntry.box.position.y,
									buildingEntry.box.position.y-buildingEntry.box.position.z,
									buildingEntry.box.position.z+generator->vMainSamplerOffset.y + 5.43f*seed ) ) ) );
}


//  ExcavateInstance
// Excavates given instance
void CTownManager::ExcavateInstance ( const sPattern& townPattern, const sPatternInstance& instanceEntry, CBoob* pBoob, const RangeVector& position )
{
	// Check if a instanced instance
	if ( instanceEntry.category == 0 )
	{
		CTerraGenDefault::TerraInstance terraInst;

		// Set default options to get an instant generation
		terraInst.affectedAreas.push_back( position );
		terraInst.type	= instanceEntry.type;
		terraInst.position		= instanceEntry.position;
		terraInst.startPosition	= position;
		terraInst.hasdowntraced	= true;
		terraInst.mode	= 0;
	
		// Set generation mode
		if ( terraInst.type < NST_SYSTEM_END_PROCEDURAL_LIST ) {
			terraInst.mode = 0;
		}
		else if ( terraInst.type < NST_SYSTEM_END_VXG_LIST ) {
			terraInst.mode = 1;
		}

		// Generate the instance using instance system
		generator->TerraGen_Instances_Generate_Work( &terraInst, pBoob, position );
	}
}