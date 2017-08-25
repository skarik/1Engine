
#include "COctreeTerrain.h"
#include <array>

#include "CTimeProfiler.h"

uint32_t COctreeTerrain::GetResolutionAtPosition ( const Vector3d& approximatePosition ) const
{
	// Set subdivision target (LODs) based on distance of approximate position from center.
	uint32_t t_subdivision = m_subdivisionTarget;
	// For every power of 2 times blockSize*16 the target is from the distance, double the target
	Vector3d deltaPosition = approximatePosition;// - m_state_centerPosition;
	Real seekDistance = std::max<Real>( abs(deltaPosition.x), std::max<Real>( abs(deltaPosition.y), abs(deltaPosition.z) ) );
	for ( Real lodCount = 2*blockSize*16; lodCount < seekDistance; lodCount *= 2 ) {
		t_subdivision *= 2;
	}
	return t_subdivision;
}
bool COctreeTerrain::PositionInOctree ( const Vector3d& approximatePosition ) const
{
	Vector3d deltaPosition = approximatePosition;// - m_state_centerPosition;
	if ( abs( deltaPosition.z ) > m_treeStartResolution*blockSize*0.5*0.5 )
		return false;
	if ( abs( deltaPosition.x ) > m_treeStartResolution*blockSize*0.5 )
		return false;
	if ( abs( deltaPosition.y ) > m_treeStartResolution*blockSize*0.5 )
		return false;
	return true;
}


void COctreeTerrain::SetDataAtPositionMinimal ( const uint64_t newData, const Vector3d& approximatePosition )
{
	TrackInfo tTracker;
	tTracker.center = Vector3d(0,0,0);//m_state_centerPosition;
	tTracker.subdivision = m_treeStartResolution;
	tTracker.sector = m_root;

	// Check if position in octree to prevent wierd access stuff
	if ( !PositionInOctree( approximatePosition ) ) {
		return;
	}

	// Set subdivision target (LODs) based on distance of approximate position from center.
	uint32_t t_subdivisionTarget = GetResolutionAtPosition( approximatePosition );

	while ( !SeekToPosition( tTracker, approximatePosition, 2 ) ) {
		// Just keep going.
	}
	if ( tTracker.sector == NULL ) {
		return;
	}
	// Is currently locked, and at the target.
	// At the target. If the values don't match, then we need to make things messy. (Yuck.)
	{
		/*if ( !tTracker.sector->data[tTracker.bindex] ) // Not at full possible resolution here.
		{
			// Make use of full resolution.
			tTracker.sector->lock.unlock_shared();
			while ( !SeekToPosition( tTracker, approximatePosition, 2 ) ) {
				// Just keep going (could use a little optimization later)
			}
		}*/
		// Is currently locked, and at target.
		/*if ( tTracker.sector->data[tTracker.bindex] )
		{
			if ( tTracker.sector->info[tTracker.bindex].raw != newData )
			{*/
		//tTracker.sector->lock.unlock_shared(); // Between this line and SectorSubdivide is a race condition. Eh, oh well.
		// Need to begin a write here.
		Terrain::Sector* t_startSector = tTracker.sector;
		t_startSector->BeginWrite();

		// Subdivide the area until subdivision is at target.
		while ( tTracker.subdivision > t_subdivisionTarget ) {
			// There is a race condition that another thread may have subdivided this while we were waiting, so first make sure the area is subdividable
			//try {
			if ( tTracker.sector->data[tTracker.bindex] == true ) {
				SectorSubdivide( tTracker ); // Safe since the top level of this sector is locked.
			}
			/*}
			catch ( std::exception& ) {
				// Nothing. Sometimes it'll just happen.
			}*/
			/*while ( !SeekToPosition( tTracker, approximatePosition, t_subdivisionTarget ) ) {
				// Just keep going (could use a little optimization later)
			}*/
			// Now seek forwards again in the tracker

			//tTracker.sector->lock.unlock_shared();

			// Trace down the tree as far as possible.
			SeekToPosition( tTracker, approximatePosition, t_subdivisionTarget );
		}

		if ( tTracker.sector != t_startSector )
		{
			tTracker.sector->BeginWrite();	// look at these locks
			t_startSector->EndWrite();		// they're kissing
		}

		//Terrain::write_lock lock(tTracker.sector->lock); // Lock before writing

		if ( tTracker.sector->data[tTracker.bindex] ) {
			// Set the data at the target now
			tTracker.sector->info[tTracker.bindex].raw = newData;
		}
		else {
			std::cout << "Attemping to set link" << std::endl;
			throw std::out_of_range("Attemping to set link");
			return;
		}

		tTracker.sector->EndWrite();

			/*}
			else
			{
				// No change needed, just unlock.
				tTracker.sector->lock.unlock_shared();
			}
		}
		else
		{
			std::cout << "Attempted to seek to link at " << (int)tTracker.bindex << std::endl;
			throw std::out_of_range("Attempted to seek to link that is data");
			return;
		}*/
	}

	// Now, minimize sectors
	//Terrain::write_lock lock(tTracker.sector->leafs[8]->lock);
	/*uint repeatCount = 0;
	while ( tTracker.sector->leafs[8] && (repeatCount++ < m_givenRange) )
	{
		tTracker.sector->lock.lock();
		Terrain::write_lock lock(tTracker.sector->leafs[8]->lock);
		//Terrain::write_lock lock(tTracker.sector->lock);
		//tTracker.sector->lock.lock();
		bool condense = true;
		{
			//Terrain::read_lock lock(tTracker.sector->lock); // Lock before reading
			// Look at data
			for ( int i = 0; i < 8; ++i ) {
				if ( (tTracker.sector->data[i] == false) || (tTracker.sector->info[i].raw != newData) ) {
					condense = false;
					break;
				}
			}
		}
		if ( condense )
		{
			Terrain::Sector* currentSector = tTracker.sector;
			//Terrain::write_lock majorlock( currentSector->lock ); // Lock before deletion
			// Go up to parent
			tTracker.sector = (Terrain::Sector*)tTracker.sector->leafs[8];
			tTracker.subdivision *= 2;
			// Find self and delete
			//Terrain::write_lock parentlock(tTracker.sector->lock); // Lock before writing
			int tTarget = -1;
			for ( int i = 0; i < 8; ++i )
			{
				if ( tTracker.sector->leafs[i] == currentSector ) {
					tTarget = i;
					break;
				}
			}
			if ( tTarget != -1 ) {
				tTracker.sector->leafs[tTarget] = NULL;
				tTracker.sector->info[tTarget].raw = newData;
				tTracker.sector->data[tTarget] = true;

				currentSector->lock.unlock();
				FreeDataBlock( currentSector );
				//tTracker.sector->links[i] = newData;
				//tTracker.sector->lock.unlock();
				continue;
			}
			// Couldn't find self. Got a problem.
			//throw std::out_of_range("Could not find child during condensing");
			//currentSector = NULL;
			// (will happen if two things are working near the same area)
			// so just leave
			//tTracker.sector->lock.unlock();
			currentSector->lock.unlock();
		}
		else
		{
			tTracker.sector->lock.unlock();
			//tTracker.sector->leafs[8]->lock.unlock();
			break;
		}
	}*/
}


// Returns true when it gets to the target position
bool COctreeTerrain::SeekToPosition ( TrackInfo& n_currentArea, const Vector3d& approximateTarget, const uint32_t n_subdivisionTarget )
{
	if ( n_currentArea.sector == NULL ) {
		return true;
	}
	// Returns true when can't subdivide any more.
	if ( n_currentArea.subdivision == n_subdivisionTarget ) {
		int targetIndex =
			( (approximateTarget.x > n_currentArea.center.x)?1:0 )+
			( (approximateTarget.y > n_currentArea.center.y)?2:0 )+
			( (approximateTarget.z > n_currentArea.center.z)?(n_currentArea.sector->linkstyle?0:4):0 );
		n_currentArea.bindex = targetIndex;
		// Will be used, so lock for read.
		//n_currentArea.sector->lock.lock_shared();
		return true;
	}
	else {
		// Return true on reach the deepest area. (Can't subdivide any more)
		int targetIndex =
			( (approximateTarget.x > n_currentArea.center.x)?1:0 )+
			( (approximateTarget.y > n_currentArea.center.y)?2:0 )+
			( (approximateTarget.z > n_currentArea.center.z)?(n_currentArea.sector->linkstyle?0:4):0 );
		//Terrain::read_lock lock(n_currentArea.sector->lock); // Lock before reading
		//n_currentArea.sector->lock.lock_shared();
		if ( n_currentArea.sector->data[targetIndex] ) {
			// Don't unlock. Still will be used.
			n_currentArea.bindex = targetIndex;
			return true;
		}
		else {
			// If can go to a deeper subdivision, go to it.
			Terrain::Sector* previousSector = n_currentArea.sector;
			n_currentArea.sector = n_currentArea.sector->leafs[targetIndex];
			n_currentArea.bindex = targetIndex;
			n_currentArea.center = n_currentArea.center + Vector3d(
				((targetIndex%2)-0.5f)		*n_currentArea.subdivision*blockSize*0.5f,
				(((targetIndex/2)%2)-0.5f)	*n_currentArea.subdivision*blockSize*0.5f,
				((targetIndex/4)-0.5f)		*n_currentArea.subdivision*blockSize*0.5f * (previousSector->linkstyle?0:1)
				);
			n_currentArea.subdivision = n_currentArea.subdivision/2;
			// Unlock as read
			//previousSector->lock.unlock_shared();
			// Return false on not reach
			return (n_currentArea.sector==NULL);
		}
	}
}

// Subdivides the area at the given data into copies.
void COctreeTerrain::SectorSubdivide ( const TrackInfo& n_targetArea )
{
	//Terrain::write_lock lock(n_targetArea.sector->lock); // Lock before writing

	if ( n_targetArea.sector->data[n_targetArea.bindex] == false ) { // Is pointer data already, so throw error
		throw std::exception( "Attempted to allocate pointer on a pointer set" );
		std::cout << "OCTREE: Attempted to allocate pointer on a pointer set!" << std::endl;
	}
	
	// Get current data to copy across new children
	uint64_t sourceData = n_targetArea.sector->info[n_targetArea.bindex].raw;

	// Mark as pointer
	n_targetArea.sector->data[n_targetArea.bindex] = false;

	// Get new sector
	Terrain::Sector* newSector;
	NewDataBlock( &newSector );
	// Mark sector as unusable until this function is done
	//Terrain::write_lock sublock( newSector->lock ); // don't need to.
	// Set the parent
	newSector->leafs[8] = n_targetArea.sector;
	// Set sector data
	for ( int i = 0; i < 8; ++i ) {
		newSector->info[i].raw = sourceData;
	}
	// Set the new child
	n_targetArea.sector->leafs[n_targetArea.bindex] = newSector;

	// And it's now subdivided
}
// Deletes the area at the given data. Will attempt to save data
void COctreeTerrain::SectorDelete ( TrackInfo& n_targetArea )
{
	//Terrain::write_lock lock(n_targetArea.sector->lock); // Lock before writing
	//n_targetArea.sector->BeginWrite();

	// Create the recursive delete functor (only this function should have access to this)
	struct
	{
		void operator() ( Terrain::Sector* n_sector, uint64_t& rawValue )
		{
			n_sector->BeginWrite(); // Mark for write to wait for readers to leave
			for ( int i = 0; i < 8; ++i ) {
				// If data, check if it's a block to grab
				if ( n_sector->data[i] ) {
					Terrain::terra_b rawcheck;
					rawcheck.raw = rawValue;
					// Grab the denser block
					if ( n_sector->info[i].block > rawcheck.block ) {
						rawValue = n_sector->info[i].raw;
					}
				}
				// If its a pointer, delete that data
				else {
					(*this)( n_sector->leafs[i], rawValue );
				}
			}
			// And now delete self
			master->FreeDataBlock( n_sector );
		}
		COctreeTerrain* master;
	} DeleteRecursiveOperation;
	DeleteRecursiveOperation.master = this; // Set master so can use memory deallocation

	// Perform the deletion
	uint64_t tBlockRaw = 0; // start with an empty block
	DeleteRecursiveOperation( n_targetArea.sector->leafs[n_targetArea.bindex], tBlockRaw );

	// Set new data
	n_targetArea.sector->data[n_targetArea.bindex] = true;
	n_targetArea.sector->leafs[n_targetArea.bindex] = NULL;
	n_targetArea.sector->info[n_targetArea.bindex].raw = tBlockRaw;

	// End the write
	//n_targetArea.sector->EndWrite();
}

// Subdivides terrain into a shiftable set
void COctreeTerrain::SubdivideStart ( Terrain::Sector* n_currentSector, const uint32_t n_currentSubdiv )
{
	if ( n_currentSubdiv > 32 )
	{
		for ( int i = 0; i < ((n_currentSector->linkstyle)?4:8); ++i )
		{
			// Create tracker information about the current sector.
			TrackInfo tTracker;
			tTracker.bindex = i;
			tTracker.sector = n_currentSector;
			// If the current sector is data
			if ( tTracker.sector->data[i] == true ) {
				// Feed that information into the subdivision routine.
				SectorSubdivide( tTracker );
			}
			// Run this routine on the current subset.
			SubdivideStart( n_currentSector->leafs[i], n_currentSubdiv/2 );
		}
	}
}

// Subdivides the sector into a full subdivided set
void COctreeTerrain::SubdivideSideBuffer ( Terrain::Sector* n_currentSector, const uint32_t n_currentSubdiv )
{
	if ( n_currentSubdiv > 2 )
	{
		for ( int i = 0; i < 8; ++i )
		{
			// Create tracker information about the current sector.
			TrackInfo tTracker;
			tTracker.bindex = i;
			tTracker.sector = n_currentSector;
			// If the current sector is data
			if ( tTracker.sector->data[i] == true ) {
				// Feed that information into the subdivision routine.
				SectorSubdivide( tTracker );
			}
			// Run this routine on the current subset.
			SubdivideSideBuffer( n_currentSector->leafs[i], n_currentSubdiv/2 );
		}
	}
}
// Travel prototype (side-buffers use them too)
void Travel ( TrackInfo& trackInfo, uint32_t n_subXtarget, uint32_t n_subYtarget, uint32_t n_subZtarget, uint32_t n_targetSubdivision );
// Sets data at side-buffer position
void COctreeTerrain::SetDataAtSidebufferPosition ( Terrain::Sector* n_sideBuffer, const uint64_t newData, const RangeVector& absoluteIndex )
{
	TrackInfo tTracker;
	tTracker.sector = n_sideBuffer;
	tTracker.subdivision = 32;
	Travel( tTracker, absoluteIndex.x, absoluteIndex.y, absoluteIndex.z, 1 );

	if ( !tTracker.sector->data[tTracker.bindex] ) {
		throw std::exception( "Incorrect subdivision!" );
	}
	tTracker.sector->info[tTracker.bindex].raw = newData;
}
// Gets data at side-buffer position
void COctreeTerrain::GetDataAtSidebufferPosition ( Terrain::Sector* n_sideBuffer, uint64_t& o_data, const RangeVector& absoluteIndex )
{
	TrackInfo tTracker;
	tTracker.sector = n_sideBuffer;
	tTracker.subdivision = 32;
	Travel( tTracker, absoluteIndex.x, absoluteIndex.y, absoluteIndex.z, 1 );

	if ( !tTracker.sector->data[tTracker.bindex] ) {
		throw std::exception( "Incorrect subdivision!" );
	}
	o_data = tTracker.sector->info[tTracker.bindex].raw;
}


// Go to sector specified by the bitmasks
void Travel ( TrackInfo& trackInfo, uint32_t n_subXtarget, uint32_t n_subYtarget, uint32_t n_subZtarget, uint32_t n_targetSubdivision )
{
	/*trackInfo.subdivision /= 2;
	trackInfo.bindex = 
			((n_subXtarget & trackInfo.subdivision)?1:0) +
			((n_subYtarget & trackInfo.subdivision)?2:0) +
			((n_subZtarget & trackInfo.subdivision)?(trackInfo.sector->linkstyle?0:4):0);
	if ( trackInfo.subdivision != n_targetSubdivision )
	{
		if ( trackInfo.sector->data[trackInfo.bindex] || trackInfo.sector->leafs[trackInfo.bindex] == NULL ) {
			throw std::out_of_range( "Travelled too far!" );
			std::cout << "Travelled too far!" << std::endl;
		}
		trackInfo.sector = trackInfo.sector->leafs[trackInfo.bindex];
		Travel ( trackInfo, n_subXtarget, n_subYtarget, n_subZtarget, n_targetSubdivision );
	}*/
	while ( true )
	{
		trackInfo.subdivision /= 2;
		trackInfo.bindex = 
			((n_subXtarget & trackInfo.subdivision)?1:0) +
			((n_subYtarget & trackInfo.subdivision)?2:0) +
			((n_subZtarget & trackInfo.subdivision)?(trackInfo.sector->linkstyle?0:4):0);
		if ( trackInfo.subdivision != n_targetSubdivision )
		{
			if ( trackInfo.sector->data[trackInfo.bindex] || trackInfo.sector->leafs[trackInfo.bindex] == NULL ) {
				throw std::out_of_range( "Travelled too far!" );
				std::cout << "Travelled too far!" << std::endl;
			}
			trackInfo.sector = trackInfo.sector->leafs[trackInfo.bindex];
		}
		else
		{
			break;
		}
	}
}
// Shift all data at the given subdivision to a direction
void COctreeTerrain::ShiftData ( const uint32_t n_subdivisionTarget, const ushort n_shiftDirection )
{
	// Subdivide the entire terrain first to allow for tighter movement
	SubdivideStart( m_root, m_treeStartResolution );

	// Lock the root now after editing is done.
	//Terrain::write_lock root_lock( m_root->lock );
	m_root->BeginWrite();

	// Shit. So, we can seek down to an area, no problem. The problem is more of access.
	// We need to iterate to both the source, and the target. This is best done in slices.
	// Do move in slices that are perpendicular to the current direction.
	// If we move in X, we iterate across Z,Y, then X.

	TrackInfo tTracker;
	tTracker.center = Vector3d(0,0,0);//m_state_centerPosition;
	tTracker.subdivision = m_treeStartResolution;
	tTracker.sector = m_root;

	int32_t *trSubmajor0, *trSubmajor1;
	int32_t *tSubmajorMax0, *tSubmajorMax1;
	int32_t *trMajor0;
	int32_t *tMajorMax0;
	int32_t subX, subY, subZ;
	int32_t tMaxX, tMaxY, tMaxZ;

	tMaxX = (int32_t)m_treeStartResolution;
	tMaxY = (int32_t)m_treeStartResolution;
	tMaxZ = (int32_t)m_treeStartResolution/2;

	// Set the direction based on the axes
#define SET_AXES(MINOR0,MINORmax0,MINOR1,MINORmax1,MAJOR,MAJORmax)\
	trSubmajor0		= &MINOR0;		\
	tSubmajorMax0	= &MINORmax0;	\
	trSubmajor1		= &MINOR1;		\
	tSubmajorMax1	= &MINORmax1;	\
	trMajor0		= &MAJOR;		\
	tMajorMax0		= &MAJORmax;

	if ( n_shiftDirection == X_POS || n_shiftDirection == X_NEG ) {
		SET_AXES(subZ,tMaxZ,subY,tMaxY,subX,tMaxX);
	}
	else if ( n_shiftDirection == Y_POS || n_shiftDirection == Y_NEG ) {
		SET_AXES(subZ,tMaxZ,subX,tMaxX,subY,tMaxY);
	}
	else if ( n_shiftDirection == Z_POS || n_shiftDirection == Z_NEG ) {
		SET_AXES(subX,tMaxX,subY,tMaxY,subZ,tMaxZ);
	}
#undef SET_AXES

	if ( n_shiftDirection == X_POS || n_shiftDirection == Y_POS || n_shiftDirection == Z_POS )
	{
		// This shifts the data using pointers to coordinates, only on the negative direction.
		for ( *trSubmajor0 = 0; *trSubmajor0 < *tSubmajorMax0; *trSubmajor0 += n_subdivisionTarget )
		{
			for ( *trSubmajor1 = 0; *trSubmajor1 < *tSubmajorMax1; *trSubmajor1 += n_subdivisionTarget )
			{
				TrackInfo tTrackerCurrent;
				TrackInfo tTrackerPrevious;
				for ( *trMajor0 = 0; *trMajor0 < *tMajorMax0; *trMajor0 += n_subdivisionTarget )
				{
					tTrackerPrevious = tTrackerCurrent;
					tTrackerCurrent = tTracker;
					Travel( tTrackerCurrent, subX, subY, subZ, n_subdivisionTarget );
					if ( *trMajor0 == 0 ) {
						// Free up tTrackerPrevious's information
						// (Recursive free on tTrackerPrevious)
						tTrackerCurrent.sector->BeginWrite();
						SectorDelete( tTrackerCurrent );
						tTrackerCurrent.sector->EndWrite();
						continue;
					}

					// Move tracker current's data to tracker previous
					tTrackerPrevious.sector->data[tTrackerPrevious.bindex] = tTrackerCurrent.sector->data[tTrackerCurrent.bindex];
					tTrackerPrevious.sector->info[tTrackerPrevious.bindex] = tTrackerCurrent.sector->info[tTrackerCurrent.bindex];
					tTrackerPrevious.sector->leafs[tTrackerPrevious.bindex] = tTrackerCurrent.sector->leafs[tTrackerCurrent.bindex];
				}
				// And clear out tTrackerCurrent's information
				tTrackerCurrent.sector->data[tTrackerCurrent.bindex] = true;
				tTrackerCurrent.sector->info[tTrackerCurrent.bindex].raw = 0;
				tTrackerCurrent.sector->leafs[tTrackerCurrent.bindex] = NULL;
			}
		}
	}
	else
	{
		// This shifts the data using pointers to coordinates, only on the positive direction.
		for ( *trSubmajor0 = 0; *trSubmajor0 < *tSubmajorMax0; *trSubmajor0 += n_subdivisionTarget )
		{
			for ( *trSubmajor1 = 0; *trSubmajor1 < *tSubmajorMax1; *trSubmajor1 += n_subdivisionTarget )
			{
				TrackInfo tTrackerCurrent;
				TrackInfo tTrackerPrevious;
				for ( *trMajor0 = *tMajorMax0-n_subdivisionTarget; *trMajor0 >= 0; *trMajor0 -= n_subdivisionTarget )
				{
					tTrackerPrevious = tTrackerCurrent;
					tTrackerCurrent = tTracker;
					Travel( tTrackerCurrent, subX, subY, subZ, n_subdivisionTarget );
					if ( *trMajor0 == *tMajorMax0-n_subdivisionTarget ) {
						// Free up tTrackerPrevious's information
						// (Recursive free on tTrackerPrevious)
						tTrackerCurrent.sector->BeginWrite();
						SectorDelete( tTrackerCurrent );
						tTrackerCurrent.sector->EndWrite();
						continue;
					}

					// Move tracker current's data to tracker previous
					tTrackerPrevious.sector->data[tTrackerPrevious.bindex] = tTrackerCurrent.sector->data[tTrackerCurrent.bindex];
					tTrackerPrevious.sector->info[tTrackerPrevious.bindex] = tTrackerCurrent.sector->info[tTrackerCurrent.bindex];
					tTrackerPrevious.sector->leafs[tTrackerPrevious.bindex] = tTrackerCurrent.sector->leafs[tTrackerCurrent.bindex];
				}
				// And clear out tTrackerCurrent's information
				tTrackerCurrent.sector->data[tTrackerCurrent.bindex] = true;
				tTrackerCurrent.sector->info[tTrackerCurrent.bindex].raw = 0;
				tTrackerCurrent.sector->leafs[tTrackerCurrent.bindex] = NULL;
			}
		}
	}

	m_root->EndWrite();
	return;
	/*
	// This shifts 32 to X-, essenstially moves the root +64X
	uint32_t tSubdivideCount = m_treeStartResolution/n_subdivisionTarget;
	for ( uint32_t subZ = 0; subZ < m_treeStartResolution/2; subZ += n_subdivisionTarget )
	{
		for ( uint32_t subY = 0; subY < m_treeStartResolution; subY += n_subdivisionTarget )
		{
			TrackInfo tTrackerCurrent;
			TrackInfo tTrackerPrevious;
			for ( uint32_t subX = 0; subX < m_treeStartResolution; subX += n_subdivisionTarget )
			{
				tTrackerPrevious = tTrackerCurrent;
				tTrackerCurrent = tTracker;
				Travel( tTrackerCurrent, subX, subY, subZ, n_subdivisionTarget );
				if ( subX == 0 ) {
					// Free up tTrackerPrevious's information
					// (Recursive free on tTrackerPrevious)
					continue;
				}

				// Move tracker current's data to tracker previous
				tTrackerPrevious.sector->data[tTrackerPrevious.bindex] = tTrackerCurrent.sector->data[tTrackerCurrent.bindex];
				tTrackerPrevious.sector->info[tTrackerPrevious.bindex] = tTrackerCurrent.sector->info[tTrackerCurrent.bindex];
				tTrackerPrevious.sector->leafs[tTrackerPrevious.bindex] = tTrackerCurrent.sector->leafs[tTrackerCurrent.bindex];
			}
			// And clear out tTrackerCurrent's information
			tTrackerCurrent.sector->data[tTrackerCurrent.bindex] = true;
			tTrackerCurrent.sector->info[tTrackerCurrent.bindex].raw = 0;
			tTrackerCurrent.sector->leafs[tTrackerCurrent.bindex] = NULL;
		}
	}
	*/
}


// Returns true when it gets to the target position.
// Differs from SeekTo in that it will traverse the heirarchy upwards
bool COctreeTerrain::TraverseToPosition ( TrackInfo& n_currentArea, const Vector3d& approximateTarget )
{
	if ( n_currentArea.sector == NULL ) {
		return true;
	}
	// Check if the target is in the current subdivision.
	if (abs(approximateTarget.x - n_currentArea.center.x) < n_currentArea.subdivision &&
		abs(approximateTarget.y - n_currentArea.center.y) < n_currentArea.subdivision &&
		abs(approximateTarget.z - n_currentArea.center.z) < n_currentArea.subdivision )
	{
		// Returns true when can't subdivide any more.
		if ( n_currentArea.subdivision == m_currentSubdivisionTarget ) {
			int targetIndex =
				( (approximateTarget.x > n_currentArea.center.x)?1:0 )+
				( (approximateTarget.y > n_currentArea.center.y)?2:0 )+
				( (approximateTarget.z > n_currentArea.center.z)?(n_currentArea.sector->linkstyle?0:4):0 );
			n_currentArea.bindex = targetIndex;
			return true;
		}
		else {
			// Return true on reach the deepest area. (Can't subdivide any more)
			int targetIndex =
				( (approximateTarget.x > n_currentArea.center.x)?1:0 )+
				( (approximateTarget.y > n_currentArea.center.y)?2:0 )+
				( (approximateTarget.z > n_currentArea.center.z)?(n_currentArea.sector->linkstyle?0:4):0 );
			//Terrain::read_lock lock(n_currentArea.sector->lock); // Lock before reading
			if ( n_currentArea.sector->data[targetIndex] ) {
				n_currentArea.bindex = targetIndex;
				return true;
			}
			else {
				// If can go to a deeper subdivision, go to it.
				Terrain::Sector* previousSector = n_currentArea.sector;
				n_currentArea.sector = n_currentArea.sector->leafs[targetIndex];
				n_currentArea.bindex = targetIndex;
				n_currentArea.center = n_currentArea.center + Vector3d(
					((targetIndex%2)-0.5f)		*n_currentArea.subdivision*blockSize*0.5f,
					(((targetIndex/2)%2)-0.5f)	*n_currentArea.subdivision*blockSize*0.5f,
					((targetIndex/4)-0.5f)		*n_currentArea.subdivision*blockSize*0.5f * (previousSector->linkstyle?0:1)
					);
				n_currentArea.subdivision = n_currentArea.subdivision/2;
				// Return false on not reach
				return (n_currentArea.sector==NULL);
			}
		}
	}
	else
	{
		//Terrain::read_lock lock(n_currentArea.sector->lock); // Lock before reading
		// It's not in this subdivision. We need to go up to our parent.
		n_currentArea.sector = n_currentArea.sector->leafs[8];
		n_currentArea.subdivision = n_currentArea.subdivision*2;
		n_currentArea.center = n_currentArea.center + Vector3d(
			floor(n_currentArea.center.x/(n_currentArea.subdivision*blockSize)) + n_currentArea.subdivision*blockSize*0.5f,
			floor(n_currentArea.center.y/(n_currentArea.subdivision*blockSize)) + n_currentArea.subdivision*blockSize*0.5f,
			floor(n_currentArea.center.z/(n_currentArea.subdivision*blockSize)) + n_currentArea.subdivision*blockSize*0.5f
			);
		// Return false on not reach.
		return (n_currentArea.sector==NULL);
	}
}

void COctreeTerrain::SampleBlock ( const Vector3d& n_approximatePosition, uint64_t& o_data )
{
	const string qdic = std::to_string((int)&o_data);
	// Check if position in octree to prevent wierd access stuff
	if ( !PositionInOctree( n_approximatePosition ) ) {
		o_data = 0;
		return;
	}

	TrackInfo tTrackerSource;
	tTrackerSource.center = Vector3d(0,0,0);//m_state_centerPosition;
	tTrackerSource.subdivision = m_treeStartResolution;
	tTrackerSource.sector = m_root;

	TrackInfo tTracker;
	tTracker = tTrackerSource;

	// Read through the loop, saving the read access in the stack
	Terrain::Sector* accesses [16];
	int8_t accesscount = 0;
	bool hitEnd;
	do {
		tTracker.sector->BeginRead();
		accesses[accesscount++] = tTracker.sector;
		hitEnd = SeekToPosition( tTracker, n_approximatePosition, 2 );
	} while ( hitEnd == false );
	// At the target, now read the data
	if ( tTracker.sector ) {
		tTracker.sector->BeginRead();
		o_data = tTracker.sector->info[tTracker.bindex].raw;
		tTracker.sector->EndRead();
	}
	else {
		o_data = 0;
	}
	// Free all accesses aggrued to the stack
	while ( accesscount > 0 ) {
		accesses[--accesscount]->EndRead();
	}

	// Read through the loop, requesting read access every iteration
	/*bool hitEnd;
	Terrain::Sector* t_startSector;
	do {
		t_startSector = tTracker.sector;
		t_startSector->BeginRead();
		hitEnd = SeekToPosition( tTracker, n_approximatePosition, 2 );
		if ( !hitEnd ) {
			t_startSector->EndRead();
		}
	} while ( hitEnd == false );

	if ( tTracker.sector ) {
		o_data = tTracker.sector->info[tTracker.bindex].raw;
	}
	else {
		o_data = 0;
	}
	t_startSector->EndRead();*/
}
void COctreeTerrain::SampleNormal ( const Vector3d& n_approximatePosition, Vector3d& o_normal )
{
	// Check if position in octree to prevent wierd access stuff
	if ( !PositionInOctree( n_approximatePosition ) ) {
		o_normal = Vector3d(0,0,0);
		return;
	}

	throw std::exception("Out of date function");

	// HIlariously all wrong
	Terrain::terra_b tSampledVoxel;

	TrackInfo tTrackerSource;
	tTrackerSource.center = Vector3d(0,0,0);//m_state_centerPosition;
	tTrackerSource.subdivision = m_treeStartResolution;
	tTrackerSource.sector = m_root;

	TrackInfo tTracker;
	tTracker = tTrackerSource;

	// Read through the loop, saving the read access in a stack
	Terrain::Sector* accesses [16];
	int8_t accesscount = 0;
	bool hitEnd;
	do {
		tTracker.sector->BeginRead();
		accesses[accesscount++] = tTracker.sector;
		hitEnd = SeekToPosition( tTracker, n_approximatePosition, 2 );
	} while ( hitEnd == false );
	// At the target, now read the data
	if ( tTracker.sector ) {
		tTracker.sector->BeginRead();
		tSampledVoxel = tTracker.sector->info[tTracker.bindex];
		o_normal.x = (Real(tSampledVoxel.normal_y_x)-7);
		o_normal.y = (Real(tSampledVoxel.normal_x_y)-7);
		o_normal.z = (Real(tSampledVoxel.normal_x_z)-7);
		o_normal.normalize();
		tTracker.sector->EndRead();
	}
	else {
		o_normal = Vector3d(0,0,0);
	}
	// Free all accesses aggrued to the stack
	while ( accesscount > 0 ) {
		accesses[--accesscount]->EndRead();
	}
}

void COctreeTerrain::SampleSector ( const Vector3d& n_approximatePosition, Terrain::Sector*& o_sector, const uint32_t n_subdivisionTarget )
{
	// Check if position in octree to prevent wierd access stuff
	if ( !PositionInOctree( n_approximatePosition ) ) {
		o_sector = NULL;
		return;
	}

	TrackInfo tTrackerSource;
	tTrackerSource.center = Vector3d(0,0,0);//m_state_centerPosition;
	tTrackerSource.subdivision = m_treeStartResolution;
	tTrackerSource.sector = m_root;

	TrackInfo tTracker;
	tTracker = tTrackerSource;
	/*while ( !SeekToPosition( tTracker, n_approximatePosition, n_subdivisionTarget ) ) {}
	if ( tTracker.sector ) {
		if ( tTracker.sector->data[tTracker.bindex] ) {
			o_sector = NULL;
		}
		else {
			o_sector = tTracker.sector->leafs[tTracker.bindex];
		}
	}
	else {
		o_sector = NULL;
	}
	tTracker.sector->lock.unlock_shared();*/

	// Read through the loop, saving the read access in the stack
	Terrain::Sector* accesses [16];
	int8_t accesscount = 0;
	bool hitEnd;
	do {
		tTracker.sector->BeginRead();
		accesses[accesscount++] = tTracker.sector;
		hitEnd = SeekToPosition( tTracker, n_approximatePosition, n_subdivisionTarget );
	} while ( hitEnd == false );
	// At the target, now read the data
	if ( tTracker.sector ) {
		tTracker.sector->BeginRead();
		if ( tTracker.sector->data[tTracker.bindex] ) {
			o_sector = NULL;
		}
		else {
			o_sector = tTracker.sector->leafs[tTracker.bindex];
		}
		tTracker.sector->EndRead();
	}
	else {
		o_sector = NULL;
	}
	// Free all accesses aggrued to the stack
	while ( accesscount > 0 ) {
		accesses[--accesscount]->EndRead();
	}
}

void COctreeTerrain::SampleBlockExpensive ( const Vector3d& n_approximatePosition, BlockTrackInfo* o_tracker )
{
	// Check if position in octree to prevent wierd access stuff
	if ( !PositionInOctree( n_approximatePosition ) ) {
		o_tracker->block.raw = 0;
		o_tracker->valid = false;
		return;
	}

	// Track to target
	TrackInfo tTrackerSource;
	tTrackerSource.center = Vector3d(0,0,0);
	tTrackerSource.subdivision = m_treeStartResolution;
	tTrackerSource.sector = m_root;

	TrackInfo tTracker;
	tTracker = tTrackerSource;
	while ( !SeekToPosition( tTracker, n_approximatePosition, 2 ) )
	{
		if ( tTracker.sector ) {
			o_tracker->pos_x |= tTracker.subdivision * (tTracker.bindex%2);
			o_tracker->pos_y |= tTracker.subdivision * ((tTracker.bindex/2)%2);
			o_tracker->pos_z |= tTracker.subdivision * (tTracker.bindex/4);
		}
	}
	/*// Save tracker data
	o_tracker->bindex = tTracker.bindex;
	o_tracker->center = tTracker.center;
	o_tracker->sector = tTracker.sector;
	o_tracker->subdivision = tTracker.subdivision;

	// Grab output value
	if ( tTracker.sector ) {
		// Mark last position
		o_tracker->pos_x |= tTracker.subdivision * (tTracker.bindex%2);
		o_tracker->pos_y |= tTracker.subdivision * ((tTracker.bindex/2)%2);
		o_tracker->pos_z |= tTracker.subdivision * (tTracker.bindex/4);

		// Save data
		o_tracker->block.raw = tTracker.sector->info[tTracker.bindex].raw;
		o_tracker->pBlock = &(tTracker.sector->info[tTracker.bindex]);
		// Now that pBlock has a value, mark tracker as valid
		o_tracker->valid = true;

		if ( o_tracker->block.block == EB_NONE ) {
			//throw std::exception();
			//printf( "Shit\n" );
		}
	}
	else {
		// Bad sector found, so mark as invalid
		o_tracker->valid = false;
	}
	tTracker.sector->lock.unlock_shared();*/

	// Read through the loop, saving the read access in a stack
	std::list<Terrain::Sector*> accesses;
	bool hitEnd;
	do {
		tTracker.sector->BeginRead();
		accesses.push_back( tTracker.sector );
		hitEnd = SeekToPosition( tTracker, n_approximatePosition, 2 );
		if ( tTracker.sector ) {
			o_tracker->pos_x |= tTracker.subdivision * (tTracker.bindex%2);
			o_tracker->pos_y |= tTracker.subdivision * ((tTracker.bindex/2)%2);
			o_tracker->pos_z |= tTracker.subdivision * (tTracker.bindex/4);
		}
	} while ( hitEnd == false );

	// Save tracker data
	o_tracker->bindex = tTracker.bindex;
	o_tracker->center = tTracker.center;
	o_tracker->sector = tTracker.sector;
	o_tracker->subdivision = tTracker.subdivision;

	// Grab output value
	if ( tTracker.sector ) {
		tTracker.sector->BeginRead();

		// Mark last position
		o_tracker->pos_x |= tTracker.subdivision * (tTracker.bindex%2);
		o_tracker->pos_y |= tTracker.subdivision * ((tTracker.bindex/2)%2);
		o_tracker->pos_z |= tTracker.subdivision * (tTracker.bindex/4);

		// Save data
		o_tracker->block.raw = tTracker.sector->info[tTracker.bindex].raw;
		o_tracker->pBlock = &(tTracker.sector->info[tTracker.bindex]);
		// Now that pBlock has a value, mark tracker as valid
		o_tracker->valid = true;

		if ( o_tracker->block.block == EB_NONE ) {
			//throw std::exception();
			//printf( "Shit\n" );
		}

		tTracker.sector->EndRead();
	}
	else {
		// Bad sector found, so mark as invalid
		o_tracker->valid = false;
	}

	// Free all accesses aggrued to the stack
	while ( !accesses.empty() ) {
		accesses.front()->EndRead();
		accesses.pop_front();
	}
}