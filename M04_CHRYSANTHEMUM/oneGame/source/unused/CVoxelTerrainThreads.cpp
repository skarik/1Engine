
#include "CVoxelTerrain.h"
#include "CGameSettings.h"
#include "CTerrainGenerator.h"

void CVoxelTerrain::StartSideThread ( void )
{
	iStallCount = 0; // Reset stall count

	bContinueThread = true;

	bQueueLoadingListPop = false;
	bQueueSimulation = false;

	sCallableThread newTStruct;
	newTStruct.pMyTerrain = this;
	mtThread = thread( newTStruct );
}
void CVoxelTerrain::EndSideThread ( void )
{
	bContinueThread = false;

	mtThread.join();
}
void CVoxelTerrain::sCallableThread::operator () ( void )
{
	pMyTerrain->WorkSideThread();
}
void CVoxelTerrain::WorkSideThread ( void )
{
	while ( bContinueThread )
	{
		iStallCount += 1; // Up the stall count
		if ( bQueueSimulation && bInitialLoadDone )
		{
			// Lock the terrain while updating
			mutex::scoped_lock local_lock( mtGaurd );

			// Perform world simulation
			if ( generator ) {
				generator->Simulate();
			}

			// Perform recursive simulation
			ResetBlock7Flag( root );
			UpdateSimulation( root, root_position );
			ResetBlock7Flag( root );

			// Don't do simulation for a frame
			bQueueSimulation = false;
		}
		if ( bQueueLoadingListPop )
		{
			// Reorder loading list based on visible areas
			static int popCount = 0;
			popCount += 1;
			if (( popCount > 2 )&&( bInitialLoadDone ))
			{
				ReorderLoadingList(NULL);
				popCount = 0;
			}
			
			// Save
			iPopListCount = 0;	// Reset loadlist count
			PopSavingList();

			// Load
			iPopListCount = 0;	// Reset loadlist count
			PopLoadingList();

			// Prune the loading list
			if ( bInitialLoadDone ) PruneLoadingList();

			// Don't pop until next request
			bQueueLoadingListPop = false;
		}

		if ( bLODBeginLoad )
		{
			// Handle the loading of the far LOD's
			LOD_Level1_UpdateTarget();
			bLODBeginLoad = false;
		}

		if ( bMinimizeThreads ) {	// Try to stop thread if possible
			if ( iStallCount > 4 ) {
				// Stop the thread since we're stalling
				bContinueThread = false;
			}
		}
	}
}


void CVoxelTerrain::StartGenerationThread ( void )
{
	iGenerationStallCount = 0;
	iGenerationCreateCount = 0;
	iGenerationPregenSpread = 0;

	bContinueGenerationThread = true;

	vGenerationRequests.reserve( iTerrainSize*iTerrainSize*iTerrainSize );

	sGenerationThread newTStruct;
	newTStruct.pMyTerrain = this;
	mtGenerationThread = thread( newTStruct );
}
void CVoxelTerrain::EndGenerationThread ( void )
{
	bContinueGenerationThread = false;

	mtGenerationThread.join();
}
void CVoxelTerrain::sGenerationThread::operator () ( void )
{
	pMyTerrain->WorkGenerationThread();
}
void CVoxelTerrain::WorkGenerationThread ( void )
{
	while ( bContinueGenerationThread )
	{
		iGenerationStallCount += 1; // Up the stall count

		sGenerationRequest currentRequest;
		bool hasRequest = false;
		{
			// Lock the list while checking list
			mutex::scoped_lock local_lock( mtGenerationListGaurd );

			// Check list for a request
			if ( vGenerationRequests.size() > 0 )
			{
				currentRequest = vGenerationRequests.back();
				vGenerationRequests.pop_back();
				hasRequest = true;
			}

			// Lock ends here
		}

		// If we have a request, then create some epic stuff
		if ( hasRequest )
		{
			iGenerationStallCount = 0; // Reset stall count
			
			// Check for the file already existing
			bool bDoGeneration = true;
			if ( currentRequest.checkfile )
			{
				CBinaryFile oCheckfile;
				char tempstring[256];

				sprintf( tempstring, "%s/%d_%d_%d\0",
					CGameSettings::Active()->GetTerrainSaveDir().c_str(),
					currentRequest.position.x,currentRequest.position.y,currentRequest.position.z );

				if ( oCheckfile.Exists( tempstring ) ) {
					bDoGeneration = false;
				}
			}
			if ( bDoGeneration )
			{ iploadstate = 1; iGenerationPregenSpread = 0;
				// With the current request, create a new metaboob
				CMetaboob* pBoob = GenerateMetaboob();
				// Set generation mode
				generator->SetGenerationMode( CTerrainGenerator::GENERATE_TO_DISK );
				// Generate with the metaboob
				generator->GenerateSingleTerrain( pBoob, currentRequest.position );
				// Save the boob
				SaveMetaboobData( pBoob, currentRequest.position );
				// Free the boob
				FreeBoob( pBoob );
			}

			iGenerationCreateCount += 1;
			// If create count is greater than a certain amount, reorder the requests
			if ( iGenerationCreateCount % 4 == 0 ) {
				PruneGenerationRequests();
			}
		}
		else
		{
			// If we don't have a request, push back the surrounding size*2 x size*2 area, plus what the player is looking at.
			/*const int iHalfSize = iTerrainSize/2;
			int iTerraLoadSize;
			if ( bLoadLowDetail ) {
				iTerraLoadSize = iTerrainSize*2;
			}
			else {
				iTerraLoadSize = iTerrainSize;
			}
			for ( int i = -iTerraLoadSize; i < iTerraLoadSize; ++i )
			{
				if (( i < -iHalfSize )||( i > iHalfSize ))
				for ( int j = -iTerraLoadSize; j < iTerraLoadSize; ++j ) 
				{
					if (( j < -iHalfSize )||( j > iHalfSize ))
					for ( int k = -iTerraLoadSize; k < iTerraLoadSize; ++k )
					{
						if (( k < -iHalfSize )||( k > iHalfSize ))
						{
							// Push the new request (needed terrains should get pushed to the top after this)
							AddGenerationRequest( RangeVector( root_position.x+i, root_position.y+j, root_position.z+k ), true );
						}
					}
				}
			}*/
			iGenerationPregenSpread += 1;
			int iTerraLoadSize, iTerraLoadSizeZ;
			iTerraLoadSize = iTerrainSize+iGenerationPregenSpread;
			iTerraLoadSizeZ = iTerrainSize+iGenerationPregenSpread/4;

			// If we don't have a request, push back the surrounding size*2 x size*2 area, plus what the player is looking at.
			for ( int i = -iTerraLoadSize; i < iTerraLoadSize; ++i )
			{
				for ( int j = -iTerraLoadSize; j < iTerraLoadSize; ++j ) 
				{
					for ( int k = -iTerraLoadSizeZ; k < iTerraLoadSizeZ; ++k )
					{
						// Push the new request (needed terrains should get pushed to the top after this)
						AddGenerationRequest( RangeVector( root_position.x+i, root_position.y+j, root_position.z+k ), true );
					}
				}
			}
			
			if ( iGenerationPregenSpread > iTerrainSize*2 ) {
				iGenerationPregenSpread -= 2;
			}

			// Reorder the requests
			PruneGenerationRequests();
		}
		iploadstate = 0;

		if ( bMinimizeThreads ) {	// Try to stop thread if possible
			if ( iGenerationStallCount > 4 ) {
				// Stop the thread since we're stalling
				bContinueGenerationThread = false;
			}
		}
	} // End while loop
}