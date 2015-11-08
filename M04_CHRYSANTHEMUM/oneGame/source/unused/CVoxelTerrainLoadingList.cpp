
#include "CVoxelTerrain.h"

using std::cout;
using std::endl;

// Push a new loading request onto the list
void CVoxelTerrain::PushLoadingList ( CBoob* pBoob, RangeVector const& position, bool recursive )
{
	if ( vBoobLoadList.size() > 100000 )
	{
		cout << "WARNING: LOAD LIST OVERLOAD!" << endl;
		return;
	}

	bool regenSurround = recursive;
	bool regenWater = false;
	// First check if it's in the loading list, if it is, delete it
	int index = IsInLoadingList ( pBoob );
	// Check if it's in the loading list.
	if ( index >= 0 )
	{
		// If it's in the list, grab it's recursive regen value
		regenSurround = vBoobLoadList[index].regenSurround;
		// Also grab the water regen value
		regenWater = vBoobLoadList[index].regenWater;
		// And remove it
		RemoveFromLoadingList( index );
	}

	// Add a new request to the back
	sVTQueuedLoad newRequest;
	newRequest.pBoob = pBoob;
	newRequest.position = position;
	newRequest.regenSurround = regenSurround;
	newRequest.regenMesh = true;
	newRequest.regenWater = regenWater;

	mutex::scoped_lock( mtLoadingListGaurd );
	vBoobLoadList.push_back( newRequest );
}
// Push a new loading request onto the list, but variation for water
void CVoxelTerrain::PushLoadingListWater ( CBoob* pBoob, RangeVector const& position, bool recursive )
{
	if ( vBoobLoadList.size() > 100000 )
	{
		cout << "WARNING: LOAD LIST OVERLOAD!" << endl;
		return;
	}

	//bool regenSurround = recursive;
	//bool regenMesh = false;
	// First check if it's in the loading list, if it is, delete it
	int index = IsInLoadingList ( pBoob );
	// Check if it's in the loading list.
	if ( index >= 0 )
	{
		// If it's in the list, grab it's recursive regen value
		//regenSurround = vBoobLoadList[index].regenSurround;
		// Also grab the water regen value
		//regenMesh = vBoobLoadList[index].regenMesh;
		// And remove it
		//RemoveFromLoadingList( index );
		vBoobLoadList[index].regenSurround = ( vBoobLoadList[index].regenSurround||recursive );
		vBoobLoadList[index].regenWater = true;
	}
	else
	{
		// Add a new request to the back
		sVTQueuedLoad newRequest;
		newRequest.pBoob = pBoob;
		newRequest.position = position;
		newRequest.regenSurround = recursive;
		newRequest.regenMesh = false;
		newRequest.regenWater = true;

		mutex::scoped_lock( mtLoadingListGaurd );
		vBoobLoadList.push_back( newRequest );
	}
}
// Look through the loading list and find the target in it
int CVoxelTerrain::IsInLoadingList ( CBoob* pBoob )
{
	mutex::scoped_lock( mtLoadingListGaurd );
	int i, max_i;
	max_i = (signed)vBoobLoadList.size();
	for ( i = 0; i < max_i; i += 1 )
	{
		if ( vBoobLoadList[i].pBoob == pBoob )
			return i;
	}
	return -1;
}
// Remove the target index from the loading list
void CVoxelTerrain::RemoveFromLoadingList ( int targetIndex )
{
	mutex::scoped_lock( mtLoadingListGaurd );
	vBoobLoadList.erase( vBoobLoadList.begin()+targetIndex );
}
// Clear the loading list completely
void CVoxelTerrain::ClearLoadingList ( void )
{
	mutex::scoped_lock( mtLoadingListGaurd );
	vBoobLoadList.clear();
}
// Pop the query from the list and load it
void CVoxelTerrain::PopLoadingList ( void )
{
	if ( !vBoobLoadList.empty() )
	{
		// Reset stall count
		iStallCount = 0;

		// Increase pop count
		iPopListCount += 1;

		// Get the boob
		//sVTQueuedLoad topRequest = vBoobLoadList.back();
		sVTQueuedLoad topRequest;
		{
			mutex::scoped_lock( mtLoadingListGaurd );
			topRequest = vBoobLoadList.front();
		}

		// Remove the boob from the list
		//vBoobLoadList.pop_back();
		//vBoobLoadList.pop();
		// Wait for unlock, then lock
		//while ( topRequest.pBoob->locked );
		//topRequest.pBoob->locked = true;
		// If locked, try again later
		if ( topRequest.pBoob->locked )
		{
			mutex::scoped_lock( mtLoadingListGaurd );
			vBoobLoadList.push_back( vBoobLoadList.front() );
			vBoobLoadList.erase( vBoobLoadList.begin() );
			return;
		}

		if ( !topRequest.pBoob->pMesh )
		{
			cout << "Warning in CVoxelTerrain::PopLoadingList: Boob does not have a mesh allocated!" << endl;
			// Unlock the boob
			topRequest.pBoob->locked = false;
			return;
		}

		// Mark as locked
		topRequest.pBoob->locked = true;

		// Get the boob's data
		if ( !bLoadLowDetail )	// No LOD's on far boobs
		{
			// Load prop data
			topRequest.pBoob->loadPropData = true;
			// If no data loaded, then get data
			if ( !topRequest.pBoob->hasBlockData )
			{
				GetBoobData( topRequest.pBoob, topRequest.position, true );
				UpdateOre_Default32( topRequest.pBoob );
			}
			// If has data loaded, tell to add to loading list
			if ( topRequest.pBoob->hasBlockData )
			{
				if ( topRequest.pBoob->pMesh == NULL ) {
					cout << "Warning in CVoxelTerrain::PopLoadingList: Boob does not have a mesh allocated!" << endl;
					return;
				} // Memory error somewhere around here.
				topRequest.pBoob->pMesh->needUpdateOnVBO = topRequest.regenMesh; // Request an update on the newly loaded boob
				topRequest.pBoob->pMesh->needUpdateOnWaterVBO = topRequest.regenWater;
				topRequest.pBoob->pCollision->needUpdateOnCollider = topRequest.pBoob->pCollision->needUpdateOnCollider || topRequest.regenMesh; // Update collision on newly loaded boob
				if ( topRequest.pBoob->pCollision->needUpdateOnCollider )
				{
					for ( int i = 0; i < 8; ++i ) {
						topRequest.pBoob->pCollision->update[i] = true;
					}
				}

				// Remove from loading list
				RemoveFromLoadingList( 0 );
			}
			else // If has no data, try again later
			{
				{
					mutex::scoped_lock( mtLoadingListGaurd );
					vBoobLoadList.push_back( vBoobLoadList.front() );
					vBoobLoadList.erase( vBoobLoadList.begin() );
				}

				// Pop again since we did no work
				//PopLoadingList(); // this causes same lag (infinite loop!)
				// need to check if the list has something loadable before going through the loop
				if ( iPopListCount < vBoobLoadList.size()/2 ) PopLoadingList();
			}
		}
		/*else // LOD's on far boobs
		{
			// Change the request for prop data
			topRequest.pBoob->loadPropData = (topRequest.pBoob->current_resolution == 1);

			// If no data loaded, then get data
			if ( topRequest.pBoob->loadPropData )		// If loading props
				if ( !topRequest.pBoob->hasPropData )	// and no props are loaded
					GetBoobData( topRequest.pBoob, topRequest.position, true ); // Generate
			if ( !topRequest.pBoob->loadPropData )		// If not loading props
				if ( !topRequest.pBoob->hasBlockData )  // But still no block data
					GetBoobData( topRequest.pBoob, topRequest.position, true ); // Generate either way

			if ( topRequest.pBoob->hasBlockData )
				UpdateOre_Default32( topRequest.pBoob );

			// Normal resolution
			if ( topRequest.pBoob->loadPropData )
			{
				// If has block data loaded, request regen
				if ( topRequest.pBoob->hasBlockData )
				{
					topRequest.pBoob->pMesh->needUpdateOnVBO = topRequest.regenMesh; // Request an update on the newly loaded boob
					topRequest.pBoob->pMesh->needUpdateOnWaterVBO = topRequest.regenWater;
					topRequest.pBoob->pCollision->needUpdateOnCollider = topRequest.pBoob->pCollision->needUpdateOnCollider || topRequest.regenMesh; // Update collision on newly loaded boob

					// Remove from loading list
					RemoveFromLoadingList( 0 );
				}
				else // If has no data, try again later
				{
					{
						mutex::scoped_lock( mtLoadingListGaurd );
						vBoobLoadList.push_back( vBoobLoadList.front() );
						vBoobLoadList.erase( vBoobLoadList.begin() );
					}

					// Pop again since we did no work
					if ( iPopListCount < vBoobLoadList.size()/2 ) PopLoadingList();
				}
			}
			else // Lower resolution
			{
				// If has data loaded, then request regen
				if ( topRequest.pBoob->hasBlockData )
				{
					topRequest.pBoob->pMesh->needUpdateOnVBO = topRequest.regenMesh; // Request an update on the newly loaded boob
					topRequest.pBoob->pMesh->needUpdateOnWaterVBO = topRequest.regenWater;
					topRequest.pBoob->pCollision->needUpdateOnCollider = false; // Make sure this is always false for far away places :D

					// Remove from loading list
					RemoveFromLoadingList( 0 );
				}
				else // If has no data, try again later
				{
					vBoobLoadList.push_back( vBoobLoadList.front() );
					vBoobLoadList.erase( vBoobLoadList.begin() );

					// Pop again since we did no work
					if ( iPopListCount < vBoobLoadList.size()/2 ) PopLoadingList();
				}
			}
		}*/

		// Push the surrounding areas to the list if the request asks
		if ( topRequest.regenSurround )
		{
			if ( topRequest.regenMesh )
			{
				if ( topRequest.pBoob->front )
					PushLoadingList( topRequest.pBoob->front, RangeVector( topRequest.position.x+1, topRequest.position.y, topRequest.position.z ), false );
				if ( topRequest.pBoob->back )
					PushLoadingList( topRequest.pBoob->back, RangeVector( topRequest.position.x-1, topRequest.position.y, topRequest.position.z ), false );
				if ( topRequest.pBoob->left )
					PushLoadingList( topRequest.pBoob->left, RangeVector( topRequest.position.x, topRequest.position.y+1, topRequest.position.z ), false );
				if ( topRequest.pBoob->right )
					PushLoadingList( topRequest.pBoob->right, RangeVector( topRequest.position.x, topRequest.position.y-1, topRequest.position.z ), false );
				if ( topRequest.pBoob->top )
					PushLoadingList( topRequest.pBoob->top, RangeVector( topRequest.position.x, topRequest.position.y, topRequest.position.z+1 ), false );
				if ( topRequest.pBoob->bottom )
					PushLoadingList( topRequest.pBoob->bottom, RangeVector( topRequest.position.x, topRequest.position.y, topRequest.position.z-1 ), false );
			}
			if ( topRequest.regenWater )
			{
				if ( topRequest.pBoob->front )
					PushLoadingListWater( topRequest.pBoob->front, RangeVector( topRequest.position.x+1, topRequest.position.y, topRequest.position.z ), false );
				if ( topRequest.pBoob->back )
					PushLoadingListWater( topRequest.pBoob->back, RangeVector( topRequest.position.x-1, topRequest.position.y, topRequest.position.z ), false );
				if ( topRequest.pBoob->left )
					PushLoadingListWater( topRequest.pBoob->left, RangeVector( topRequest.position.x, topRequest.position.y+1, topRequest.position.z ), false );
				if ( topRequest.pBoob->right )
					PushLoadingListWater( topRequest.pBoob->right, RangeVector( topRequest.position.x, topRequest.position.y-1, topRequest.position.z ), false );
				if ( topRequest.pBoob->top )
					PushLoadingListWater( topRequest.pBoob->top, RangeVector( topRequest.position.x, topRequest.position.y, topRequest.position.z+1 ), false );
				if ( topRequest.pBoob->bottom )
					PushLoadingListWater( topRequest.pBoob->bottom, RangeVector( topRequest.position.x, topRequest.position.y, topRequest.position.z-1 ), false );
			}
		}

		// Unlock the boob
		topRequest.pBoob->locked = false;

	}
}

// int GetLoadingListSize()
//  returns current size of the loading queue
int		CVoxelTerrain::GetLoadingListSize ( void )
{
	return (int)vBoobLoadList.size(); // should be thread-safe
}

// int GetGenerationListSize()
//  returns current size of the generation queue
int		CVoxelTerrain::GetGenerationListSize ( void )
{
	static int count = 0;

	if ( mtGenerationListGaurd.try_lock() )
	{
		if ( mtLoadingListGaurd.try_lock() )
		{
			count = 0;
			for ( uint i = 0; i < vGenerationRequests.size(); ++i )
			{
				if ( ( abs(vGenerationRequests[i].position.x - root_position.x) <= (iTerrainSize/2) )&&
					 ( abs(vGenerationRequests[i].position.y - root_position.y) <= (iTerrainSize/2) )&&
					 ( abs(vGenerationRequests[i].position.z - root_position.z) <= (iTerrainSize/2) ) )
				{
					for ( uint j = 0; j < vBoobLoadList.size(); ++j )
					{
						if ( vGenerationRequests[i].position == vBoobLoadList[j].position ) {
							j = (uint)(-2);
							count += 1;
						}
					}
				}
			}
			mtLoadingListGaurd.unlock();
		}
		mtGenerationListGaurd.unlock();
	}
	//return (int)vGenerationRequests.size(); // should be thread-safe

	return count;
}

// float GetLoadingPercent()
//  returns percent to finishing initial load
float CVoxelTerrain::GetLoadingPercent ( void )
{
	return (1.0f-( vBoobLoadList.size() / ((float)(cub( iTerrainSize ))) ));
}
// float GetGenerationPercent()
//  returns percent of starting area in loading area
/*float CVoxelTerrain::GetGenerationPercent ( void )
{
	int placeCount;

}*/


// bool CheckLoadedAndVisible( root )
//  returns true if the terrain is ready to start being played on
//  if the terrain isn't ready for play, it'll return false.
//  cl_faststart in the gameoptions skips this check (somehow)
bool CVoxelTerrain::CheckLoadedAndVisible ( CBoob* pBoob )
{
	if ( pBoob == root )
	{
		/*if ( ((signed)vBoobLoadList.size()) >= iTerrainSize )
		{
			return false;
		}*/
		if ( vBoobLoadList.size() > 0 )
		{
			for ( unsigned int i = 0; i < vBoobLoadList.size(); ++i )
			{
				if ( vBoobLoadList[i].regenMesh )
					return false;
			}
		}
	}
	if ( pBoob != NULL )
	{
		if ( pBoob->bitinfo[7] == false )
		{
			pBoob->bitinfo[7] = true;
			if (( (!pBoob->hasBlockData) || (pBoob->pMesh->needUpdateOnVBO)  )&&( pBoob->current_resolution == 1 ))
			{
				return false;
			}
			else
			{
				return (
					CheckLoadedAndVisible( pBoob->left )||
					CheckLoadedAndVisible( pBoob->right )||
					CheckLoadedAndVisible( pBoob->top )||
					CheckLoadedAndVisible( pBoob->bottom )||
					CheckLoadedAndVisible( pBoob->front )||
					CheckLoadedAndVisible( pBoob->back )
					);
			}
		}
	}
	return true;
}

// Forces a recursive update
void	CVoxelTerrain::ForceFullUpdate ( void )
{
	// Add all the terrain to the loading list
	ResetBlock7Flag( root );
	ForceRecursiveUpdate( root, root_position );
	ResetBlock7Flag( root );
	/*for ( vector<CBoob*>::iterator it = terraList.begin(); it != terraList.end(); ++it )
	{
		PushLoadingList( *it, (*it)->li_position );
	}*/
}

// Performs recursive update
void CVoxelTerrain::ForceRecursiveUpdate ( CBoob * pBoob, RangeVector const& position )
{
	if ( pBoob != NULL )
	{
		if ( pBoob->bitinfo[7] == false )
		{
			PushLoadingList( pBoob, position );

			pBoob->bitinfo[7] = true;
			pBoob->li_position = position;

			if ( position.z <= root_position.z )
				ForceRecursiveUpdate( pBoob->bottom, RangeVector( position.x, position.y, position.z-1 ) );
			if ( position.x >= root_position.x )
				ForceRecursiveUpdate( pBoob->front, RangeVector( position.x+1, position.y, position.z ) );
			if ( position.y >= root_position.y )
				ForceRecursiveUpdate( pBoob->left, RangeVector( position.x, position.y+1, position.z ) );
			if ( position.z >= root_position.z )
				ForceRecursiveUpdate( pBoob->top, RangeVector( position.x, position.y, position.z+1 ) );
			if ( position.x <= root_position.x )
				ForceRecursiveUpdate( pBoob->back, RangeVector( position.x-1, position.y, position.z ) );
			if ( position.y <= root_position.y )
				ForceRecursiveUpdate( pBoob->right, RangeVector( position.x, position.y-1, position.z ) );
		}
	}
}

struct CVoxelTerrain::tLoadingListComparator
{
	//static RangeVector root_pos;
	RangeVector root_pos;
	bool operator() ( CVoxelTerrain::sVTQueuedLoad si, CVoxelTerrain::sVTQueuedLoad sj )
	{
		CBoob* i = si.pBoob;
		CBoob* j = sj.pBoob;
		int regenWeighti = 0, regenWeightj = 0;
		if ( si.regenMesh )
			regenWeighti += 2;
		if ( sj.regenMesh )
			regenWeightj += 2;
		if ( si.regenWater )
			regenWeighti += 1;
		if ( sj.regenWater )
			regenWeightj += 1;

		// return true if i<j
		// i is smaller if needs to be loaded first
		// if i needs to be loaded first, return true

		// if i is visible, needs to be loaded first
		if ( i->bitinfo[0] != j->bitinfo[0] )
		{
			return ( i->bitinfo[0] );
		}
		else
		{
			// if i is bigger than j, needs to be loaded first
			if ( regenWeighti != regenWeightj )
			{
				return ( regenWeighti > regenWeightj );
			}
			else
			{
				// if i's resolution is more fine, needs to be loaded first
				if ( i->current_resolution != j->current_resolution )
				{
					return ( i->current_resolution < j->current_resolution );
				}
				else
				{
					Vector3d difi ( ftype(root_pos.x - i->li_position.x), ftype(root_pos.y - i->li_position.y), ftype(root_pos.z - i->li_position.z) );
					Vector3d difj ( ftype(root_pos.x - j->li_position.x), ftype(root_pos.y - j->li_position.y), ftype(root_pos.z - j->li_position.z) );
					// if i's distance is smaller, needs to be loaded first
					return ( difi.sqrMagnitude() < difj.sqrMagnitude() );
				}
			}	
		}
	};
};// LoadingListComparator;
//RangeVector tLoadingListComparator::root_pos;

// Reorders loading list based on if areas are visible
void CVoxelTerrain::ReorderLoadingList ( CBoob* pBoob )
{
	//mutex::scoped_lock( mtLoadingListGaurd );
	boost::lock_guard<mutex> guard( mtLoadingListGaurd );

	tLoadingListComparator LoadingListComparator;
	LoadingListComparator.root_pos = root_position;
	sort( vBoobLoadList.begin(), vBoobLoadList.end(), LoadingListComparator );
}

// PruneLoadingList iterates through the loading list, removing any sort of "fluff" it finds.
// Fluff includes water areas that are in immediate need of loading.
void CVoxelTerrain::PruneLoadingList ( void )
{
	//mutex::scoped_lock( mtLoadingListGaurd );
	boost::lock_guard<mutex> guard( mtLoadingListGaurd );

	if ( vBoobLoadList.size() <= 0 ) {
		return;
	}

	// loop through loading list
	int loadCount = 0;
	vector<sVTQueuedLoad>::iterator loadInfo = vBoobLoadList.begin();
	//sVTQueuedLoad currentRequest = *loadInfo;

	while ( loadInfo != vBoobLoadList.end() )
	{
		if ( loadCount > 3 ) {
			break;
		}
		//currentRequest = (*loadInfo);
		if ( loadInfo->regenWater )
		{	// w/ water needed, force update
			if (( loadInfo->pBoob->hasBlockData )&&( loadInfo->pBoob->hasPropData ))
			{	// Make sure has data loaded
				loadInfo->pBoob->pMesh->needUpdateOnWaterVBO = true;
				loadInfo->regenWater = false;
				loadCount += 1;

				// If there's no need to update the land mesh, remove this from the list
				if ( loadInfo->regenMesh == false ) {
					loadInfo = vBoobLoadList.erase( loadInfo );
					continue;
				}
			}
		}
		++loadInfo;
	}
		
}