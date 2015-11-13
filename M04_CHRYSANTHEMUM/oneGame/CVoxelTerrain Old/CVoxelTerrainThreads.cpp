
#include "CVoxelTerrain.h"
#include "CGameSettings.h"
#include "CTerrainGenerator.h"

void CVoxelTerrain::StartSideThread ( void )
{
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
		if ( bQueueSimulation )
		{
			// Lock the terrain while updating
			mutex::scoped_lock local_lock( mtGaurd );

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
			if ( popCount > 2 )
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

			// Don't pop until next request
			bQueueLoadingListPop = false;
		}
	}
}


void CVoxelTerrain::StartGenerationThread ( void )
{
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
			// todo: do we need to check for the file already existing? (yes, if the request says to check)
			bool bDoGeneration = true;
			if ( currentRequest.checkfile )
			{
				CBinaryFile oCheckfile;
				stringstream tempStream;

				tempStream << CGameSettings::Active()->GetTerrainSaveDir() << "\\";
				tempStream << currentRequest.position.x << "_" << currentRequest.position.y << "_" << currentRequest.position.z;

				if ( oCheckfile.Exists( tempStream.str().c_str() ) )
				{
					bDoGeneration = false;
				}
			}
			if ( bDoGeneration )
			{ iploadstate = 1;
				// With the current request, create a new metaboob
				CMetaboob* pBoob = GenerateMetaboob();
				//if ( pBoob->pGrass )
				//	cout << "Constructor error..." << endl;
				// Set generation mode
				//--iGenerationMode = GENERATE_TO_DISK;
				generator->SetGenerationMode( CTerrainGenerator::GENERATE_TO_DISK );
				// Generate with the metaboob
				//--GenerateSingleTerrain( pBoob, currentRequest.position );
				generator->GenerateSingleTerrain( pBoob, currentRequest.position );
				//if ( pBoob->pGrass )
				//	cout << "Memory error..." << endl;
				// Save the boob
				SaveMetaboobData( pBoob, currentRequest.position );
				//if ( pBoob->pGrass )
				//	cout << "Saving error..." << endl;
				// Free the boob
				FreeBoob( pBoob );
			}
		}
		else
		{
			// If we don't have a request, push back the surrounding size*2 x size*2 area.
			const int iHalfSize = iTerrainSize/2;
			int iTerraLoadSize;
			if ( bLoadLowDetail )
				iTerraLoadSize = iTerrainSize*2;
			else
				iTerraLoadSize = iTerrainSize;
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
							AddGenerationRequest( LongIntPosition( root_position.x+i, root_position.y+j, root_position.z+k ), true );
						}
					}
				}
			}
		}
		iploadstate = 0;
	}
}

// This locks the generation thread, then adds or pushes down a request for terrain loading
void CVoxelTerrain::AddGenerationRequest ( const LongIntPosition & newRequestPosition, const bool newRequestCheckfile )
{
	// Lock the list while checking list
	mutex::scoped_lock local_lock( mtGenerationListGaurd );

	// Check for the request
	bool exists = false;
	vector<sGenerationRequest>::iterator found_iterator;
	for ( vector<sGenerationRequest>::iterator it = vGenerationRequests.begin(); it != vGenerationRequests.end(); ++it )
	{
		if ( it->position == newRequestPosition )
		{
			exists = true;
			found_iterator = it;
		}
	}

	// Add the request if it doesn't exist
	if ( !exists )
	{
		sGenerationRequest newRequest;
		newRequest.position = newRequestPosition;
		newRequest.checkfile = newRequestCheckfile;
		vGenerationRequests.push_back( newRequest );
	}
	else
	{
		// Move the request down if it does
		vGenerationRequests.erase( found_iterator );

		sGenerationRequest newRequest;
		newRequest.position = newRequestPosition;
		newRequest.checkfile = (newRequestCheckfile||newRequest.checkfile);
		vGenerationRequests.push_back( newRequest );
	}
}