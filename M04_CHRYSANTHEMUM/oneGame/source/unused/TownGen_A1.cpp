
#include "TownGen_A1.h"

#include "CTownManager.h"
#include "CTerraGenDefault.h"
#include "BlockInfo.h"

#include "Math.h"
#include "Zones.h"
#include "Quadrilateral.h"

void TownGen_A1::Generate ( void )
{
	// Initialize randomizer values
	mseed = 0;

	// Set center position
	townCenterPosition = Vector3d( areaPosition.x+0.5f,areaPosition.y+0.5f,areaElevation-0.1f );

	// Calculate general types
	genTown->properties.verticality = fabs( generator->noise->Get( 12.4f+townCenterPosition.y*135.2f, 12.4f+townCenterPosition.x*135.2f ) ) * 0.1f;
	switch ( areaTerra ) {
	case TER_SPIRES:	genTown->properties.verticality = genTown->properties.verticality*2.0f + 0.7f;	 break;
	case TER_MOUNTAINS:	genTown->properties.verticality = genTown->properties.verticality*2.4f + 0.9f;	 break;
	case TER_FLATLANDS:	genTown->properties.verticality = 0.0f; break;
	case TER_DESERT:	genTown->properties.verticality = 0.0f; break;
	}

	// Create town center
	sPatternLandmark town_center;
	town_center.name = "Town Center";
	town_center.radius = 1.0f;
	town_center.position = townCenterPosition;
	genTown->landmarks.push_back( town_center );
	// And make a clearing there for the hell of it.
	{
		CTownManager::sPatternInstance instance;
		instance.position = townCenterPosition;
		instance.type = NST_CLEARING;
		instance.category = 0;
		genTown->instances.push_back( instance );
	}
	// Make a watchtower for testing
	/*{
		CTownManager::sPatternInstance instance;
		instance.position = townCenterPosition;
		instance.position += Vector3d( RandomNumber(), RandomNumber(), 0 );
		instance.type = NST_VXG_WATCHTOWER0;
		instance.category = 0;
		genTown->instances.push_back( instance );
	}*/

	// Given the town center, start generating buildings
	GenerateBuildings_Positions();
	// Create building floorplans now.
	GenerateBuildings_Floorplans();

	// Create the roads from the building entrances
	GenerateRoad_BuildingEntrances();
	// Create the roads linking buildings together
	//GenerateRoad_ConnectMajor();
	GenerateRoad_FullSequence();

	// At the end, create the bounding box
	CalculateTownBoundingBox();
}
/*
bool TownGen_A1::Chance ( const ftype chance, const int seed )
{
	mseed = 6287*mseed + 997;
	if ( (generator->noise->Unnormalize(
		   generator->noise->Get3D(
			213.2f*(seed+mseed) + (seed^(0x1f23a)) + townCenterPosition.x*(232.2f+seed),
			123.2f*seed + (mseed^(0x1f23a)) + townCenterPosition.y*(423.5f+seed),
			seed*12.1f+genTown->buildings.size()+mseed*526.1f
			)
		   )+0.5f
		  ) < chance )
	{
		return true;
	}
	return false;
}
// Gets a random number from -0.5 to 0.5
ftype TownGen_A1::RandomNumber ( void )
{
	mseed = 6287*mseed + 997;
	return ( generator->noise->Unnormalize(
			  generator->noise_hhf->Get3D(
				mseed*areaPosition.x*324.2f+areaPosition.y+(mseed^0x1241),
				mseed*12.23f + mseed*areaPosition.z*893.7f+(mseed^0xfa221),
				mseed*526.1f + 23.2f + areaPosition.x*42.1f
				)
			)
		);
}
*/

void TownGen_A1::GenerateBuildings_Positions ( void )
{
	// Need certain buildings for a town.
	
	// Chance for a inn to spawn
	if ( Chance( 0.6f, 1 ) ) {
		AddBuilding( World::eBuildingType::BLD_Inn );
	}

	// Chance for a pub to spawn
	if ( Chance( 0.8f, 2 ) ) {
		AddBuilding( World::eBuildingType::BLD_Pub );
	}

	// Chance for a store to spawn
	if ( Chance( 0.9f, 3 ) ) {
		AddBuilding( World::eBuildingType::BLD_Shop );
	}
	// Chance for second store to spawn
	if ( Chance( 0.4f, 4 ) ) {
		AddBuilding( World::eBuildingType::BLD_Shop );
	}

	// Chance for other buildings to spawn
	for ( uint i = 0; i < 5; ++i ) {
		if ( Chance( 0.7f-(i*0.1f) + (( genTown->buildings.size() <= 3 ) ? 0.2f : 0.0f ), 5+i ) ) {
			AddBuilding( World::eBuildingType::BLD_Default );
		}
	}
}

// Function to select a position for a building and check for room
void TownGen_A1::AddBuilding ( const World::eBuildingType buildingType )
{
	sPatternBuilding newBuilding;

	// Based on building type, guess the building bounding box.
	newBuilding.type = (uchar)buildingType;
	switch ( buildingType )
	{
	case World::eBuildingType::BLD_Default:
		newBuilding.box.size = Vector3d( 0.7f,0.7f,0.7f );
		break;
	case World::eBuildingType::BLD_Inn:
		newBuilding.box.size = Vector3d( 1.2f,1.2f,1.2f );
		break;
	case World::eBuildingType::BLD_Pub:
		newBuilding.box.size = Vector3d( 1.2f,1.2f,0.7f );
		break;
	case World::eBuildingType::BLD_Shop:
		newBuilding.box.size = Vector3d( 0.7f,0.7f,1.2f );
		break;
	}

	// Choose a spot that is not near other buildings.
	bool validSpot = false;
	ftype range = 1.0f;
	while ( !validSpot ) {
		range += 0.1f;
		newBuilding.box.position = townCenterPosition + Vector3d( RandomNumber()*range, RandomNumber()*range, 0 );
		// Loop through previous buildings to check for collision
		validSpot = true;
		for ( uint j = 0; j < genTown->buildings.size(); ++j ) {
			if ( newBuilding.box.CubicCollides( genTown->buildings[j].box ) ) {
				validSpot = false;
			}
		}
	}
	
	// Spot chosen, now offset by the verticality.
	newBuilding.box.position.z = generator->TerraGen_1p_GetElevation( newBuilding.box.position );
	newBuilding.box.position.z += genTown->properties.verticality * RandomNumber();

	// Set to debug creation for now
	newBuilding.genstyle = 199;

	// Push back the new buildings
	genTown->buildings.push_back( newBuilding );
}

// Calculates town's bounding box
void TownGen_A1::CalculateTownBoundingBox ( void )
{
	// Look at the current town layout and set the town sizes as needed
	{
		Vector3d deltaStart, deltaEnd;
		ftype sqrMag;

		// First, go through all roads
		for ( uint i = 0; i < genTown->roads.size(); ++i )
		{
			deltaStart = genTown->roads[i].location.pos - townCenterPosition;
			deltaEnd = (genTown->roads[i].location.pos+genTown->roads[i].location.dir) - townCenterPosition;
			// Check differences in height from center to heighten town
			/*if ( ceil(fabs(deltaStart.z)) > genTownEntry->height ) {
				genTownEntry->height = ceil(fabs(deltaStart.z)+1.2f);
			}
			if ( ceil(fabs(deltaEnd.z)) > genTownEntry->height ) {
				genTownEntry->height = ceil(fabs(deltaEnd.z)+1.2f);
			}
			// Check differences in distance from center to widen town
			sqrMag = deltaStart.sqrMagnitude()+0.7f;
			if ( sqrMag > sqr(genTownEntry->radius) ) {
				genTownEntry->radius = sqrt(sqrMag);
			}
			sqrMag = deltaEnd.sqrMagnitude()+0.7f;
			if ( sqrMag > sqr(genTownEntry->radius) ) {
				genTownEntry->radius = sqrt(sqrMag);
			}*/
			if ( deltaStart.x < genTownEntry->bb_min.x ) {
				genTownEntry->bb_min.x = deltaStart.x;
			}
			if ( deltaStart.y < genTownEntry->bb_min.y ) {
				genTownEntry->bb_min.y = deltaStart.y;
			}
			if ( deltaStart.z < genTownEntry->bb_min.z ) {
				genTownEntry->bb_min.z = deltaStart.z;
			}
			if ( deltaEnd.x < genTownEntry->bb_min.x ) {
				genTownEntry->bb_min.x = deltaEnd.x;
			}
			if ( deltaEnd.y < genTownEntry->bb_min.y ) {
				genTownEntry->bb_min.y = deltaEnd.y;
			}
			if ( deltaEnd.z < genTownEntry->bb_min.z ) {
				genTownEntry->bb_min.z = deltaEnd.z;
			}

			if ( deltaStart.x > genTownEntry->bb_max.x ) {
				genTownEntry->bb_max.x = deltaStart.x;
			}
			if ( deltaStart.y > genTownEntry->bb_max.y ) {
				genTownEntry->bb_max.y = deltaStart.y;
			}
			if ( deltaStart.z > genTownEntry->bb_max.z ) {
				genTownEntry->bb_max.z = deltaStart.z;
			}
			if ( deltaEnd.x > genTownEntry->bb_max.x ) {
				genTownEntry->bb_max.x = deltaEnd.x;
			}
			if ( deltaEnd.y > genTownEntry->bb_max.y ) {
				genTownEntry->bb_max.y = deltaEnd.y;
			}
			if ( deltaEnd.z > genTownEntry->bb_max.z ) {
				genTownEntry->bb_max.z = deltaEnd.z;
			}
		}
		// First, go through all roads
		for ( uint i = 0; i < genTown->buildings.size(); ++i )
		{
			deltaStart = genTown->buildings[i].box.position - townCenterPosition;
			deltaEnd = (genTown->buildings[i].box.position+genTown->buildings[i].box.size) - townCenterPosition;
			// Check differences in height from center to heighten town
			/*if ( ceil(fabs(deltaStart.z)) > genTownEntry->height ) {
				genTownEntry->height = ceil(fabs(deltaStart.z)+1.2f);
			}
			if ( ceil(fabs(deltaEnd.z)) > genTownEntry->height ) {
				genTownEntry->height = ceil(fabs(deltaEnd.z)+1.2f);
			}
			// Check differences in distance from center to widen town
			sqrMag = deltaStart.sqrMagnitude()+0.7f;
			if ( sqrMag > sqr(genTownEntry->radius) ) {
				genTownEntry->radius = sqrt(sqrMag);
			}
			sqrMag = deltaEnd.sqrMagnitude()+0.7f;
			if ( sqrMag > sqr(genTownEntry->radius) ) {
				genTownEntry->radius = sqrt(sqrMag);
			}*/
			if ( deltaStart.x < genTownEntry->bb_min.x ) {
				genTownEntry->bb_min.x = deltaStart.x;
			}
			if ( deltaStart.y < genTownEntry->bb_min.y ) {
				genTownEntry->bb_min.y = deltaStart.y;
			}
			if ( deltaStart.z < genTownEntry->bb_min.z ) {
				genTownEntry->bb_min.z = deltaStart.z;
			}
			if ( deltaEnd.x < genTownEntry->bb_min.x ) {
				genTownEntry->bb_min.x = deltaEnd.x;
			}
			if ( deltaEnd.y < genTownEntry->bb_min.y ) {
				genTownEntry->bb_min.y = deltaEnd.y;
			}
			if ( deltaEnd.z < genTownEntry->bb_min.z ) {
				genTownEntry->bb_min.z = deltaEnd.z;
			}

			if ( deltaStart.x > genTownEntry->bb_max.x ) {
				genTownEntry->bb_max.x = deltaStart.x;
			}
			if ( deltaStart.y > genTownEntry->bb_max.y ) {
				genTownEntry->bb_max.y = deltaStart.y;
			}
			if ( deltaStart.z > genTownEntry->bb_max.z ) {
				genTownEntry->bb_max.z = deltaStart.z;
			}
			if ( deltaEnd.x > genTownEntry->bb_max.x ) {
				genTownEntry->bb_max.x = deltaEnd.x;
			}
			if ( deltaEnd.y > genTownEntry->bb_max.y ) {
				genTownEntry->bb_max.y = deltaEnd.y;
			}
			if ( deltaEnd.z > genTownEntry->bb_max.z ) {
				genTownEntry->bb_max.z = deltaEnd.z;
			}
		}

		//genTownEntry->radius += 1.4f;
		genTownEntry->bb_min += Vector3d(-1,-1,-1)*3;
		genTownEntry->bb_max += Vector3d(+1,+1,+1)*3;
	}
}

// Create building floorplans
void TownGen_A1::GenerateBuildings_Floorplans ( void )
{
	// Loop through all the buildings and start making some floorplans
	for ( uint i = 0; i < genTown->buildings.size(); ++i ) 
	{
		CTownManager::sPatternBuilding &building = genTown->buildings[i];
		switch ( building.type )
		{
		case World::eBuildingType::BLD_Pub:
			if ( Chance( 0.5f, 100 ) ) {
				strcpy( building.floorplans[0].name, "eurobar1" );
			}
			else {
				strcpy( building.floorplans[0].name, "eurobar2" );
			}
			building.floorplans[0].posx = 0;
			building.floorplans[0].posy = 0;
			building.floorplans[0].posz = 0;
			building.genstyle = 1;
			break;
		case World::eBuildingType::BLD_Inn:
			strcpy( building.floorplans[0].name, "euroinn1" );
			building.floorplans[0].posx = 0;
			building.floorplans[0].posy = 0;
			building.floorplans[0].posz = 0;
			building.genstyle = 1;
			break;
		case World::eBuildingType::BLD_Shop:
			strcpy( building.floorplans[0].name, "euroshop1" );
			building.floorplans[0].posx = 0;
			building.floorplans[0].posy = 0;
			building.floorplans[0].posz = 0;
			building.genstyle = 1;
			break;
		default:
			strcpy( building.floorplans[0].name, "b1" );
			building.floorplans[0].posx = 0;
			building.floorplans[0].posy = 0;
			building.floorplans[0].posz = 0;
			building.genstyle = 1;
			break;
		}
	}

	// Loop through each building's floorplans to get entrances
	for ( uint i = 0; i < genTown->buildings.size(); ++i ) 
	{
		CTownManager::sPatternBuilding &building = genTown->buildings[i];

		// Load in the building.
		arstring<256> fname ( ".res/terra/floorplans/" );
		fname += building.floorplans[0].name;
		fname += ".png";
		floorplans->ImageToFloorPlan( fname );

		// Rotate floorplan to match the target.

		// Loop through building to find first exit.
		for ( uint x = 0; x < World::ActiveTownManager->floor_plan_width; ++x ) {
			for ( uint y = 0; y < World::ActiveTownManager->floor_plan_height; ++y ) {
				if ( (World::eFloorplanValue)floorplans->floor_plan_buffer[x+y*floorplans->floor_plan_width] == World::eFloorplanValue::FLR_DoorwayExternal ) {
					if ( (World::eFloorplanValue)floorplans->floor_plan_buffer[x+1+y*floorplans->floor_plan_width] == World::eFloorplanValue::FLR_DoorwayExternal ) {
						// Create vertical doorway
						if ( y < floorplans->floor_plan_height/2 ) {
							building.entrance_dir = Vector3d( 0,-1,0 );
						}
						else {
							building.entrance_dir = Vector3d( 0,1,0 );
						}
						// Set doorway position
						building.entrance = building.box.position + Vector3d( (building.floorplans[0].posx+x+1)/32.0f, (building.floorplans[0].posy+y)/32.0f, 0 );
					}
					else if ( (World::eFloorplanValue)floorplans->floor_plan_buffer[x+(y+1)*floorplans->floor_plan_width] == World::eFloorplanValue::FLR_DoorwayExternal ) {
						// Create horizontal doorway
						if ( x < floorplans->floor_plan_width/2 ) {
							building.entrance_dir = Vector3d( -1,0,0 );
						}
						else {
							building.entrance_dir = Vector3d( 1,0,0 );
						}
						// Set doorway position
						building.entrance = building.box.position + Vector3d( (building.floorplans[0].posx+x)/32.0f, (building.floorplans[0].posy+y)/32.0f, 0 );
					}
				}
			}
		}
	}
	// End Entrance Creation

	// Loop through each building's non-external passages to generate more buildings


}

// Looks at all building entrances and makes small little roads
void TownGen_A1::GenerateRoad_BuildingEntrances ( void )
{
	// Loop through each building and make a short road from the entrance
	for ( uint i = 0; i < genTown->buildings.size(); ++i ) 
	{
		CTownManager::sPatternBuilding &building = genTown->buildings[i];

		CTownManager::sPatternRoad newRoad;
		newRoad.location.pos = building.entrance + Vector3d(0,0,-1/32.0f);
		newRoad.location.dir = building.entrance_dir*0.13f;
		newRoad.importance = 2.2f;
		newRoad.size = 3.0f;

		newRoad.elevation = generator->TerraGen_1p_GetElevation( newRoad.location.pos );
		newRoad.terra = generator->iCurrentTerraType;

		genTown->roads.push_back( newRoad );
	}
}

// Looks at current roads, and expands from there.
void TownGen_A1::GenerateRoad_ConnectMajor ( void )
{
	// Loop through the starting roads and link them to the next building.
	for ( uint i = 0; i < genTown->buildings.size(); ++i )
	{
		CTownManager::sPatternRoad &road = genTown->roads[i];
		//CTownManager::sPatternRoad &targetRoad = genTown->roads[i+1];
		Vector3d targetPos;
		if ( i != genTown->buildings.size()-1 ) {
			targetPos = genTown->roads[i+1].location.pos + genTown->roads[i+1].location.dir;
		}
		else {
			targetPos = genTown->roads[0].location.pos + genTown->roads[0].location.dir;
		}

		// So, spread out from current position to the target position
		int roadIndex = 0;
		bool madeTarget = false;
		while ( !madeTarget )
		{
			CTownManager::sPatternRoad newRoad;

			// First road go perpendicular to make a nice porch road.
			if ( roadIndex == 0 ) {
				newRoad.location.pos = road.location.pos + road.location.dir;
				newRoad.location.dir = road.location.dir.cross(Vector3d::up);
				newRoad.location.dir.z = 0;
				newRoad.location.dir.normalize();
				newRoad.location.dir.x = fabs( newRoad.location.dir.x );
				newRoad.location.dir.y = fabs( newRoad.location.dir.y );
				newRoad.location.dir.x *= Math.sgn<ftype>( targetPos.x - newRoad.location.pos.x );
				newRoad.location.dir.y *= Math.sgn<ftype>( targetPos.y - newRoad.location.pos.y );
				newRoad.location.dir *= std::max<ftype>( genTown->buildings[i].box.size.x, genTown->buildings[i].box.size.y )*0.9f;

				roadIndex = (int)((RandomNumber()+0.5f)*2);
			}
			else
			{
				if ( roadIndex%2 == 0 ) {
					// place road on X
					newRoad.location.pos = genTown->roads.back().location.pos+genTown->roads.back().location.dir;
					newRoad.location.dir.x = targetPos.x - newRoad.location.pos.x;
					newRoad.location.dir.y = 0;
					newRoad.location.dir.z = targetPos.z - newRoad.location.pos.z;
				}
				else {
					// place road on Y
					newRoad.location.pos = genTown->roads.back().location.pos+genTown->roads.back().location.dir;
					newRoad.location.dir.x = 0;
					newRoad.location.dir.y = targetPos.y - newRoad.location.pos.y;
					newRoad.location.dir.z = targetPos.z - newRoad.location.pos.z;
				}
				// Check for zero XY movement
				if ( (fabs(newRoad.location.dir.y)+fabs(newRoad.location.dir.x)) < fabs(newRoad.location.dir.z)*0.02f )
				{
					int dif = Math.sgn<ftype>(RandomNumber());
					if ( dif == 0 ) dif = 1;
					if ( roadIndex%2 == 0 ) {
						newRoad.location.dir.x += dif * 0.6f;
					}
					else {
						newRoad.location.dir.y += dif * 0.6f;
					}
				}
				// Limit length
				if ( newRoad.location.dir.magnitude() > 1.0f ) {
					newRoad.location.dir = newRoad.location.dir.normal();
				}
				// Limit Z change
				if ( fabs( newRoad.location.dir.z ) > (fabs( newRoad.location.dir.x )+fabs( newRoad.location.dir.y ))*0.5f ) {
					newRoad.location.dir.z = (fabs( newRoad.location.dir.x )+fabs( newRoad.location.dir.y ))*0.5f * Math.sgn<ftype>(newRoad.location.dir.z );
				}
			}

			// Get road width and such
			newRoad.importance = 2.2f;
			newRoad.size = 3.0f;
			
			// Push back new road
			genTown->roads.push_back( newRoad );

			// Make a road 
			if ( (targetPos-(newRoad.location.pos + newRoad.location.dir)).sqrMagnitude() < FTYPE_PRECISION ) {
				madeTarget = true;
			}
			++roadIndex;
		}
	}
	// 
}

void TownGen_A1::GenerateRoad_FullSequence ( void )
{
	vector<Quadrilateral> l_oversized;
	vector<Quadrilateral> l_allot;

	// Create first set of final polygons to enclose buildings
	for ( int i = 0; i < genTown->buildings.size(); ++i )
	{
		Quadrilateral quad;
		quad[0] = Vector2d( genTown->buildings[i].box.position.x, genTown->buildings[i].box.position.y );
		quad[2] = Vector2d( genTown->buildings[i].box.position.x+genTown->buildings[i].box.size.x, genTown->buildings[i].box.position.y+genTown->buildings[i].box.size.y );
		quad[1] = Vector2d( quad[2].x, quad[0].y );
		quad[3] = Vector2d( quad[0].x, quad[2].y );

		l_allot.push_back( quad );
	}

	// Create the full town plot to cover 1.5x the area the town is in
	{
		Vector2d minPos = Vector2d( townCenterPosition.x, townCenterPosition.y );
		Vector2d maxPos = minPos;
		for ( int i = 0; i < l_allot.size(); ++i )
		{
			for ( int c = 0; c < 4; ++c ) {
				if ( l_allot[i][c].x < minPos.x ) {
					minPos.x = l_allot[i][c].x;
				}
				if ( l_allot[i][c].y < minPos.y ) {
					minPos.y = l_allot[i][c].y;
				}
				if ( l_allot[i][c].x > maxPos.x ) {
					maxPos.x = l_allot[i][c].x;
				}
				if ( l_allot[i][c].y > maxPos.y ) {
					maxPos.y = l_allot[i][c].y;
				}
			}
		}
		minPos *= 1.5f;
		maxPos *= 1.5f;
		Quadrilateral townquad;
		townquad[0] = minPos;
		townquad[2] = maxPos;
		townquad[1] = Vector2d( townquad[2].x, townquad[0].y );
		townquad[3] = Vector2d( townquad[0].x, townquad[2].y );
		l_oversized.push_back( townquad );
	}

	// Split the singular oversized quad based on the current smaller quads.
}