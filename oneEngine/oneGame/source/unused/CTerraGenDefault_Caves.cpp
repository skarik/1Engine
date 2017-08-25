
// ======== CTerraGenDefault_Caves =======
// This is the definition for the first pass dungeon generation.
// Dungeons, caves, bridges, and excavations are handled in this area.

// NOTE: The functionality of this code has been supersceded by the Dungeon system.
// See CTerraGenDefault_Dungeons.cpp for implementation details.

#include "CTerraGenDefault.h"
/*
// =======================================
// == Generation Algorithm for Dungeons ==
// =======================================
void CTerraGenDefault::TerraGenCavePass ( CBoob * pBoob, RangeVector const& position )
{
	for ( char i = -2; i <= 2; i++ )
		for ( char j = -2; j <= 2; j++ )
			for ( char k = -2; k <= 2; k++ )
				TerraGen_Caves_CreateSystems( RangeVector( position.x+i,position.y+j,position.z+k ) );
	TerraGen_Caves_CreateCaves( pBoob, position );
}

void CTerraGenDefault::TerraGen_Caves_CreateSystems ( RangeVector const& position )
{

	// First check if a system already exists in the generation list for this section
	for ( vector<TerraCave*>::iterator it = vCaveSystems.begin(); it != vCaveSystems.end(); it++ )
	{
		if ( (*it)->startPosition == position )
		{
			// If it does exist, then stop now. Don't generate a thang.
			return;
		}
	}
	// Now, begin
	{
		// Need the elevation line
		static ftype elevationLine;
		elevationLine = TerraGen_1p_GetElevation( position );

		// Grab the terrain type
		char terType = TerraGen_priv_GetType( Vector3d( (ftype)position.x, (ftype)position.y, (ftype)position.z ) );

		// Grab density of the boob
		static ftype density;
		density = 0.5f;
		density *= 0.56f;

		// Grab random value based on position
		static ftype fMainTer;
		fMainTer = noise->Get3D( position.x*3.9f,position.y*3.9f,position.z*4.1f )+0.5f;

		// Select Cavetypes
		ECaveType new_cavetype = CAV_NULL;
		// Based on the position + biome, choose cave types
		ftype temp_zdif = position.z-elevationLine;
		if (( temp_zdif > 1.3f )&&( temp_zdif <= 1.5f ))
		{
			new_cavetype = BRIDGE_DIRT;
		}
		else if (( temp_zdif > 0.2f )&&( temp_zdif <= 1.3f ))
		{
			// If the boob is at the surface, create a cave system with a smaller random chance
			if ( fMainTer < density*0.46f )
				new_cavetype = CAV_CAVE;
			else if ( fMainTer > (1.0f-density*0.84f) )
				new_cavetype = BRIDGE_DIRT;
		}
		else if (( temp_zdif > 0 )&&( temp_zdif <= 0.2f ))
		{
			// Near surface
			new_cavetype = CAV_CAVE;
		}
		else if (( temp_zdif > -15.0f )&&( temp_zdif <= 0 ))
		{
			// If the boob is underground, create a cave system with a random chance
			new_cavetype = CAV_CAVE;
		}

		// Based on current cavetype, check against density
		switch ( new_cavetype )
		{
		case CAV_CAVE:
			if (( temp_zdif > 0 )&&( temp_zdif <= 1.3f ))
			{
				if ( fMainTer > density*0.46f ) // If the boob is at the surface, create a cave system with a smaller random chance
					new_cavetype = CAV_NULL;
			}
			else if (( temp_zdif > -15.0f )&&( temp_zdif <= 0 ))
			{
				if ( fMainTer > density )
					new_cavetype = CAV_NULL;
			}
			break;
		case BRIDGE_STONE:
		case BRIDGE_DIRT:
			density *= 0.678f; // Lower density (bridges shouldn't be too common, as they marr the terrain)
			// Change density based on terrain.
			switch ( terType )
			{
				case TER_OUTLANDS:
					density += 0.3f;
					break;
				case TER_MOUNTAINS: case TER_SPIRES:
				case TER_THE_EDGE:
					density += 0.2f;
					break;
				case TER_HILLLANDS:
					density -= 0.26f;
					break;
				case TER_OCEAN: case TER_FLATLANDS:
				case TER_ISLANDS: case TER_DESERT:
				case TER_BADLANDS:
					density -= 0.4f;
					break;
				default:
					// Nothing
					break;
			};
			if (( temp_zdif > 1.3f )&&( temp_zdif <= 1.5f ))
			{
				if ( fMainTer < (1.0f-density*0.34f) )
					new_cavetype = CAV_NULL;
			}
			else if (( temp_zdif > 0.2f )&&( temp_zdif <= 1.3f ))
			{
				if ( fMainTer < (1.0f-density*0.84f) )
					new_cavetype = CAV_NULL;
			}
			break;
		}

		// Create a system based on the new type
		if ( new_cavetype != CAV_NULL )
		{
			TerraGen_Caves_StartSystem( position, new_cavetype );
		}
	}
}


// Start a system if it doesn't exist, and generate it all the way
void CTerraGenDefault::TerraGen_Caves_StartSystem ( RangeVector const& position, ECaveType const& cavetype )
{
	TerraCave* newCave = new TerraCave;
	newCave->startPosition = position;
	newCave->type = cavetype;			// Parent needs to set cavetype too. Beginning tunnels may have
										// different cavetypes than later tunnels in a single system.

	// Create initial segment
	TerraTunnel newSegment;
	switch ( cavetype )
	{
	case CAV_CAVE:
		// Start with an initial cave segment in a random position
		newSegment.ray.pos = Vector3d(
			noise_hf->Get3D( position.x*0.6f, position.y*0.7f-0.3f, position.z*0.8f )+0.5f+ftype(position.x),
			noise_hf->Get3D( position.y*0.6f, position.z*0.7f-0.3f, position.x*0.8f )+0.5f+ftype(position.y),
			noise_hf->Get3D( position.z*0.6f, position.x*0.7f-0.3f, position.y*0.8f )+0.5f+ftype(position.z)
		);
		// With a random height from radius 1.5 to 2.9
		newSegment.height = 2.2f + 1.4f*noise_hf->Get3D( position.x*1.3f+0.4f,position.z*1.6f,position.y*1.4f );
		// With a random width from radius 1.0 to 3.0
		newSegment.width = 2.0f + 2.0f*noise_hf->Get3D( position.x*1.7f-0.4f,position.z*1.2f-0.2f,position.y*1.6f );
		// With a random roughness from 0.0 to 0.2
		newSegment.noise = 0.1f + 0.2f*noise_hf->Get( position.x+position.y*0.2f,position.z+0.4f );
		// And a random direction from +15 degrees to -90 degrees
		newSegment.ray.dir = Vector3d(
			noise_hf->Get3D( position.y+position.z*1.3f-0.3f, position.x*1.2f+position.y+0.5f, position.x*2.3f ),
			noise_hf->Get3D( position.x+position.y*1.5f-0.2f, position.y*1.7f+position.z+0.3f, position.y*1.4f ),
			noise_hf->Get( position.y+position.z*1.3f-0.3f, position.x*1.4f+position.z+0.5f )*1.25f-0.375f
		);
		// And extend that from 10 to 50 percent of the boob width
		newSegment.ray.dir = newSegment.ray.dir.normal() * ( 0.3f + 0.4f*noise_hf->Get( position.z*0.6f, position.y*2.1f+position.x*0.7f-0.3f ) );
		break;
	case BRIDGE_DIRT:
	case BRIDGE_STONE:
		// Start with an initial cave segment in a random position
		newSegment.ray.pos = Vector3d(
			noise_hhf->Get3D( position.x*0.6f, position.y*0.7f-0.3f, position.z*0.8f )+0.5f+ftype(position.x),
			noise_hhf->Get3D( position.y*0.6f, position.z*0.7f-0.3f, position.x*0.8f )+0.5f+ftype(position.y),
			noise_hhf->Get3D( position.z*0.6f, position.x*0.7f-0.3f, position.y*0.8f )+0.5f+ftype(position.z)
		);
		// With a random height from radius 1.5 to 2.9
		newSegment.height = 2.2f + 1.4f*noise_hf->Get3D( position.x*1.3f+0.4f,position.z*1.6f,position.y*1.4f );
		// With a random width from radius 1.0 to 3.0
		newSegment.width = 2.0f + 2.0f*noise_hf->Get3D( position.x*1.7f-0.4f,position.z*1.2f-0.2f,position.y*1.6f );
		// With a random roughness from 0.0 to 0.2
		newSegment.noise = 0.1f + 0.2f*noise_hf->Get( position.x+position.y*0.2f,position.z+0.4f );
		// And a random direction from +15 degrees to -15 degrees
		newSegment.ray.dir = Vector3d(
			noise_hf->Get3D( position.y+position.z*1.3f-0.3f, position.x*1.2f+position.y+0.5f, position.x*2.3f )*2.0f,
			noise_hf->Get3D( position.x+position.y*1.5f-0.2f, position.y*1.7f+position.z+0.3f, position.y*1.4f )*2.0f,
			noise_hf->Get( position.y+position.z*1.3f-0.3f, position.x*1.4f+position.z+0.5f )*0.4f
		);
		// And extend that from 10 to 50 percent of the boob width
		newSegment.ray.dir = newSegment.ray.dir.normal() * ( 0.3f + 0.4f*noise_hf->Get( position.z*0.6f, position.y*2.1f+position.x*0.7f-0.3f ) );
		break;
	}
	// (Set segment type)
	newSegment.type = cavetype;

	// Add this current area to the affected stack
	newCave->affectedAreas.push_back( position );
	// Add this new segment to the stack
	newCave->partTree.push_back( newSegment );

	// Set the cave center
	vCurrentCaveCenter = Vector3d( ftype(position.x)+0.5f, ftype(position.y)+0.5f, ftype(position.z)+0.5f );

	// Recursively create the cave
	switch ( cavetype )
	{
	case CAV_CAVE:
		TerraGen_Caves_Generate( newCave, 0 );
		break;
	case BRIDGE_DIRT:
	case BRIDGE_STONE:
		TerraGen_Bridges_Generate( newCave, 0 );
		break;
	}

	// Add all effected areas to the affected stack
	RangeVector currentPos;

	for ( unsigned int i = 0; i < newCave->partTree.size(); i++ )
	{
		newCave->affectionMap.push_back( new vector<RangeVector> () );

		currentPos = BoobSpaceToIndex( newCave->partTree[i].ray.pos );
		newCave->affectionMap[i]->push_back( currentPos );
		currentPos = BoobSpaceToIndex( newCave->partTree[i].ray.pos + newCave->partTree[i].ray.dir );
		newCave->affectionMap[i]->push_back( currentPos );

		// Search through all the affected areas
		vector<RangeVector>::iterator pos_it = find( newCave->affectedAreas.begin(), newCave->affectedAreas.end(), currentPos );
		// Add if not found
		if ( pos_it == newCave->affectedAreas.end() )
			newCave->affectedAreas.push_back( currentPos );
	}
	

	// Add our now-completed cave to the pile
	vCaveSystems.push_back( newCave );
}

void CTerraGenDefault::TerraGen_Caves_CreateCaves ( CBoob * pBoob, RangeVector const& position )
{
	// Loop through all the caves.
	for ( vector<TerraCave*>::iterator it = vCaveSystems.begin(); it != vCaveSystems.end(); it++ )
	{
		// Look for this current boob index
		vector<RangeVector>::iterator searchResult = find( (*it)->affectedAreas.begin(), (*it)->affectedAreas.end(), position );

		// If it can be affected
		if ( searchResult != (*it)->affectedAreas.end() )
		{
			// Then for each tunnel
			for ( unsigned int i = 0; i < (*it)->partTree.size(); i++ )
			{
				// If this current boob is in the tunnel edited
				vector<RangeVector>::iterator searchResult2 = find( (*it)->affectionMap[i]->begin(), (*it)->affectionMap[i]->end(), position );

				if ( searchResult2 != (*it)->affectionMap[i]->end() )
				{
					TerraTunnel const& tunnelRef = (*it)->partTree[i];

					for ( char i = 0; i < 8; i += 1 )
					{
						for ( char j = 0; j < 8; j += 1 )
						{
							for ( int k = 0; k < 512; k += 1 )
							{
								// Declared static to save init time
								static Vector3d blockPosition;
								static ftype fMainTer;
								static ftype fSubTer;

								switch ( tunnelRef.type )
								{
								case CAV_CAVE:
									if (( pBoob->data[i].data[j].data[k].block != EB_NONE )&&( pBoob->data[i].data[j].data[k].block != EB_WATER ))
									{
										blockPosition.x = position.x + (((i%2) * 16) + ((j%2) * 8) + (k%8))/32.0f;
										blockPosition.y = position.y + ((((i/2)%2) * 16) + (((j/2)%2) * 8) + ((k/8)%8))/32.0f;
										blockPosition.z = position.z + (((i/4) * 16) + ((j/4) * 8)  + (k/64))/32.0f;

										Vector3d a_to_p = blockPosition-(tunnelRef.ray.pos);
										Vector3d a_to_b = tunnelRef.ray.dir;

										ftype atb2 = a_to_b.sqrMagnitude();
										ftype atp_dot_atb = a_to_p.dot( a_to_b );

										ftype t = min( 1.0f, max( 0.0f, atp_dot_atb / atb2  ) );

										Vector3d pos = tunnelRef.ray.pos + a_to_b*t;

										// If the distance is close enough, then bam, excavate
										if ( (pos-blockPosition).magnitude() <= (tunnelRef.height/32.0f) )
										{
											pBoob->data[i].data[j].data[k].block = EB_NONE;
										}
									}
									break;
								case BRIDGE_DIRT:
								case BRIDGE_STONE:
									if (( pBoob->data[i].data[j].data[k].block == EB_NONE )||( pBoob->data[i].data[j].data[k].block == EB_WATER ))
									{
										blockPosition.x = position.x + (((i%2) * 16) + ((j%2) * 8) + (k%8))/32.0f;
										blockPosition.y = position.y + ((((i/2)%2) * 16) + (((j/2)%2) * 8) + ((k/8)%8))/32.0f;
										blockPosition.z = position.z + (((i/4) * 16) + ((j/4) * 8)  + (k/64))/32.0f;

										Vector3d a_to_p = blockPosition-(tunnelRef.ray.pos);
										Vector3d a_to_b = tunnelRef.ray.dir;

										ftype atb2 = a_to_b.sqrMagnitude();
										ftype atp_dot_atb = a_to_p.dot( a_to_b );

										ftype t = min( 1.0f, max( 0.0f, atp_dot_atb / atb2  ) );

										Vector3d pos = tunnelRef.ray.pos + a_to_b*t;

										// If the distance is close enough, then bam, excavate
										if ( (pos-blockPosition).magnitude() <= (tunnelRef.height/32.0f) )
										{
											if ( tunnelRef.type == BRIDGE_DIRT )
												pBoob->data[i].data[j].data[k].block = EB_DIRT;
											else
												pBoob->data[i].data[j].data[k].block = EB_STONE;
										}
									}
									break;
								}
							}
						}
					}
					// End block for loop
				}
			}
			// End tunnel for loop
		}
	}
	// End cavern for loop
}


// ====================================
// ==     Regular Cave Generation    ==
// ====================================
// turn/end if 2 from the center point. Center point = cavecenter+Vector3d(0.5,0.5,0.5)
void CTerraGenDefault::TerraGen_Caves_Generate( TerraCave* pCave, const unsigned int iTunnel, const unsigned int seed )
{
	ftype fSeed = seed*2.3f+iTunnel*2.4f;

	// Get the previous tunnel
	TerraTunnel prevSegment = pCave->partTree[iTunnel];
	
	// Make a new tunnel
	TerraTunnel newSegment;
	newSegment.ray.pos = prevSegment.ray.pos + prevSegment.ray.dir;
	newSegment.type = pCave->type;	// Set tunnel type

	// Get the elevation
	static ftype elevationLine;
	elevationLine = TerraGen_1p_GetElevation( pCave->startPosition );

	// If the previous index is even, then we want to go horizontal or slightly down
	if ( iTunnel % 2 == 0 )
	{
		// Choose a random direction that's related to the current direction
		newSegment.ray.dir.x = 2.0f * noise->Get( pCave->startPosition.z*1.2f+newSegment.ray.pos.x-0.4f,  pCave->startPosition.y * 1.2f + prevSegment.ray.pos.y - iTunnel*1.8f );
		newSegment.ray.dir.y = 2.0f * noise->Get( pCave->startPosition.z*1.3f+newSegment.ray.pos.y-0.5f,  pCave->startPosition.x * 1.3f + prevSegment.ray.pos.x - iTunnel*1.7f );

		newSegment.ray.dir += prevSegment.ray.dir.normal()*0.4f;
		newSegment.ray.dir.z *= 0.03f;

		// So randomly go horizontal or down
		if ( noise->Get( iTunnel*0.7f, pCave->startPosition.x * 1.2f ) > 0.0f )
			newSegment.ray.dir.z = -0.1f + 0.2f+noise->Get( pCave->startPosition.x*1.2f+iTunnel*0.8f-0.4f,  pCave->startPosition.z * 1.2f + prevSegment.ray.pos.y );
		// But go downwards if we're on the surface
		if (( iTunnel <= 12 )&&( newSegment.ray.pos.z > elevationLine-0.5f ))
			newSegment.ray.dir.z -= 0.4f;

		// Normalize it and extend it from 10 to 30 percent of the boob width
		newSegment.ray.dir = newSegment.ray.dir.normal() * ( 0.2f + 0.2f*noise_hf->Get( newSegment.ray.pos.z*0.7f+0.2f, newSegment.ray.pos.y*2.2f+newSegment.ray.pos.x*0.6f-0.7f ) );

		// Now make sure the ending position is in the boob area
		Vector3d endPos = newSegment.ray.pos + newSegment.ray.dir;
		if (( fabs( endPos.x-vCurrentCaveCenter.x ) < 1.7f )&&( fabs( endPos.y-vCurrentCaveCenter.y ) < 1.7f )&&( fabs( endPos.z-vCurrentCaveCenter.z ) < 1.7f ))
		{
			// If it's in the area, then we can continue generation and add this tunnel.

			// With a random height from the previous segment, limited at 1.5 and 5.0
			newSegment.height = prevSegment.height + 1.4f*noise_hf->Get3D( endPos.x*1.3f+0.4f,endPos.z*1.6f,endPos.y*1.4f );
			newSegment.height = max( 1.5f, min( 5.0f, newSegment.height ) );
			// With a random width from the previous segment, limited at 1.0 and 6.0
			newSegment.width = prevSegment.width + 2.0f*noise_hf->Get3D( endPos.x*1.7f-0.4f,endPos.z*1.2f-0.2f,endPos.y*1.6f );
			newSegment.width = max( 1.0f, min( 6.0f, newSegment.width ) );
			// With a random roughness from the previous segment, limited at 0.0 and 0.5
			newSegment.noise = prevSegment.noise + 0.2f*noise_hf->Get( endPos.x+endPos.y*0.2f,endPos.z+0.4f );
			newSegment.noise = max( 0.0f, min( 0.5f, newSegment.noise ) );

			// Add the tunnel to the list and continue generating
			pCave->partTree.push_back( newSegment );
			TerraGen_Caves_Generate( pCave, pCave->partTree.size()-1, seed );
		}
	}
	// If the previous index is odd, then we want to go horizontal, directly downwards, directly upwards, or split
	else
	{
		// Choose a type of generation
		ftype randomVal = noise_hf->Get3D( fSeed*2.1f-newSegment.ray.pos.x*0.8f+0.2f,newSegment.ray.pos.z*0.8f,newSegment.ray.pos.y*0.8f-fSeed*0.4f );

		// Do a horizontal tunnel
		if (( randomVal > -0.15f )||(( randomVal < -0.2 )&&( pCave->partTree.size() > 20 )))
		{
			// Choose a random direction that's related to the current direction
			newSegment.ray.dir.x = 2.0f * noise->Get( pCave->startPosition.z*1.2f+newSegment.ray.pos.x-0.4f,  pCave->startPosition.y * 1.2f + prevSegment.ray.pos.y - fSeed*1.8f );
			newSegment.ray.dir.y = 2.0f * noise->Get( pCave->startPosition.z*1.3f+newSegment.ray.pos.y-0.5f,  pCave->startPosition.x * 1.3f + prevSegment.ray.pos.x - fSeed*1.7f );

			newSegment.ray.dir += prevSegment.ray.dir.normal();
			newSegment.ray.dir.z *= 0.03f;

			// So randomly go horizontal or slightly down
			if ( noise->Get( iTunnel*0.7f, pCave->startPosition.x * 1.2f ) > 0.0f )
				newSegment.ray.dir.z = -0.1f + 0.2f+noise->Get( pCave->startPosition.x*1.2f+iTunnel*0.8f-0.4f,  pCave->startPosition.z * 1.2f + prevSegment.ray.pos.y );
			// But go downwards more if we're on the surface
			if (( iTunnel <= 12 )&&( newSegment.ray.pos.z > elevationLine-0.3f ))
				newSegment.ray.dir.z -= 0.35f;

			// Normalize it and extend it from 10 to 30 percent of the boob width
			newSegment.ray.dir = newSegment.ray.dir.normal() * ( 0.2f + 0.2f*noise_hf->Get( newSegment.ray.pos.z*0.7f+0.2f, newSegment.ray.pos.y*2.2f+newSegment.ray.pos.x*0.6f-0.7f ) );
		}
		// Do a tunnel split
		else if ( randomVal < -0.2f )
		{
			// Choose a random direction that's related to the current direction
			newSegment.ray.dir.x = 2.0f * noise->Get( pCave->startPosition.z*1.2f+newSegment.ray.pos.x-0.4f,  pCave->startPosition.y * 1.2f + prevSegment.ray.pos.y - fSeed*1.8f );
			newSegment.ray.dir.y = 2.0f * noise->Get( pCave->startPosition.z*1.3f+newSegment.ray.pos.y-0.5f,  pCave->startPosition.x * 1.3f + prevSegment.ray.pos.x - fSeed*1.7f );

			newSegment.ray.dir += prevSegment.ray.dir.normal()*0.2f;
			newSegment.ray.dir.z *= 0.03f;

			// So randomly go horizontal or down
			if ( noise->Get( iTunnel*0.7f, pCave->startPosition.x * 1.2f ) > 0.0f )
				newSegment.ray.dir.z = -0.1f + 0.2f+noise->Get( pCave->startPosition.x*1.2f+iTunnel*0.8f-0.4f,  pCave->startPosition.z * 1.2f + prevSegment.ray.pos.y );

			// Normalize it and extend it from 10 to 30 percent of the boob width
			newSegment.ray.dir = newSegment.ray.dir.normal() * ( 0.2f + 0.2f*noise_hf->Get( newSegment.ray.pos.z*0.7f+0.2f, newSegment.ray.pos.y*2.2f+newSegment.ray.pos.x*0.6f-0.7f ) );

			// Also do another expansion
			TerraGen_Caves_Generate( pCave, iTunnel, seed+1 );
		}
		// Do a vertical up or down spike
		else
		{
			// Choose a random direction that's related to the current direction
			newSegment.ray.dir.x = 2.0f * noise->Get( pCave->startPosition.z*1.2f+newSegment.ray.pos.x-0.4f,  pCave->startPosition.y * 1.2f + prevSegment.ray.pos.y - fSeed*1.8f );
			newSegment.ray.dir.y = 2.0f * noise->Get( pCave->startPosition.z*1.3f+newSegment.ray.pos.y-0.5f,  pCave->startPosition.x * 1.3f + prevSegment.ray.pos.x - fSeed*1.7f );

			newSegment.ray.dir += prevSegment.ray.dir.normal();
			newSegment.ray.dir.z *= 0.03f;

			// So randomly go down -10 to -40
			newSegment.ray.dir.z = -25.0f + 30.0f*noise_hf->Get( pCave->startPosition.x*1.3f+iTunnel*0.9f+0.4f,  pCave->startPosition.z * 1.3f + prevSegment.ray.pos.y );

			// Randomly flip it
			//if ( noise->Get( iTunnel*0.7f, pCave->startPosition.x * 1.2f ) > 0.3f )
			//	newSegment.ray.dir.z *= -1;

			// Normalize it and extend it from 30 to 120 percent of the boob width
			// 30 to 120 = 0.75 + 0.9
			// 30 to 90 = 0.6 + 0.6
			newSegment.ray.dir = newSegment.ray.dir.normal() * ( 0.6f + 0.6f*noise_hf->Get( newSegment.ray.pos.z*0.7f+0.2f+fSeed*2.1f, newSegment.ray.pos.y*2.2f+newSegment.ray.pos.x*0.6f-0.7f-fSeed*2.1f ) );
		}

		// Now make sure the ending position is in the boob area
		Vector3d endPos = newSegment.ray.pos + newSegment.ray.dir;
		if (( fabs( endPos.x-vCurrentCaveCenter.x ) < 1.7f )&&( fabs( endPos.y-vCurrentCaveCenter.y ) < 1.7f )&&( fabs( endPos.z-vCurrentCaveCenter.z ) < 1.7f ))
		{
			// If it's in the area, then we can continue generation and add this tunnel.

			// With a random height from the previous segment, limited at 1.5 and 5.0
			newSegment.height = prevSegment.height + 1.4f*noise_hf->Get3D( endPos.x*1.3f+0.4f,endPos.z*1.6f,endPos.y*1.4f );
			newSegment.height = max( 1.5f, min( 5.0f, newSegment.height ) );
			// With a random width from the previous segment, limited at 1.0 and 6.0
			newSegment.width = prevSegment.width + 2.0f*noise_hf->Get3D( endPos.x*1.7f-0.4f,endPos.z*1.2f-0.2f,endPos.y*1.6f );
			newSegment.width = max( 1.0f, min( 6.0f, newSegment.width ) );
			// With a random roughness from the previous segment, limited at 0.0 and 0.5
			newSegment.noise = prevSegment.noise + 0.2f*noise_hf->Get( endPos.x+endPos.y*0.2f,endPos.z+0.4f );
			newSegment.noise = max( 0.0f, min( 0.5f, newSegment.noise ) );

			// Add the tunnel to the list and continue generating
			pCave->partTree.push_back( newSegment );
			TerraGen_Caves_Generate( pCave, pCave->partTree.size()-1, seed );
		}
	}
}

// ====================================
// ==    Regular Bridge Generation   ==
// ====================================
// turn/end if 2 from the center point. Center point = cavecenter+Vector3d(0.5,0.5,0.5)
void CTerraGenDefault::TerraGen_Bridges_Generate( TerraCave* pCave, const unsigned int iTunnel, const unsigned int seed )
{
	ftype fSeed = seed*2.3f+iTunnel*2.4f;

	// Get the previous tunnel
	TerraTunnel prevSegment = pCave->partTree[iTunnel];
	
	// Make a new tunnel
	TerraTunnel newSegment;
	newSegment.ray.pos = prevSegment.ray.pos + prevSegment.ray.dir;
	newSegment.type = pCave->type;	// Set tunnel type

	// Get the elevation
	static ftype elevationLine;
	elevationLine = TerraGen_1p_GetElevation( pCave->startPosition );

	// If the previous index is even, then we want to go horizontal or slightly down
	if ( iTunnel % 2 == 0 )
	{
		// Choose a random direction that's related to the current direction
		newSegment.ray.dir.x = 2.0f * noise->Get( pCave->startPosition.z*1.2f+newSegment.ray.pos.x-0.4f,  pCave->startPosition.y * 1.2f + prevSegment.ray.pos.y - iTunnel*1.8f );
		newSegment.ray.dir.y = 2.0f * noise->Get( pCave->startPosition.z*1.3f+newSegment.ray.pos.y-0.5f,  pCave->startPosition.x * 1.3f + prevSegment.ray.pos.x - iTunnel*1.7f );

		// So randomly go horizontal or down
		if ( noise->Get( iTunnel*0.7f, pCave->startPosition.x * 1.2f ) > 0.0f )
			newSegment.ray.dir.z = -0.1f + 0.2f*noise->Get( pCave->startPosition.x*1.2f+iTunnel*0.8f-0.4f,  pCave->startPosition.z * 1.2f + prevSegment.ray.pos.y );
		// Lower z val
		newSegment.ray.dir.z *= 0.05f;

		newSegment.ray.dir += prevSegment.ray.dir.normal() * 4.0f;
		
		// Normalize it and extend it from 10 to 30 percent of the boob width
		newSegment.ray.dir = newSegment.ray.dir.normal() * ( 0.2f + 0.2f*noise_hf->Get( newSegment.ray.pos.z*0.7f+0.2f, newSegment.ray.pos.y*2.2f+newSegment.ray.pos.x*0.6f-0.7f ) );

		// Now make sure the ending position is in the boob area
		Vector3d endPos = newSegment.ray.pos + newSegment.ray.dir;
		if (( fabs( endPos.x-vCurrentCaveCenter.x ) < 1.4f )&&( fabs( endPos.y-vCurrentCaveCenter.y ) < 1.4f )&&( fabs( endPos.z-vCurrentCaveCenter.z ) < 1.7f ))
		{
			// If it's in the area, then we can continue generation and add this tunnel.

			// With a random height from the previous segment, limited at 1.5 and 5.0
			newSegment.height = prevSegment.height + 1.4f*noise_hf->Get3D( endPos.x*1.3f+0.4f,endPos.z*1.6f,endPos.y*1.4f );
			newSegment.height = max( 1.5f, min( 5.0f, newSegment.height ) );
			// With a random width from the previous segment, limited at 1.0 and 6.0
			newSegment.width = prevSegment.width + 2.0f*noise_hf->Get3D( endPos.x*1.7f-0.4f,endPos.z*1.2f-0.2f,endPos.y*1.6f );
			newSegment.width = max( 1.0f, min( 6.0f, newSegment.width ) );
			// With a random roughness from the previous segment, limited at 0.0 and 0.5
			newSegment.noise = prevSegment.noise + 0.2f*noise_hf->Get( endPos.x+endPos.y*0.2f,endPos.z+0.4f );
			newSegment.noise = max( 0.0f, min( 0.5f, newSegment.noise ) );

			// Add the tunnel to the list and continue generating
			pCave->partTree.push_back( newSegment );
			TerraGen_Bridges_Generate( pCave, pCave->partTree.size()-1, seed );
		}
	}
	// If the previous index is odd, then we want to go horizontal, directly downwards, directly upwards, or split
	else
	{
		bool isDownspike = false;

		// Choose a type of generation
		ftype randomVal = noise_hf->Get3D( fSeed*2.1f-newSegment.ray.pos.x*0.8f+0.2f,newSegment.ray.pos.z*0.8f,newSegment.ray.pos.y*0.8f-fSeed*0.4f );

		// Do a horizontal tunnel
		if (( randomVal > 0.0f )||(( randomVal < -0.2 )&&( pCave->partTree.size() > 4 )))
		{
			// Choose a random direction that's related to the current direction
			newSegment.ray.dir.x = 2.0f * noise->Get( pCave->startPosition.z*1.2f+newSegment.ray.pos.x-0.4f,  pCave->startPosition.y * 1.2f + prevSegment.ray.pos.y - fSeed*1.8f );
			newSegment.ray.dir.y = 2.0f * noise->Get( pCave->startPosition.z*1.3f+newSegment.ray.pos.y-0.5f,  pCave->startPosition.x * 1.3f + prevSegment.ray.pos.x - fSeed*1.7f );

			// So randomly go horizontal or slightly down
			if ( noise->Get( iTunnel*0.7f, pCave->startPosition.x * 1.2f ) > 0.0f )
				newSegment.ray.dir.z = -0.1f + 0.2f*noise->Get( pCave->startPosition.x*1.2f+iTunnel*0.8f-0.4f,  pCave->startPosition.z * 1.2f + prevSegment.ray.pos.y );
			newSegment.ray.dir.z *= 0.04f;

			newSegment.ray.dir += prevSegment.ray.dir.normal() * 1.4f;

			// Normalize it and extend it from 10 to 30 percent of the boob width
			newSegment.ray.dir = newSegment.ray.dir.normal() * ( 0.2f + 0.2f*noise_hf->Get( newSegment.ray.pos.z*0.7f+0.2f, newSegment.ray.pos.y*2.2f+newSegment.ray.pos.x*0.6f-0.7f ) );
		}
		// Do a tunnel split
		else if ( randomVal < -0.3f )
		{
			// Choose a random direction that's related to the current direction
			newSegment.ray.dir.x = 2.0f * noise->Get( pCave->startPosition.z*1.2f+newSegment.ray.pos.x-0.4f,  pCave->startPosition.y * 1.2f + prevSegment.ray.pos.y - fSeed*1.8f );
			newSegment.ray.dir.y = 2.0f * noise->Get( pCave->startPosition.z*1.3f+newSegment.ray.pos.y-0.5f,  pCave->startPosition.x * 1.3f + prevSegment.ray.pos.x - fSeed*1.7f );

			// So randomly go horizontal or down
			if ( noise->Get( iTunnel*0.7f, pCave->startPosition.x * 1.2f ) > 0.0f )
				newSegment.ray.dir.z = -0.1f + 0.2f+noise->Get( pCave->startPosition.x*1.2f+iTunnel*0.8f-0.4f,  pCave->startPosition.z * 1.2f + prevSegment.ray.pos.y );
			newSegment.ray.dir.z *= 0.07f;

			newSegment.ray.dir += prevSegment.ray.dir.normal()*0.8f;

			// Normalize it and extend it from 10 to 30 percent of the boob width
			newSegment.ray.dir = newSegment.ray.dir.normal() * ( 0.2f + 0.2f*noise_hf->Get( newSegment.ray.pos.z*0.7f+0.2f, newSegment.ray.pos.y*2.2f+newSegment.ray.pos.x*0.6f-0.7f ) );

			// Also do another expansion
			TerraGen_Bridges_Generate( pCave, iTunnel, seed+1 );
		}
		// Do a vertical up or down spike
		else
		{
			// Set that this IS a downspike
			isDownspike = true;

			// Choose a random direction that's related to the current direction
			newSegment.ray.dir.x = 8.0f * noise->Get( pCave->startPosition.z*1.2f+newSegment.ray.pos.x-0.4f,  pCave->startPosition.y * 1.2f + prevSegment.ray.pos.y - fSeed*1.8f );
			newSegment.ray.dir.y = 8.0f * noise->Get( pCave->startPosition.z*1.3f+newSegment.ray.pos.y-0.5f,  pCave->startPosition.x * 1.3f + prevSegment.ray.pos.x - fSeed*1.7f );

			newSegment.ray.dir += prevSegment.ray.dir.normal();
			newSegment.ray.dir.z *= 0.1f;

			// So randomly go down -4 to -20
			newSegment.ray.dir.z += -12.0f + 16.0f*noise_hf->Get( pCave->startPosition.x*1.3f+iTunnel*0.9f+0.4f,  pCave->startPosition.z * 1.3f + prevSegment.ray.pos.y );
			
			// Normalize it and extend it from 60 to 120 percent of the boob width
			newSegment.ray.dir = newSegment.ray.dir.normal() * ( 0.9f + 0.6f*noise_hf->Get( newSegment.ray.pos.z*0.7f+0.2f+fSeed*2.1f, newSegment.ray.pos.y*2.2f+newSegment.ray.pos.x*0.6f-0.7f-fSeed*2.1f ) );
			if ( newSegment.ray.dir.z > 0 )
				newSegment.ray.dir.z = -newSegment.ray.dir.z;
		}

		// Now make sure the ending position is in the boob area
		Vector3d endPos = newSegment.ray.pos + newSegment.ray.dir;
		if (( fabs( endPos.x-vCurrentCaveCenter.x ) < 1.4f )&&( fabs( endPos.y-vCurrentCaveCenter.y ) < 1.4f )&&( fabs( endPos.z-vCurrentCaveCenter.z ) < 1.7f ))
		{
			// If it's in the area, then we can continue generation and add this tunnel.

			// Only keep going though if it wasn't a downspike
			if ( !isDownspike )
			{
				// With a random height from the previous segment, limited at 1.5 and 5.0
				newSegment.height = prevSegment.height + 1.4f*noise_hf->Get3D( endPos.x*1.3f+0.4f,endPos.z*1.6f,endPos.y*1.4f );
				newSegment.height = max( 1.5f, min( 5.0f, newSegment.height ) );
				// With a random width from the previous segment, limited at 1.0 and 6.0
				newSegment.width = prevSegment.width + 2.0f*noise_hf->Get3D( endPos.x*1.7f-0.4f,endPos.z*1.2f-0.2f,endPos.y*1.6f );
				newSegment.width = max( 1.0f, min( 6.0f, newSegment.width ) );
				// With a random roughness from the previous segment, limited at 0.0 and 0.5
				newSegment.noise = prevSegment.noise + 0.2f*noise_hf->Get( endPos.x+endPos.y*0.2f,endPos.z+0.4f );
				newSegment.noise = max( 0.0f, min( 0.5f, newSegment.noise ) );

				// Add the tunnel to the list and continue generating
				pCave->partTree.push_back( newSegment );
				TerraGen_Bridges_Generate( pCave, pCave->partTree.size()-1, seed );
			}
			else
			{
				// With a random height from the previous segment, limited at 1.5 and 5.0, subtract 2.0
				newSegment.height = prevSegment.height + 1.4f*noise_hf->Get3D( endPos.x*1.3f+0.4f,endPos.z*1.6f,endPos.y*1.4f ) - 2.0f;
				newSegment.height = max( 1.5f, min( 5.0f, newSegment.height ) );
				// With a random width from the previous segment, limited at 1.0 and 6.0, subtract 2.0
				newSegment.width = prevSegment.width + 2.0f*noise_hf->Get3D( endPos.x*1.7f-0.4f,endPos.z*1.2f-0.2f,endPos.y*1.6f ) - 2.0f;
				newSegment.width = max( 1.0f, min( 6.0f, newSegment.width ) );
				// With a random roughness from the previous segment, limited at 0.0 and 0.5
				newSegment.noise = prevSegment.noise + 0.2f*noise_hf->Get( endPos.x+endPos.y*0.2f,endPos.z+0.4f );
				newSegment.noise = max( 0.0f, min( 0.5f, newSegment.noise ) );

				// Add the downspike to the list
				pCave->partTree.push_back( newSegment );
				// Also do another expansion, but in a different way
				TerraGen_Bridges_Generate( pCave, iTunnel, seed+6 );
			}
		}
	}
}
*/