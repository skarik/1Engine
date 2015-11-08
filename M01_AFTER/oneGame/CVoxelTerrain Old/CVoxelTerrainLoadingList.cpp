
#include "CVoxelTerrain.h"

// Push a new loading request onto the list
void CVoxelTerrain::PushLoadingList ( CBoob* pBoob, LongIntPosition const& position, bool recursive )
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
void CVoxelTerrain::PushLoadingListWater ( CBoob* pBoob, LongIntPosition const& position, bool recursive )
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
// Pop the query from the list and load it
void CVoxelTerrain::PopLoadingList ( void )
{
	if ( !vBoobLoadList.empty() )
	{
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

		if ( !topRequest.pBoob->pMesh )
		{
			cout << "Warning in CVoxelTerrain::PopLoadingList: Boob does not have a mesh allocated!" << endl;
			return;
		}

		// Get the boob's data
		if ( !bLoadLowDetail )	// No LOD's on far boobs
		{
			// If no data loaded, then get data
			if ( !topRequest.pBoob->hasData )
			{
				GetBoobData( topRequest.pBoob, topRequest.position, true );
			}
			// If has data loaded, tell to add to loading list
			if ( topRequest.pBoob->hasData )
			{
				topRequest.pBoob->pMesh->needUpdateOnVBO = topRequest.regenMesh; // Request an update on the newly loaded boob
				topRequest.pBoob->pMesh->needUpdateOnWaterVBO = topRequest.regenWater;

				// Remove from loading list
				RemoveFromLoadingList( 0 );
			}
			else // If has no data, try again later
			{
				vBoobLoadList.push_back( vBoobLoadList.front() );
				vBoobLoadList.erase( vBoobLoadList.begin() );

				// Pop again since we did no work
				//PopLoadingList(); // this causes same lag (infinite loop!)
				// need to check if the list has something loadable before going through the loop
				if ( iPopListCount < vBoobLoadList.size()/2 )
					PopLoadingList();
			}
		}
		else // LOD's on far boobs
		{
			// If no data loaded, then get data
			/*if (( !topRequest.pBoob->hasData )||( !topRequest.pBoob->hasBlockData ))
			{
				if ( topRequest.pBoob->current_resolution == 1 )	// However, only generate if the current resolution is full
					GetBoobData( topRequest.pBoob, topRequest.position, true );
				else if ( !topRequest.pBoob->hasBlockData )
					GetBoobData( topRequest.pBoob, topRequest.position, false ); // But this means, if this is still in the loading list, it gets loaded again
			}*/
			if ( topRequest.pBoob->current_resolution == 1 )
				if ( !topRequest.pBoob->hasData )
					GetBoobData( topRequest.pBoob, topRequest.position, true );
			if ( topRequest.pBoob->current_resolution != 1 )
				if ( !topRequest.pBoob->hasBlockData )
					GetBoobData( topRequest.pBoob, topRequest.position, true ); // Generate either way

			// Normal resolution
			if ( topRequest.pBoob->current_resolution == 1 )
			{
				// If has data loaded, request regen
				if ( topRequest.pBoob->hasData )
				{
					topRequest.pBoob->pMesh->needUpdateOnVBO = topRequest.regenMesh; // Request an update on the newly loaded boob
					topRequest.pBoob->pMesh->needUpdateOnWaterVBO = topRequest.regenWater;

					// Remove from loading list
					RemoveFromLoadingList( 0 );
				}
				else // If has no data, try again later
				{
					vBoobLoadList.push_back( vBoobLoadList.front() );
					vBoobLoadList.erase( vBoobLoadList.begin() );

					// Pop again since we did no work
					if ( iPopListCount < vBoobLoadList.size()/2 )
						PopLoadingList();
				}
			}
			else // Lower resolution
			{
				// If has data loaded, then request regen
				if ( topRequest.pBoob->hasBlockData )
				{
					topRequest.pBoob->pMesh->needUpdateOnVBO = topRequest.regenMesh; // Request an update on the newly loaded boob
					topRequest.pBoob->pMesh->needUpdateOnWaterVBO = topRequest.regenWater;

					// Remove from loading list
					RemoveFromLoadingList( 0 );
				}
				else // If has no data, try again later
				{
					vBoobLoadList.push_back( vBoobLoadList.front() );
					vBoobLoadList.erase( vBoobLoadList.begin() );

					// Pop again since we did no work
					if ( iPopListCount < vBoobLoadList.size()/2 )
						PopLoadingList();
				}
			}
		}

		/*if ( topRequest.regenWater && !topRequest.regenMesh )
		{
			stepNum += 2;
		}*/

		// Request an update on the neighbors
		/*if ( topRequest.pBoob->front )
			topRequest.pBoob->front->pMesh->needUpdateOnVBO = true;
		if ( topRequest.pBoob->back )
			topRequest.pBoob->back->pMesh->needUpdateOnVBO = true;
		if ( topRequest.pBoob->left )
			topRequest.pBoob->left->pMesh->needUpdateOnVBO = true;
		if ( topRequest.pBoob->right )
			topRequest.pBoob->right->pMesh->needUpdateOnVBO = true;
		if ( topRequest.pBoob->top )
			topRequest.pBoob->top->pMesh->needUpdateOnVBO = true;
		if ( topRequest.pBoob->bottom )
			topRequest.pBoob->bottom->pMesh->needUpdateOnVBO = true;*/
		if ( topRequest.regenSurround )
		{
			if ( topRequest.regenMesh )
			{
				if ( topRequest.pBoob->front )
					PushLoadingList( topRequest.pBoob->front, LongIntPosition( topRequest.position.x+1, topRequest.position.y, topRequest.position.z ), false );
				if ( topRequest.pBoob->back )
					PushLoadingList( topRequest.pBoob->back, LongIntPosition( topRequest.position.x-1, topRequest.position.y, topRequest.position.z ), false );
				if ( topRequest.pBoob->left )
					PushLoadingList( topRequest.pBoob->left, LongIntPosition( topRequest.position.x, topRequest.position.y+1, topRequest.position.z ), false );
				if ( topRequest.pBoob->right )
					PushLoadingList( topRequest.pBoob->right, LongIntPosition( topRequest.position.x, topRequest.position.y-1, topRequest.position.z ), false );
				if ( topRequest.pBoob->top )
					PushLoadingList( topRequest.pBoob->top, LongIntPosition( topRequest.position.x, topRequest.position.y, topRequest.position.z+1 ), false );
				if ( topRequest.pBoob->bottom )
					PushLoadingList( topRequest.pBoob->bottom, LongIntPosition( topRequest.position.x, topRequest.position.y, topRequest.position.z-1 ), false );
			}
			if ( topRequest.regenWater )
			{
				if ( topRequest.pBoob->front )
					PushLoadingListWater( topRequest.pBoob->front, LongIntPosition( topRequest.position.x+1, topRequest.position.y, topRequest.position.z ), false );
				if ( topRequest.pBoob->back )
					PushLoadingListWater( topRequest.pBoob->back, LongIntPosition( topRequest.position.x-1, topRequest.position.y, topRequest.position.z ), false );
				if ( topRequest.pBoob->left )
					PushLoadingListWater( topRequest.pBoob->left, LongIntPosition( topRequest.position.x, topRequest.position.y+1, topRequest.position.z ), false );
				if ( topRequest.pBoob->right )
					PushLoadingListWater( topRequest.pBoob->right, LongIntPosition( topRequest.position.x, topRequest.position.y-1, topRequest.position.z ), false );
				if ( topRequest.pBoob->top )
					PushLoadingListWater( topRequest.pBoob->top, LongIntPosition( topRequest.position.x, topRequest.position.y, topRequest.position.z+1 ), false );
				if ( topRequest.pBoob->bottom )
					PushLoadingListWater( topRequest.pBoob->bottom, LongIntPosition( topRequest.position.x, topRequest.position.y, topRequest.position.z-1 ), false );
			}
		}
	}
}

// int GetLoadingListSize()
//  returns current size of the loading queue
int		CVoxelTerrain::GetLoadingListSize ( void )
{
	return (int)vBoobLoadList.size();
}

// float GetLoadingPercent()
//  returns percent to finishing initial load
float CVoxelTerrain::GetLoadingPercent ( void )
{
	return (1.0f-( vBoobLoadList.size() / ((float)(cub( iTerrainSize ))) ));
}

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
}

// Performs recursive update
void CVoxelTerrain::ForceRecursiveUpdate ( CBoob * pBoob, LongIntPosition const& position )
{
	if ( pBoob != NULL )
	{
		if ( pBoob->bitinfo[7] == false )
		{
			PushLoadingList( pBoob, position );

			pBoob->bitinfo[7] = true;

			if ( position.z <= root_position.z )
				ForceRecursiveUpdate( pBoob->bottom, LongIntPosition( position.x, position.y, position.z-1 ) );
			if ( position.x >= root_position.x )
				ForceRecursiveUpdate( pBoob->front, LongIntPosition( position.x+1, position.y, position.z ) );
			if ( position.y >= root_position.y )
				ForceRecursiveUpdate( pBoob->left, LongIntPosition( position.x, position.y+1, position.z ) );
			if ( position.z >= root_position.z )
				ForceRecursiveUpdate( pBoob->top, LongIntPosition( position.x, position.y, position.z+1 ) );
			if ( position.x <= root_position.x )
				ForceRecursiveUpdate( pBoob->back, LongIntPosition( position.x-1, position.y, position.z ) );
			if ( position.y <= root_position.y )
				ForceRecursiveUpdate( pBoob->right, LongIntPosition( position.x, position.y-1, position.z ) );
		}
	}
}

struct tLoadingListComparator
{
	//static LongIntPosition root_pos;
	LongIntPosition root_pos;
	bool operator() ( sVTQueuedLoad si, sVTQueuedLoad sj )
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
//LongIntPosition tLoadingListComparator::root_pos;

// Reorders loading list based on if areas are visible
void CVoxelTerrain::ReorderLoadingList ( CBoob* pBoob )
{
	/*if ( pBoob != NULL )
	{
		if ( pBoob->bitinfo[7] == false )
		{
			// Set boob info true
			pBoob->bitinfo[7] = true;

			// Now, look at if the 

			// Work on all the nearby boobs as well
			if ( position.z <= root_position.z )
				ForceRecursiveUpdate( pBoob->bottom, LongIntPosition( position.x, position.y, position.z-1 ) );
			if ( position.x >= root_position.x )
				ForceRecursiveUpdate( pBoob->front, LongIntPosition( position.x+1, position.y, position.z ) );
			if ( position.y >= root_position.y )
				ForceRecursiveUpdate( pBoob->left, LongIntPosition( position.x, position.y+1, position.z ) );
			if ( position.z >= root_position.z )
				ForceRecursiveUpdate( pBoob->top, LongIntPosition( position.x, position.y, position.z+1 ) );
			if ( position.x <= root_position.x )
				ForceRecursiveUpdate( pBoob->back, LongIntPosition( position.x-1, position.y, position.z ) );
			if ( position.y <= root_position.y )
				ForceRecursiveUpdate( pBoob->right, LongIntPosition( position.x, position.y-1, position.z ) );
		}
	}*/
	mutex::scoped_lock( mtLoadingListGaurd );

	// Now, loop through the entire list
	/*for ( unsigned int i = 0; i < vBoobLoadList.size(); ++i )
	{
		// If the boob is visible, then swap positions with first and current.
		if ( vBoobLoadList[i].pBoob->bitinfo[0] )
		{
			sVTQueuedLoad currentLoad = vBoobLoadList[i];
			vBoobLoadList.erase( vBoobLoadList.begin()+i );
			vBoobLoadList.insert( vBoobLoadList.begin(), currentLoad );
		}
	}*/
	tLoadingListComparator LoadingListComparator;
	LoadingListComparator.root_pos = root_position;
	sort( vBoobLoadList.begin(), vBoobLoadList.end(), LoadingListComparator );

}