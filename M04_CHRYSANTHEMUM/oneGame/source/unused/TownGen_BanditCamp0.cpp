
#include "TownGen_BanditCamp0.h"

#include "CTownManager.h"
#include "CTerraGenDefault.h"
#include "BlockInfo.h"

void TownGen_BanditCamp0::Generate ( void )
{
	// Initialize randomizer values
	mseed = 0;

	// Set center position
	townCenterPosition = Vector3d( areaPosition.x+0.5f,areaPosition.y+0.5f,areaElevation-0.1f );


	// Step 1: make center (first + flat area)
	{
		CTownManager::sPatternInstance instance;
		instance.position = townCenterPosition;
		instance.type = NST_CLEARING;
		instance.category = 0;
		genTown->instances.push_back( instance );
	}

	// Step 2: make shelters (tents and/or shanties)
	
	// Step 3: make watchtowers
	{
		CTownManager::sPatternInstance instance;
		instance.position = townCenterPosition;
		instance.position += Vector3d( RandomNumber(), RandomNumber(), 0 );
		instance.type = NST_VXG_WATCHTOWER0;
		instance.category = 0;

		genTown->instances.push_back( instance );
	}

	// Final step: compute bounding boxes
	{
		CalculateTownBoundingBox();
	}
}

void TownGen_BanditCamp0::CalculateTownBoundingBox ( void )
{
	// Look at the current town layout and set the town sizes as needed
	{
		Vector3d deltaStart, deltaEnd;
		//ftype sqrMag;

		// Go through all instances
		for ( uint i = 0; i < genTown->instances.size(); ++i )
		{
			deltaStart = genTown->instances[i].position - townCenterPosition - Vector3d(1,1,0.5f);
			deltaEnd = genTown->instances[i].position - townCenterPosition + Vector3d(1,1,1.5f);

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