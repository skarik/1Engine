
#include "CVoxelTerrain.h"


// int GetSavingListSize()
//  returns current size of the loading queue
int		CVoxelTerrain::GetSavingListSize ( void )
{
	return (int)vBoobSaveList.size();
}

// Look through the loading list and find the target in it
int CVoxelTerrain::IsInSavingList ( CBoob* pBoob )
{
	mutex::scoped_lock( mtSavingListGaurd );
	int i, max_i;
	max_i = (signed)vBoobSaveList.size();
	for ( i = 0; i < max_i; i += 1 )
	{
		if ( vBoobSaveList[i].pBoob == pBoob )
			return i;
	}
	return -1;
}

// Push a new save request onto the list
void CVoxelTerrain::PushSavingList ( CBoob* pBoob, LongIntPosition const& position )
{
	if ( vBoobSaveList.size() > 100000 )
	{
		cout << "WARNING: SAVE LIST OVERLOAD!" << endl;
		return;
	}

	// First check if it's in the saving list
	int index = IsInSavingList( pBoob );
	if ( index >= 0 )
	{
		// If it's in the list, and it's not in first, then bump it up one
		if ( index > 0 )
		{
			sVTQueuedSave temp = vBoobSaveList[index];
			vBoobSaveList[index] = vBoobSaveList[index-1];	// Move next one down
			vBoobSaveList[index-1] = temp;					// Change next one
		}
	}
	else
	{
		// Add a new request to the back
		sVTQueuedSave newRequest;
		newRequest.pBoob = pBoob;
		newRequest.position = position;

		mutex::scoped_lock( mtSavingListGaurd );
		vBoobSaveList.push_back( newRequest );
	}
}

// Pop the query from the list and save it
void CVoxelTerrain::PopSavingList ( void )
{
	if ( !vBoobSaveList.empty() )
	{
		// Increase pop count
		iPopListCount += 1;

		// Get the boob
		sVTQueuedSave topRequest;
		{
			mutex::scoped_lock( mtSavingListGaurd );
			topRequest = vBoobSaveList.front();
		}

		if ( topRequest.pBoob->hasData )
		{
			SaveBoobData( topRequest.pBoob, topRequest.position );
		}

		RemoveFromSavingList( 0 );
	}
}

// Remove the target index from the loading list
void CVoxelTerrain::RemoveFromSavingList ( int targetIndex )
{
	mutex::scoped_lock( mtSavingListGaurd );
	vBoobSaveList.erase( vBoobSaveList.begin()+targetIndex );
}