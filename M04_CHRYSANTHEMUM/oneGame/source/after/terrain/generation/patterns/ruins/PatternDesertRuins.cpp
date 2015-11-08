
#include "PatternDesertRuins.h"
#include "core/math/random/Random.h"

#include <vector>
using std::vector;

using namespace World;
using namespace Terrain;

void PatternDesertRuins::Generate ( void )
{
	struct Box_t
	{
		Vector3d_d position;
		Vector3d_d halfsize;
	};

	vector<Real_d> heights;
	vector<Box_t> boxes;

	//Vector3d_d centerOffset ( 420, 190, 0 );
	Vector3d_d centerOffset = Vector3d_d( areaPosition.x*64.0 + 32, areaPosition.y*64.0 + 32, areaElevation );
	Vector3d_d centerBias = Vector3d_d( areaPosition.x*64, areaPosition.y*64, areaPosition.z*64 );

	if ( heights.empty() )
	{
		// Build the heights
		for ( uint i = 0; i < 4; ++i ) {
			//Real_d height = 28 + 9 + i*8;
			Real_d height = 28 + 9 + i*11;
			height += Random.Range( -9.0f, 9.0f );
			heights.push_back( height );
		}

		// Create structure bases
		for ( uint i = 0; i < 23; ++i ) {
			Vector3d_d nextPosition, nextHalfsize;
			Real_d nextHeight;
			
			// Choose position
			nextPosition.x = Random.Range( -170,170 );
			nextPosition.y = Random.Range( -170,170 );
			nextPosition.z = Random.Range( 0,40 );
			nextPosition += centerOffset;

			// Choose size
			nextHalfsize.x = Random.Range( 2,30 );
			nextHalfsize.y = Random.Range( 2,30 );
			nextHalfsize.z = Random.Range( 2,30*Random.Range(0.5,1.0) );

			// Set height
			nextHeight = nextPosition.z + nextHalfsize.z;

			// 75% chance to snap the height to the nearest div
			if ( Random.Range(0,1) < .75f ) {
				// Look for the nearest height
				Real_d nearestHeight = heights[0];
				for ( uint i = 1; i < heights.size(); ++i )
				{
					if ( abs( nextHeight - heights[i] ) < abs( nextHeight - nearestHeight ) ) {
						nearestHeight = heights[i];
					}
				}
				nextHeight = nearestHeight;
			}

			// 50% chance to edit the height
			if ( Random.Range(0,1) < .50f ) {
				// Set up the block to go from the ground to the height
				Real_d groundLevel = 0;
				nextPosition.z = (nextHeight + groundLevel)/2;
				nextHalfsize.z = (nextHeight - groundLevel)/2;
			}
			else {
				// Set up the block to touch the top of the desired height
				nextPosition.z = nextHeight - nextHalfsize.z;
			}

			// Add the block
			{
				Box_t nextBlock;
				nextBlock.position = nextPosition;
				nextBlock.halfsize = nextHalfsize;
				boxes.push_back( nextBlock );
			}

			// 20% chance to add a column in the middle of it
			Real chance = Random.Range(0,1);
			if ( chance < 0.20f )
			{
				Box_t nextBlock;
				nextBlock.halfsize.x = Random.Range( 2,6 );
				nextBlock.halfsize.y = nextBlock.halfsize.x * Random.Range( 0.5,2.0 );
				nextBlock.halfsize.z *= Random.Range( 1.6,3.4 );
				boxes.push_back( nextBlock );
			}
			else if ( chance < 0.70f ) // 50% chance to put columns on every corner
			{
				// Put a column on each corner
				Box_t nextBlock;
				nextBlock.halfsize.x = Random.Range( 2,6 );
				nextBlock.halfsize.y = nextBlock.halfsize.x;
				nextBlock.halfsize.z *= Random.Range( 1.5,2.2 );

				nextBlock.position.x = nextPosition.x + nextHalfsize.x;
				nextBlock.position.y = nextPosition.y + nextHalfsize.y;
				boxes.push_back( nextBlock );
				nextBlock.position.x = nextPosition.x - nextHalfsize.x;
				nextBlock.position.y = nextPosition.y + nextHalfsize.y;
				boxes.push_back( nextBlock );
				nextBlock.position.x = nextPosition.x + nextHalfsize.x;
				nextBlock.position.y = nextPosition.y - nextHalfsize.y;
				boxes.push_back( nextBlock );
				nextBlock.position.x = nextPosition.x - nextHalfsize.x;
				nextBlock.position.y = nextPosition.y - nextHalfsize.y;
				boxes.push_back( nextBlock );
			}

			// 60% chance to add a child
			while ( Random.Range(0,1) < 0.60f )
			{
				// Choose position
				nextPosition.x += Random.Range( -nextHalfsize.x,nextHalfsize.x );
				nextPosition.y += Random.Range( -nextHalfsize.y,nextHalfsize.y );
				nextPosition.z += Random.Range( -nextHalfsize.z,nextHalfsize.z );

				// Choose size
				nextHalfsize.x *= Random.Range( 0.5,1.0 );
				nextHalfsize.y *= Random.Range( 0.5,1.0 );
				nextHalfsize.z *= Random.Range( 0.3,1.0 );

				// Set height
				nextHeight = nextPosition.z + nextHalfsize.z;

				// 75% chance to snap the height to the nearest div
				if ( Random.Range(0,1) < .75f ) {
					// Look for the nearest height
					Real_d nearestHeight = heights[0];
					for ( uint i = 1; i < heights.size(); ++i )
					{
						if ( abs( nextHeight - heights[i] ) < abs( nextHeight - nearestHeight ) ) {
							nearestHeight = heights[i];
						}
					}
					nextHeight = nearestHeight;
				}

				// 50% chance to edit the height
				if ( Random.Range(0,1) < .50f ) {
					// Set up the block to go from the ground to the height
					Real_d groundLevel = 0;
					nextPosition.z = (nextHeight + groundLevel)/2;
					nextHalfsize.z = (nextHeight - groundLevel)/2;
				}
				else {
					// Set up the block to touch the top of the desired height
					nextPosition.z = nextHeight - nextHalfsize.z;
				}

				// Add the block
				Box_t nextBlockB;
				nextBlockB.position = nextPosition;
				nextBlockB.halfsize = nextHalfsize;
				boxes.push_back( nextBlockB );

				// 10% chance to add a column in the middle of it
				Real chance = Random.Range(0,1);
				if ( chance < 0.10f )
				{
					Box_t nextBlock;
					nextBlock.halfsize.x = Random.Range( 2,6 );
					nextBlock.halfsize.y = nextBlock.halfsize.x * Random.Range( 0.5,2.0 );
					nextBlock.halfsize.z *= Random.Range( 1.6,3.4 );
					boxes.push_back( nextBlock );
				}
				else if ( chance < 0.50f ) // 40% chance to put columns on every corner
				{
					// Put a column on each corner
					Box_t nextBlock;
					nextBlock.halfsize.x = Random.Range( 2,6 );
					nextBlock.halfsize.y = nextBlock.halfsize.x;
					nextBlock.halfsize.z *= Random.Range( 1.5,2.2 );

					nextBlock.position.x = nextPosition.x + nextHalfsize.x;
					nextBlock.position.y = nextPosition.y + nextHalfsize.y;
					boxes.push_back( nextBlock );
					nextBlock.position.x = nextPosition.x - nextHalfsize.x;
					nextBlock.position.y = nextPosition.y + nextHalfsize.y;
					boxes.push_back( nextBlock );
					nextBlock.position.x = nextPosition.x + nextHalfsize.x;
					nextBlock.position.y = nextPosition.y - nextHalfsize.y;
					boxes.push_back( nextBlock );
					nextBlock.position.x = nextPosition.x - nextHalfsize.x;
					nextBlock.position.y = nextPosition.y - nextHalfsize.y;
					boxes.push_back( nextBlock );
				}
			} // End children
		}

		// Build structure bridges
		const uint boxCount = boxes.size();
		for ( uint i = 0; i < boxCount; ++i )
		{
			Real_d c_blockHeight;
			c_blockHeight = boxes[i].position.z + boxes[i].halfsize.z;
			// Look and see if this is nearby a step (search for nearest height)
			Real_d c_nearestHeight = heights[0];
			for ( uint hi = 1; hi < heights.size(); ++hi )
			{
				if ( abs( c_blockHeight - heights[hi] ) < abs( c_blockHeight - c_nearestHeight ) ) {
					c_nearestHeight = heights[hi];
				}
			}

			// Skip if not nearby
			if ( abs( c_blockHeight-c_nearestHeight ) > 2 )
			{
				continue;
			}

			// Look for nearby boxes
			for ( uint j = 0; j < boxCount; ++j )
			{
				// Skip self
				if ( i == j ) {
					continue;
				}
				// Check that it's close enough
				if ( (boxes[i].position - boxes[j].position).sqrMagnitude() > sqr(90) ) {
					continue;
				}
				// Check that it's connected at a right angle
				if ( abs(boxes[i].position.x-boxes[j].position.x) > boxes[i].halfsize.x && abs(boxes[i].position.x-boxes[j].position.x) > boxes[j].halfsize.x ) {
					continue;
				}
				if ( abs(boxes[i].position.y-boxes[j].position.y) > boxes[i].halfsize.y && abs(boxes[i].position.y-boxes[j].position.y) > boxes[j].halfsize.y ) {
					continue;
				}
				// Check its height
				Real_d t_blockHeight;
				t_blockHeight = boxes[j].position.z + boxes[j].halfsize.z;
				/*if ( abs( t_blockHeight - c_blockHeight ) > 3 ) {
					continue;
				}*/
				if ( c_blockHeight - t_blockHeight > 3 ) {
					continue;
				}

				// 75% chance to make a bridge
				if ( Random.Range(0,1) < 0.75f )
				{
					Vector3d_d nextPosition, nextHalfsize;

					nextHalfsize.x = abs(boxes[i].position.x-boxes[j].position.x)/2 * Random.Range(0.5,1) + 2;
					nextHalfsize.y = abs(boxes[i].position.y-boxes[j].position.y)/2 * Random.Range(0.5,1) + 2;
					nextHalfsize.z = Random.Range( 1,2.5 );

					nextPosition = (boxes[i].position + boxes[j].position)/2;
					//nextPosition.z = (t_blockHeight+c_blockHeight)/2 - nextHalfsize.z;
					nextPosition.z = c_blockHeight - nextHalfsize.z;

					// Add the block
					Box_t nextBlock;
					nextBlock.position = nextPosition;
					nextBlock.halfsize = nextHalfsize;
					boxes.push_back( nextBlock );
				}
				
			}
			// Done check
		} // Done bridges

		// Add some small spires
		for ( uint i = 0; i < 18; ++i )
		{
			Vector3d_d nextPosition, nextHalfsize;
			Real_d nextHeight;
			
			// Choose position
			nextPosition.x = Random.Range( -170,170 );
			nextPosition.y = Random.Range( -170,170 );
			nextPosition.z = Random.Range( 0,40 );
			nextPosition += centerOffset;

			// Choose size
			nextHalfsize.x = Random.Range( 2,5 );
			nextHalfsize.y = Random.Range( 2,5 );
			nextHalfsize.z = Random.Range( 30,80 ) * Random.Range( 0.5, 1.5 );

			Box_t nextBlock;
			nextBlock.position = nextPosition;
			nextBlock.halfsize = nextHalfsize;
			boxes.push_back( nextBlock );
		}
		
		// Everything is done, go to generation
	}


	// Put the boxes into the town info
	for ( uint i = 0; i < boxes.size(); ++i )
	{
		patternBuilding_t building;
		Vector3d_d pos = (boxes[i].position - boxes[i].halfsize) - centerBias;
		Vector3d_d size = boxes[i].halfsize*2;
		building.box.position = Vector3d(pos.x,pos.y,pos.z);
		building.box.size = Vector3d(size.x,size.y,size.z);
		building.type = eBuildingType::BLD_Stoneblock;
		genTown->buildings.push_back( building );
	}
}