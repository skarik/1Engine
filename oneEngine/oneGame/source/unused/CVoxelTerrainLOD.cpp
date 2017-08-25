
#include "CVoxelTerrain.h"
#include "Random.h"

// Initialize the LOD
void CVoxelTerrain::LODInit ( void )
{
	CLODBoobMesh::nullBlock.loaded = false;
	CLODBoobMesh::nullBlock.solid = false;

	pLOD_Level1Mesh = new CLODBoobMesh();
	iCurrentLoadTarget = 0;
	iCurrentSkipDirection = 1;
	bLODWantsRegen = false;

	lod_root_draw_position = root_position;

	bLODCanDraw_L1 = false;

	bLODBeginLoad = false;

	terra_t_lod* lodBlock = GetLODDataBlock(1);
	for ( int i = 0; i < 32768; ++i ) {
		lodBlock[i].loaded = false;
		lodBlock[i].solid = false;
	}
}
// Free the LOD
void CVoxelTerrain::LODFree ( void )
{
	delete pLOD_Level1Mesh;
}

// Update the loading of the terrain.
//  This is called in the loader thread, not the main thread.
void CVoxelTerrain::LOD_Level1_UpdateTarget ( void )
{
	int failCount = 0;
	bool loadResult;
	terra_t_lod* lodBlock = GetLODDataBlock(1);

	mutex::scoped_lock( mtLevelOfDetail );

	while ( failCount < 100 )
	{
		if ( lodBlock[iCurrentLoadTarget].loaded <= 0 ) {
			// TODO: Skip if no visiblity
			bool hadChange = false;
			loadResult = LOD_Level1_LoadTarget(hadChange);
			if ( !loadResult ) { // On fail jump around the list
				iCurrentLoadTarget = (iCurrentLoadTarget+(Random.Next()%65536))%32768;
				failCount += 31;
				
				// Change loading direction
				switch ( Random.Next()%3 ) {
					case 0: iCurrentSkipDirection = 1; break;
					case 1: iCurrentSkipDirection = 32; break;
					case 2: iCurrentSkipDirection = 1024; break;
				};
			}
			else { // Successful load
				if ( hadChange ) {
					bLODWantsRegen = true;
				}
				failCount += 211;
				iCurrentLoadTarget = (iCurrentLoadTarget+iCurrentSkipDirection)%32768;
			}
		}
		else { // Already loaded, so load adjacent values
			iCurrentLoadTarget = (iCurrentLoadTarget+iCurrentSkipDirection)%32768;
			failCount += 1;
		}
	}
	// If failed a lot
	if ( failCount > 110 ) {
		// Search first unloaded spot
		for ( int i = 0; i < 32768; ++i ) {
			if ( lodBlock[iCurrentLoadTarget].loaded <= 0 ) {
				break;
			}
			else {
				iCurrentLoadTarget = (iCurrentLoadTarget+1)%32768;
			}
		}
		// Change loading direction
		switch ( Random.Next()%3 ) {
			case 0: iCurrentSkipDirection = 1; break;
			case 1: iCurrentSkipDirection = 32; break;
			case 2: iCurrentSkipDirection = 1024; break;
		};
	}

	// Check loaded count
	/*int count = 0;
	for ( int i = 0; i < 32768; ++i ) {
		if ( lodBlock[i].loaded ) {
			++count;
		}
	}*/
}

RangeVector CVoxelTerrain::LODIndexToRangeVector ( unsigned short iLODIndex )
{
	//RangeVector cornerPos = root_position - RangeVector(16,16,16);
	RangeVector cornerPos;
	cornerPos.x = root_position.x - 16;
	cornerPos.y = root_position.y - 16;
	cornerPos.z = root_position.z - 16;
		
	/*RangeVector lodOffset;
	lodOffset.x = iLODIndex % 32;
	lodOffset.y = (iLODIndex/32) % 32;
	lodOffset.z = iLODIndex/1024;
	
	RangeVector convertedPos = cornerPos + lodOffset;
	return convertedPos;*/
	cornerPos.x += iLODIndex % 32;
	cornerPos.y += (iLODIndex/32) % 32;
	cornerPos.z += iLODIndex/1024;

	return cornerPos;
}


void CVoxelTerrain::LOD_ShiftData ( int levelIndex, RangeVector const& shiftOffset )
{
	terra_t_lod* lod_blocks = GetLODDataBlock(1);
	int x,y,z;

	mutex::scoped_lock( mtLevelOfDetail );

	// Shift on X axis
	if ( shiftOffset.x < 0 ) {
		for ( x = 0; x < 32+shiftOffset.x; ++x ) {
			for ( y = 0; y < 32; ++y ) {
				for ( z = 0; z < 32; ++z ) {
					lod_blocks[x+y*32+z*1024] = lod_blocks[(x-shiftOffset.x)+y*32+z*1024];
				}
			}
		}
		for ( x = 32+shiftOffset.x; x < 32; ++x ) {
			for ( y = 0; y < 32; ++y ) {
				for ( z = 0; z < 32; ++z ) {
					lod_blocks[x+y*32+z*1024].loaded = 0;
					lod_blocks[x+y*32+z*1024].solid = 0;
				}
			}
		}
	}
	else if ( shiftOffset.x > 0 ) {
		for ( x = 32; x >= shiftOffset.x; --x ) {
			for ( y = 0; y < 32; ++y ) {
				for ( z = 0; z < 32; ++z ) {
					lod_blocks[x+y*32+z*1024] = lod_blocks[(x-shiftOffset.x)+y*32+z*1024];
				}
			}
		}
		for ( x = shiftOffset.x-1; x >= 0; --x ) {
			for ( y = 0; y < 32; ++y ) {
				for ( z = 0; z < 32; ++z ) {
					lod_blocks[x+y*32+z*1024].loaded = 0;
					lod_blocks[x+y*32+z*1024].solid = 0;
				}
			}
		}
	}
	// Shift on Y axis
	if ( shiftOffset.y < 0 ) {
		for ( y = 0; y < 32+shiftOffset.y; ++y ) {
			for ( x = 0; x < 32; ++x ) {
				for ( z = 0; z < 32; ++z ) {
					lod_blocks[x+y*32+z*1024] = lod_blocks[x+(y-shiftOffset.y)*32+z*1024];
				}
			}
		}
		for ( y = 32+shiftOffset.y; y < 32; ++y ) {
			for ( x = 0; x < 32; ++x ) {
				for ( z = 0; z < 32; ++z ) {
					lod_blocks[x+y*32+z*1024].loaded = 0;
					lod_blocks[x+y*32+z*1024].solid = 0;
				}
			}
		}
	}
	else if ( shiftOffset.y > 0 ) {
		for ( y = 32; y >= shiftOffset.y; --y ) {
			for ( x = 0; x < 32; ++x ) {
				for ( z = 0; z < 32; ++z ) {
					lod_blocks[x+y*32+z*1024] = lod_blocks[x+(y-shiftOffset.y)*32+z*1024];
				}
			}
		}
		for ( y = shiftOffset.y-1; y >= 0; --y ) {
			for ( x = 0; x < 32; ++x ) {
				for ( z = 0; z < 32; ++z ) {
					lod_blocks[x+y*32+z*1024].loaded = 0;
					lod_blocks[x+y*32+z*1024].solid = 0;
				}
			}
		}
	}
	// Shift on Z axis
	if ( shiftOffset.z < 0 ) {
		for ( z = 0; z < 32+shiftOffset.z; ++z ) {
			for ( y = 0; y < 32; ++y ) {
				for ( x = 0; x < 32; ++x ) {
					lod_blocks[x+y*32+z*1024] = lod_blocks[x+y*32+(z-shiftOffset.z)*1024];
				}
			}
		}
		for ( z = 32+shiftOffset.z; z < 32; ++z ) {
			for ( y = 0; y < 32; ++y ) {
				for ( x = 0; x < 32; ++x ) {
					lod_blocks[x+y*32+z*1024].loaded = 0;
					lod_blocks[x+y*32+z*1024].solid = 0;
				}
			}
		}
	}
	else if ( shiftOffset.z > 0 ) {
		for ( z = 32; z >= shiftOffset.z; --z ) {
			for ( y = 0; y < 32; ++y ) {
				for ( x = 0; x < 32; ++x ) {
					lod_blocks[x+y*32+z*1024] = lod_blocks[x+y*32+(z-shiftOffset.z)*1024];
				}
			}
		}
		for ( z = shiftOffset.z-1; z >= 0; --z ) {
			for ( y = 0; y < 32; ++y ) {
				for ( x = 0; x < 32; ++x ) {
					lod_blocks[x+y*32+z*1024].loaded = 0;
					lod_blocks[x+y*32+z*1024].solid = 0;
				}
			}
		}
	}

	bLODWantsRegen = true;

	//
}