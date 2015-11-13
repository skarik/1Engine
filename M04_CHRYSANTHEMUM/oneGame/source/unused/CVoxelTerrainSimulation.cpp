
#include "CVoxelTerrain.h"
#include "CTimeProfiler.h"
#include <list>
using std::list;
using std::find;

using std::cout;
using std::endl;

// Run simulation
void CVoxelTerrain::UpdateSimulation ( CBoob * pBoob, RangeVector const& position )
{
	if ( pBoob != NULL )
	{
		if ( pBoob->bitinfo[7] == false )
		{
			pBoob->bitinfo[7] = true;
			
			// Simulate the far areas first
			if ( position.z <= root_position.z )
				UpdateSimulation( pBoob->bottom, RangeVector( position.x, position.y, position.z-1 ) );
			if ( position.x >= root_position.x )
				UpdateSimulation( pBoob->front, RangeVector( position.x+1, position.y, position.z ) );
			if ( position.y >= root_position.y )
				UpdateSimulation( pBoob->left, RangeVector( position.x, position.y+1, position.z ) );
			if ( position.z >= root_position.z )
				UpdateSimulation( pBoob->top, RangeVector( position.x, position.y, position.z+1 ) );
			if ( position.x <= root_position.x )
				UpdateSimulation( pBoob->back, RangeVector( position.x-1, position.y, position.z ) );
			if ( position.y <= root_position.y )
				UpdateSimulation( pBoob->right, RangeVector( position.x, position.y-1, position.z ) );

			// Simulate the boob
			if (( pBoob->current_resolution == 1 )&&( pBoob->hasBlockData ))
			{
				// Update water
				if ( iWaterSimulation % 5 == 0 )
				{
					UpdateWater_Low32( pBoob, position );
				}
				if ( pBoob == root )
					TimeProfiler.BeginTimeProfile( "terraSimu" );
				// Reset wire
				if ( bWireQueueUpdate )
				{
					UpdateWire_Reset32( pBoob, position );
				}
				// Update pre simulation
				for ( CBoob::ComponentIterator it = pBoob->v_component.begin(); it != pBoob->v_component.end(); ++it )
				{
					(*it)->PreSimulation();
				}
				// Update wire
				if ( bWireQueueUpdate )
				{
					UpdateWire_Default32( pBoob, position );
				}
				// Update post simulation
				for ( CBoob::ComponentIterator it = pBoob->v_component.begin(); it != pBoob->v_component.end(); ++it )
				{
					(*it)->PostSimulation();
				}
				if ( pBoob == root )
					TimeProfiler.EndTimeProfile( "terraSimu" );
					//TimeProfiler.EndPrintTimeProfile( "terraSimu" );

				// grass stuff
				if ( pBoob->pGrass )
					pBoob->pGrass->Simulate();
			}
			

			// Vegetation growing should also be here.
		}
	}
}

// Perform low res water simulation
void CVoxelTerrain::UpdateWater_Low32 ( CBoob * pBoob, RangeVector const& position )
{
	bWaterUpdated = false;

	// Update current boob
	for ( char i = 0; i < 8; i += 1 )
	{
		if ( !pBoob->solid[i] ) {
			UpdateWater_Low16( pBoob, &(pBoob->data[i]), i, position );
		}
	}
	//pBoob->pMesh->needUpdateOnWaterVBO = true;
	if ( bWaterUpdated ) {
		PushLoadingListWater( pBoob, position, false );
		if ( pBoob->top ) {
			PushLoadingListWater( pBoob->top, pBoob->top->li_position, false );
		}
		if ( pBoob->bottom ) {
			PushLoadingListWater( pBoob->bottom, pBoob->bottom->li_position, false );
		}
	}
}
void CVoxelTerrain::UpdateWater_Low16 ( CBoob * pBoob, subblock16 * block, char const index, RangeVector const& position )
{
	// Update current block vbo
	for ( char i = 0; i < 8; i += 1 )
	{
		UpdateWater_Low8( pBoob, &(block->data[i]), index,i, position );
	}
}
void CVoxelTerrain::UpdateWater_Low8 ( CBoob * pBoob, subblock8 * block, char const index, char const subindex, RangeVector const& position )
{
	// Draw current block
	char res = pBoob->current_resolution;
	char i, j, k;
	for ( k = 0; k < 8; k += res )
	{
		for ( j = 0; j < 8; j += res )
		{
			for ( i = 0; i < 8; i += res )
			{
				if ( block->data[i+j*8+k*64].block == EB_WATER )
				{
					UpdateWater_Low1( pBoob, block, index, subindex, i+j*8+k*64, position );
					bWaterUpdated = true;
				}
			}
		}
	}
}


// internal reset
#define RESET_VALS	pCurBoob	= pBoob;\
					pCurBlock	= block;\
					curBlock16	= index;\
					curBlock8	= subindex;\
					curBlockD	= i;
void CVoxelTerrain::UpdateWater_Low1 ( CBoob * pBoob, subblock8 * block, char const index, char const subindex, short const i, RangeVector const& position )
{
	// initial vals
	static CBoob *		pCurBoob;
	static subblock8 *	pCurBlock;
	static char			curBlock16;
	static char			curBlock8;
	static short		curBlockD;

	static float	fHaltSpreadChance;
	static bool		bDoCollisiveSpread;
	static float	fDupeChance;
	static float	fEvaporateChance;

	static short	iBlockSample0;
	static short	iBlockSample1;

	RESET_VALS;

	// Generate chances based on the biome and terrain
	switch ( pBoob->biome ) 
	{
	case TER_THE_EDGE:
		fHaltSpreadChance = 0.6f;
		bDoCollisiveSpread = true;
		fDupeChance = 0.5f;
		fEvaporateChance = 0.1f;
		break;
	default:
		fHaltSpreadChance = 0.7f;
		bDoCollisiveSpread = false;
		fDupeChance = 0.1f;
		fEvaporateChance = 0.01f;
		break;
	}
	switch ( pBoob->terrain )
	{
	case TER_SPIRES:
	case TER_ISLANDS:
	case TER_OCEAN:
		fHaltSpreadChance = 0.5f;
		bDoCollisiveSpread = false;
		fDupeChance = 0.2f;
		if ( position.z >= 0 ) {
			bDoCollisiveSpread = true;
			fDupeChance = 0.1f;
			fHaltSpreadChance = 0.8f;
			fEvaporateChance = 0.05f;
		}
		break;
	case TER_DESERT:
		fHaltSpreadChance = 0.8f;
		bDoCollisiveSpread = false;
		fDupeChance = 0.13f;
		fEvaporateChance = 0.05f;
		break;
	}
	
	// First do the evaporate chance
	if ( TraverseTree( &pCurBoob, curBlock16, curBlock8, curBlockD, TOP ) )
	{
		iBlockSample1 = pCurBoob->data[curBlock16].data[curBlock8].data[curBlockD].block;
		if ( iBlockSample1 == EB_NONE ) {
			if ( random_range(0.0f,1.0f) < fEvaporateChance ) {
				pBoob->data[index].data[subindex].data[i].block = EB_NONE;
				return;
			}
		}
	}

	RESET_VALS;

	// Next check downwards for drop
	if ( TraverseTree( &pCurBoob, curBlock16, curBlock8, curBlockD, BOTTOM ) )
	{
		iBlockSample1 = pCurBoob->data[curBlock16].data[curBlock8].data[curBlockD].block;
		if ( ( iBlockSample1 == EB_NONE ) || (( iBlockSample1 == EB_WATER || iBlockSample1 == EB_SAND )&&( bDoCollisiveSpread )) )
		{
			// Move the block down
			if ( !bDoCollisiveSpread || ( bDoCollisiveSpread && ( random_range(0.0f,1.0f) > fDupeChance )) )
				pBoob->data[index].data[subindex].data[i].block = EB_NONE;
			if ( iBlockSample1 != EB_SAND )
				pCurBoob->data[curBlock16].data[curBlock8].data[curBlockD].block = EB_WATER;
			// Update flag
			bWaterUpdated = true;
			// Chance to stop after dropping
			if ( random_range(0.0f,1.0f) < fHaltSpreadChance*0.7f )
				return;
		}
	}

	// Check all the sides, starting with a random direction
	char start = rand() % 4;
	char end = start+4;
	for ( char side = start; side < end; side++ )
	{
		RESET_VALS;

		// Check a random side
		EFaceDir dir;
		if ( side%4 == 0 )
			dir = FRONT;
		else if ( side%4 == 1 )
			dir = BACK;
		else if ( side%4 == 2 )
			dir = LEFT;
		else
			dir = RIGHT;

		// Traverse it
		if ( TraverseTree( &pCurBoob, curBlock16, curBlock8, curBlockD, dir ) )
		{
			if ( pCurBoob == NULL ) {
				cout << "Josh, you have a problem." << endl;
			}
			if ( pCurBoob->data[curBlock16].data[curBlock8].data[curBlockD].block == EB_NONE )
			{
				// Move the block to it
				if ( random_range(0.0f,1.0f) > fDupeChance )	// Clear out previous block with 1-dupe chance
					pBoob->data[index].data[subindex].data[i].block = EB_NONE;	// Clear out old block
				// Put water in the new area
				pCurBoob->data[curBlock16].data[curBlock8].data[curBlockD].block = EB_WATER;
				// Update flag
				bWaterUpdated = true;
				// Chance to stop spreading at this one
				if ( random_range(0.0f,1.0f) < fHaltSpreadChance )
					break;
			}
		}
	}

}

//
void CVoxelTerrain::UpdateOre_Default32 ( CBoob * pBoob )
{
	if ( pBoob )
	{
		//
		for ( char i = 0; i < 8; ++i ) 
		{
			for ( char j = 0; j < 8; ++j )
			{
				for ( short k = 0; k < 512; ++k )
				{
					if ( pBoob->data[i].data[j].data[k].block == EB_SYS_ORE_SPAWN ) {
						pBoob->data[i].data[j].data[k].block = EB_HEMATITE;
					}
				}
			}
		}
		//
	}
}

// Reset wire simulation
void CVoxelTerrain::UpdateWire_Reset32 ( CBoob * pBoob, RangeVector const& position )
{
	// Update current boob
	for ( char ii = 0; ii < 8; ii += 1 )
	{
		// Update current block vbo
		for ( char jj = 0; jj < 8; jj += 1 )
		{
			/*char i, j, k;
			for ( k = 0; k < 8; k += 1 )
			{
				for ( j = 0; j < 8; j += 1 )
				{
					for ( i = 0; i < 8; i += 1 )
					{
						SetPowerBit( pBoob->data[ii].data[jj].temp[i+j*8+k*64], 0 );
					}
				}
			}*/
			for ( short i = 0; i < 512; ++i )
			{
				/*uint32_t newBit = pBoob->data[ii].data[jj].temp[i];
				SetPowerBit( newBit, 0 );
				pBoob->data[ii].data[jj].temp[i] = (unsigned char)(newBit);*/
				//SetPowerBitChar( pBoob->data[ii].data[jj].temp[i], 0 );
				//SetPowerBit( pBoob->data[ii].data[jj].data[i], 0 );
				pBoob->data[ii].data[jj].data[i].power = 0;
			}
		}
	}
}

// Perform default wire simulation
void CVoxelTerrain::UpdateWire_Default32 ( CBoob * pBoob, RangeVector const& position )
{
	// Update current boob
	for ( char i = 0; i < 8; i += 1 )
	{
		if ( pBoob->solid[i] ) { //temporary optimization
			UpdateWire_Default16( pBoob, &(pBoob->data[i]), i, position );
		}
	}
}
void CVoxelTerrain::UpdateWire_Default16 ( CBoob * pBoob, subblock16 * block, char const index, RangeVector const& position )
{
	// Update current block vbo
	for ( char i = 0; i < 8; i += 1 )
	{
		UpdateWire_Default8( pBoob, &(block->data[i]), index,i, position );
	}
}
void CVoxelTerrain::UpdateWire_Default8 ( CBoob * pBoob, subblock8 * block, char const index, char const subindex, RangeVector const& position )
{
	// Draw current block
	char res = pBoob->current_resolution;
	char i, j, k;
	for ( k = 0; k < 8; k += res )
	{
		for ( j = 0; j < 8; j += res )
		{
			for ( i = 0; i < 8; i += res )
			{
				//if ( block->data[i+j*8+k*64] == EB_WATER )
				//if ( GetSource( block->temp[i+j*8+k*64] ) )
				if ( GetSource( block->data[i+j*8+k*64] ) )
				{
					//cout << "Source block." << endl;
					UpdateWire_Default1( pBoob, block, index, subindex, i+j*8+k*64, position );
				}
			}
		}
	}
}

void CVoxelTerrain::UpdateWire_Default1 ( CBoob * pBoob, subblock8 * block, char const index, char const subindex, short const i, RangeVector const& position )
{
	// initial vals
	CBoob *		pCurBoob	= pBoob;
	subblock8 * pCurBlock	= block;
	char		curBlock16	= index;
	char		curBlock8	= subindex;
	short		curBlockD	= i;

	// Reset values
	//RESET_VALS;

	// Create a struct to store info
	struct sCurrentWireSpot
	{
		CBoob *		pCurBoob;
		subblock8 * pCurBlock;
		char		curBlock16;
		char		curBlock8;
		short		curBlockD;
		char		prevDirection;

		bool operator== ( sCurrentWireSpot const& right )
		{
			//return (( pCurBlock==right.pCurBlock )&&( curBlock8==right.curBlock8 )&&( curBlockD==right.curBlockD ));
			return (( pCurBoob==right.pCurBoob )&&( curBlock16==right.curBlock16 )&&( curBlock8==right.curBlock8 )&&( curBlockD==right.curBlockD ));
		};
	};

	// Create a list of blocks to check
	list<sCurrentWireSpot> wireSpots;

	// Create a list of blocks that have been checked
	vector<sCurrentWireSpot> lookedList;

	// Push current block onto the list
	sCurrentWireSpot startPoint;
	startPoint.pCurBoob		= pCurBoob;
	startPoint.pCurBlock	= pCurBlock;
	startPoint.curBlock16	= curBlock16;
	startPoint.curBlock8	= curBlock8;
	startPoint.curBlockD	= curBlockD;
	startPoint.prevDirection = 0;

	wireSpots.push_back( startPoint );
	lookedList.push_back( startPoint );

	// Continue looking while list isn't empty
	while ( !wireSpots.empty() )
	{
		// Get current wire spot
		sCurrentWireSpot currentSpot = wireSpots.front();
		wireSpots.pop_front();

		// Set the current wire spot to 1 power
		/*uint32_t newBit = currentSpot.pCurBlock->temp[curBlockD];
		SetPowerBit( newBit, 1 );
		currentSpot.pCurBlock->temp[curBlockD] = (unsigned char)(newBit);*/
		//SetPowerBitChar( currentSpot.pCurBlock->temp[currentSpot.curBlockD], 1 );
		//SetPowerBitChar( currentSpot.pCurBoob->data[currentSpot.curBlock16].data[currentSpot.curBlock8].temp[currentSpot.curBlockD], 1 );
		//SetPowerBit( currentSpot.pCurBoob->data[currentSpot.curBlock16].data[currentSpot.curBlock8].data[currentSpot.curBlockD], 1 );
		currentSpot.pCurBoob->data[currentSpot.curBlock16].data[currentSpot.curBlock8].data[currentSpot.curBlockD].power = 1;
		
		// Look at each direction except parent direction
		for ( char dir = 1; dir <= 6; ++dir )
		{	
			if ( currentSpot.prevDirection != dir )
			{
				// Get ready to traverse
				pCurBoob	= currentSpot.pCurBoob;
				pCurBlock	= currentSpot.pCurBlock;
				curBlock16	= currentSpot.curBlock16;
				curBlock8	= currentSpot.curBlock8;
				curBlockD	= currentSpot.curBlockD;

				// Traverse tree
				if ( TraverseTree( &pCurBoob, curBlock16, curBlock8, curBlockD, (EFaceDir)(dir) ) )
				{
					// Set new current block, as it's not updated by the tree traversal.
					pCurBlock = pCurBoob->data[curBlock16].data;
					// If has a wire in direction
					//if (( pCurBlock[curBlock8].data[curBlockD] == EB_WIRE )||( GetWireBit( (uint32_t)(pCurBlock[curBlock8].temp[curBlockD]) ) ))
					//if (( GetBlockType( pCurBlock[curBlock8].data[curBlockD] ) == EB_WIRE )||( GetWireBit( pCurBlock[curBlock8].data[curBlockD] ) ))
					if (( pCurBlock[curBlock8].data[curBlockD].block == EB_WIRE )||( pCurBlock[curBlock8].data[curBlockD].wire != 0 ))
					{
						// Create new wire spot
						sCurrentWireSpot newSpot;
						newSpot.pCurBoob	= pCurBoob;
						newSpot.pCurBlock	= pCurBlock;
						newSpot.curBlock16	= curBlock16;
						newSpot.curBlock8	= curBlock8;
						newSpot.curBlockD	= curBlockD;
						switch ( dir )
						{
						case FRONT:
							newSpot.prevDirection = BACK; break;
						case BACK:
							newSpot.prevDirection = FRONT; break;
						case LEFT:
							newSpot.prevDirection = RIGHT; break;
						case RIGHT:
							newSpot.prevDirection = LEFT; break;
						case TOP:
							newSpot.prevDirection = BOTTOM; break;
						case BOTTOM:
							newSpot.prevDirection = TOP; break;
						}

						// If wire is not in the looked list
						if ( find( lookedList.begin(), lookedList.end(), newSpot ) == lookedList.end() )
						{
							// add wire to list + current direction
							wireSpots.push_back( newSpot );
							lookedList.push_back( newSpot );
						}
					}
				}
			}
		}
	}

	// End list
}