// Includes
#include "CVoxelTerrain.h"
#ifdef _DEAD_
// ========================================
// == Generation Algorithm for Instances ==
// ========================================
// Instance generation
void CVoxelTerrain::TerraGenInstancePass ( CBoob * pBoob, const LongIntPosition & position )
{
	for ( char i = -4; i <= 4; i++ )
		for ( char j = -4; j <= 4; j++ )
			for ( char k = -4; k <= 4; k++ )
				TerraGen_Instances_Create( pBoob, LongIntPosition( position.x+i,position.y+j,position.z+k ) );
	TerraGen_Instances_Generate( pBoob, position );
}
void CVoxelTerrain::TerraGen_Instances_Create ( CBoob * pBoob, const LongIntPosition & position )
{
	// First check if a system already exists in the generation list for this section
	for ( vector<TerraInstance*>::iterator it = vInstanceSystems.begin(); it != vInstanceSystems.end(); it++ )
	{
		if ( (*it)->startPosition == position )
		{
			// If it does exist, then stop now. Don't generate a thang.
			return;
		}
	}

	// If we're still here, we can choose something to generate
	EInstanceType newObject = NST_NULL;
	Vector3d vTargetPosition = Vector3d(
		noise_hf->Get3D( position.x*0.8f+0.3f, position.y*0.8f-0.3f, position.z*0.3f-0.2f )+0.5f+ftype(position.x),
		noise_hf->Get3D( position.y*0.8f+0.3f, position.z*0.8f-0.3f, position.x*0.3f-0.2f )+0.5f+ftype(position.y),
		noise_hf->Get3D( position.z*0.8f+0.3f, position.x*0.8f-0.3f, position.y*0.3f-0.2f )+0.5f+ftype(position.z) );

	newObject = TerraGen_Instances_Choose( pBoob, position, vTargetPosition );

	// If we should generate something
	if ( newObject != NST_NULL )
	{
		//ftype fTempVal;

		// Create a new object
		TerraInstance* terraInst = new TerraInstance;
		terraInst->type = newObject;

		// Set default values
		terraInst->downtrace	= false;
		terraInst->hasdowntraced= false;
		terraInst->massive		= false;
		terraInst->ignore		= false;

		// Set position values
		terraInst->startPosition= position;
		terraInst->position		= vTargetPosition;

		// Depending on type, give it specifics
		switch ( newObject )
		{
		case NST_RUIN_PILLAR:
		case NST_RUIN_SPIKE:
			terraInst->downtrace = true;
			break;
		case NST_DESERT_ROCK_FORMATION_0:
		case NST_DESERT_ROCK_WATER_SPRING:
		case NST_DESERT_OASIS:
			terraInst->downtrace = true;
			break;
		case NST_CRYSTAL_SPIKE:
			terraInst->downtrace = true;
			break;
		default:
			break;
		}
		
		// Also depending on type, add what boobs it affects
		switch ( newObject )
		{
		case NST_RUIN_PILLAR:
		case NST_RUIN_SPIKE:
			// If these objects are too close to an edge, add that edge to the affected areas. Josh likes big butts  // Wow Jacob. Just wow.

			terraInst->affectedAreas.push_back( LongIntPosition( position.x, position.y, position.z ) );
			terraInst->affectedAreas.push_back( LongIntPosition( position.x-1, position.y, position.z ) );
			terraInst->affectedAreas.push_back( LongIntPosition( position.x+1, position.y, position.z ) );
			terraInst->affectedAreas.push_back( LongIntPosition( position.x, position.y-1, position.z ) );
			terraInst->affectedAreas.push_back( LongIntPosition( position.x, position.y+1, position.z ) );
			terraInst->affectedAreas.push_back( LongIntPosition( position.x, position.y, position.z+1 ) );
			terraInst->affectedAreas.push_back( LongIntPosition( position.x, position.y, position.z-1 ) );
			terraInst->affectedAreas.push_back( LongIntPosition( position.x, position.y, position.z+2 ) );
			terraInst->affectedAreas.push_back( LongIntPosition( position.x, position.y, position.z-2 ) );
			break;

		case NST_RAVINE:
			// Ravines. Yay.
			for ( char i = -3; i <= 3; i++ )
				for ( char j = -3; j <= 3; j++ )
					for ( char k = -2; k <= 2; k++ )
						terraInst->affectedAreas.push_back( LongIntPosition( position.x+i, position.y+j, position.z+k ) );
			//
			break;

		case NST_DESERT_ROCK_FORMATION_0:
		case NST_DESERT_ROCK_WATER_SPRING:
		case NST_DESERT_OASIS:
			terraInst->affectedAreas.push_back( LongIntPosition( position.x, position.y, position.z+1 ) );

			terraInst->affectedAreas.push_back( LongIntPosition( position.x, position.y, position.z ) );
			terraInst->affectedAreas.push_back( LongIntPosition( position.x-1, position.y, position.z ) );
			terraInst->affectedAreas.push_back( LongIntPosition( position.x+1, position.y, position.z ) );
			terraInst->affectedAreas.push_back( LongIntPosition( position.x, position.y-1, position.z ) );
			terraInst->affectedAreas.push_back( LongIntPosition( position.x, position.y+1, position.z ) );

			terraInst->affectedAreas.push_back( LongIntPosition( position.x, position.y, position.z-1 ) );
			terraInst->affectedAreas.push_back( LongIntPosition( position.x-1, position.y, position.z-1 ) );
			terraInst->affectedAreas.push_back( LongIntPosition( position.x+1, position.y, position.z-1 ) );
			terraInst->affectedAreas.push_back( LongIntPosition( position.x, position.y-1, position.z-1 ) );
			terraInst->affectedAreas.push_back( LongIntPosition( position.x, position.y+1, position.z-1 ) );
			break;

		case NST_CRYSTAL_SPIKE:

			terraInst->affectedAreas.push_back( LongIntPosition( position.x, position.y, position.z ) );

			break;

		default:

			break;
		}

		// Add the new object to the list
		vInstanceSystems.push_back( terraInst );
	}
}

// Chooses the instance
EInstanceType CVoxelTerrain::TerraGen_Instances_Choose ( CBoob * pBoob, const LongIntPosition & position, const Vector3d & generatedPosition )
{
	EInstanceType type = NST_NULL;
	// Grab terrain height
	ftype fTerrainHeight = TerraGen_1p_GetElevation( generatedPosition );
	ftype fTempVal;

	// Based on the terrain type, generate different things.
	if (( generatedPosition.z >= fTerrainHeight-1.2f )&&( generatedPosition.z <= fTerrainHeight+4.2f ))
	{
		// Do above ground choosing.
		switch ( pBoob->terrain )
		{
		case TER_ISLANDS:
			fTempVal = noise_hhf->Get3D( generatedPosition.x*3.2f, generatedPosition.y*3.2f, generatedPosition.z*3.2f );
			// Either choose a spike or a pillar
			if ( fTempVal > 0.41f )
			{
				type = NST_RUIN_PILLAR;
			}
			else if ( fTempVal > 0.33f )
			{
				type = NST_RUIN_SPIKE;
			}
			else
			{
				
			}
			break;

		case TER_FLATLANDS:
		case TER_HILLLANDS:
			fTempVal = noise_hhf->Get3D( generatedPosition.x*3.2f, generatedPosition.y*3.2f, generatedPosition.z*3.2f );
			if ( fTempVal > 0.44f )
			{
				type = NST_RUIN_PILLAR;
			}
			else if ( fTempVal > 0.38f )
			{
				type = NST_RUIN_SPIKE;
			}
			else if ( fTempVal > 0.35f )
			{
				type = NST_RAVINE;
			}
			else if ( fTempVal > 0.34f )
			{
				type = NST_CRYSTAL_SPIKE;
			}
			break;

		case TER_DESERT:
			fTempVal = noise_hhf->Get3D( generatedPosition.x*4.1f, generatedPosition.y*4.1f, generatedPosition.z*4.1f );
			if ( fTempVal > 0.43f )
			{
				type = NST_DESERT_ROCK_FORMATION_0;
			}
			else if ( fTempVal > 0.41f )
			{
				type = NST_DESERT_ROCK_WATER_SPRING;
			}
			else if ( fTempVal > 0.403f )
			{
				type = NST_DESERT_OASIS;
			}
			else if ( fTempVal > 0.40f )
			{
				type = NST_DESERT_MIRAGE;
			}
			break;

		case TER_OCEAN:
			fTempVal = noise_hhf->Get3D( generatedPosition.x*3.2f, generatedPosition.y*3.2f, generatedPosition.z*3.2f );
			if ( fTempVal > 0.47f )
			{
				type = NST_RAVINE;
			}
			break;

		case TER_DEFAULT:
			cout << "Error in generation: TERRAIN TYPE NOT SET" << endl;
			break;

		default:

			break;
		}
	}
	else
	{
		// Do below ground choosing
		switch ( pBoob->terrain )
		{
		case TER_THE_EDGE:
			fTempVal = noise_hhf->Get3D( generatedPosition.x*3.2f, generatedPosition.y*3.2f, generatedPosition.z*3.2f );
			if ( fTempVal > 0.35f )
			{
				type = NST_CRYSTAL_SPIKE;
			}
			break;

		case TER_FLATLANDS:
		case TER_HILLLANDS:
			fTempVal = noise_hhf->Get3D( generatedPosition.x*3.2f, generatedPosition.y*3.2f, generatedPosition.z*3.2f );
			if ( fTempVal > 0.45f )
			{
				type = NST_CRYSTAL_SPIKE;
			}
			break;

		case TER_DEFAULT:
			cout << "Error in generation: TERRAIN TYPE NOT SET" << endl;
			break;

		default:

			break;
		}
	}

	return type;
}


// Generate the instances now
void CVoxelTerrain::TerraGen_Instances_Generate( CBoob * pBoob, const LongIntPosition & position )
{
	// Loop through all the massive instances first
	for ( vector<TerraInstance*>::iterator it = vInstanceSystems.begin(); it != vInstanceSystems.end(); it++ )
	{
		if ( (*it)->massive )
		{
			TerraGen_Instances_Generate_Work( (*it), pBoob, position );
		}
	}
	// Loop through the smaller instances next
	for ( vector<TerraInstance*>::iterator it = vInstanceSystems.begin(); it != vInstanceSystems.end(); it++ )
	{
		if ( !(*it)->massive )
		{
			TerraGen_Instances_Generate_Work( (*it), pBoob, position );
		}
	}
}

// Look for this current boob index
void CVoxelTerrain::TerraGen_Instances_Generate_Work( TerraInstance * terraInst, CBoob * pBoob, const LongIntPosition & position )
{
	// If ignore, skip
	if ( terraInst->ignore )
	{
		return;
	}

	// First check for the downtrace
	if ( terraInst->downtrace )
	{
		if ( !terraInst->hasdowntraced )
		{
			// Downtrace the instance
			/*if ( !TerraGen_Instances_Downtrace( terraInst, pBoob, position ) )
				if ( !TerraGen_Instances_Downtrace_Fallback( terraInst, pBoob, position ) )
				{
					cout << "Warning in " << __FILE__ << "(" << __LINE__ << ")" << ": floatin' instance" << endl;
					terraInst->ignore = true;
				}*/
			ftype fTerrainHeight = TerraGen_1p_GetElevation( position );
			if ( terraInst->position.z > (fTerrainHeight-0.2f) )
			{
				terraInst->position.z = ((terraInst->position.z*0.05f)+(fTerrainHeight-0.3f))*0.95f - 0.1f;
				terraInst->hasdowntraced = true;
			}
			else if ( terraInst->position.z > (fTerrainHeight-0.8f) )
			{
				terraInst->position.z -= 0.5f;
				terraInst->hasdowntraced = true;
			}
			else
			{
				terraInst->hasdowntraced = false;
				terraInst->ignore = true;
			}
		}
	}

	// Look for if the current boob may be affected
	vector<LongIntPosition>::iterator searchResult = find( terraInst->affectedAreas.begin(), terraInst->affectedAreas.end(), position );

	// If it can be affected
	if ( searchResult != terraInst->affectedAreas.end() )
	{
		// Check if can generate
		if ((( terraInst->downtrace && terraInst->hasdowntraced ))||( !terraInst->downtrace ))
		{
			//cout << "Instance affected sector: " << position.x << "," << position.y << "," << position.z << endl;
			//cout << "Instance position: " << terraInst->position.x << "," << terraInst->position.y << "," << terraInst->position.z  << endl;
			// Then for each block
			for ( char i = 0; i < 8; i += 1 )
			{
				for ( char j = 0; j < 8; j += 1 )
				{
					for ( int k = 0; k < 512; k += 1 )
					{
						// Declared static to save init time
						static Vector3d blockPosition;
						static unsigned char block;

						blockPosition.x = position.x + (((i%2) * 16) + ((j%2) * 8) + (k%8))/32.0f;
						blockPosition.y = position.y + ((((i/2)%2) * 16) + (((j/2)%2) * 8) + ((k/8)%8))/32.0f;
						blockPosition.z = position.z + (((i/4) * 16) + ((j/4) * 8)  + (k/64))/32.0f;

						// Get current block value
						block = pBoob->data[i].data[j].data[k];

						// Do generation based on type
						switch ( terraInst->type )
						{
						case NST_RUIN_SPIKE:
							block = TerraGen_Instances_RuinSpike	( terraInst, blockPosition, block );
							break;
						case NST_RUIN_PILLAR:
							block = TerraGen_Instances_RuinPillar	( terraInst, blockPosition, block );
							break;
						case NST_RAVINE:
							block = TerraGen_Instances_Ravine	( terraInst, blockPosition, block );
							break;
						case NST_DESERT_ROCK_FORMATION_0:
							block = TerraGen_Instances_Desert_Rocks0	( terraInst, blockPosition, block );
							break;
						case NST_DESERT_ROCK_WATER_SPRING:
							block = TerraGen_Instances_Desert_RockWtr	( terraInst, blockPosition, block );
							break;
						case NST_DESERT_OASIS:
							block = TerraGen_Instances_Desert_Oasis	( terraInst, blockPosition, block );
							break;
						case NST_CRYSTAL_SPIKE:
							block = TerraGen_Instances_Crystal_Spike( terraInst, blockPosition, block );
							break;
						default:
							cout << "Invalid TerraInstance generated." << endl;
							break;
						}

						// Set new block type
						pBoob->data[i].data[j].data[k] = block;
					}
				}
			}
		}
		else
		{
			// If can't generate, we got an issue
			/*if ( terraInst->downtrace )
			{
				cout << "Error in generation!: attempted generation of a downtraced object that is out of memory bounds!" << endl;
				cout << "     ( note that massive instances must be manually placed )" << endl;
			}*/
			if (( terraInst->downtrace )&&( terraInst->massive ))
			{
				cout << "Error in generation!: attempted generation of a downtraced object that is out of memory bounds!" << endl;
				cout << "     ( note that massive instances must be manually placed )" << endl;
			}
		}
	}
}

// Downtrace the instance
bool CVoxelTerrain::TerraGen_Instances_Downtrace ( TerraInstance * terraInst, CBoob * pBoob, const LongIntPosition & position )
{
	// First navigate to the wanted boob.
	LongIntPosition currentPosition = position;
	CBoob* currentBoob = pBoob;

	// If we can't get to the wanted boob, we just quit

	BlockInfo currentBlock;
	if ( GetBlockInfoAtPosition( terraInst->position, currentBlock ) )
	{
		// Now once we're here, we get the current block that we're at
		while (( currentBlock.block == EB_NONE )||( currentBlock.block == EB_WATER ))
		{
			// and go downwards until we hit ground!
			if ( !TraverseTree( &(currentBlock.pBoob), currentBlock.b16index, currentBlock.b8index, currentBlock.b1index, BOTTOM ) )
				return false;
			currentBlock.block = currentBlock.pBoob->data[currentBlock.b16index].data[currentBlock.b8index].data[currentBlock.b1index];
		}

		// Now we're at the ground, set the new position
		char i = currentBlock.b16index;
		char j = currentBlock.b8index;
		short k = currentBlock.b1index;
		terraInst->position.x = (currentBlock.pBoob->position.x-32)/64 + (((i%2) * 16) + ((j%2) * 8) + (k%8))/32.0f;
		terraInst->position.y = (currentBlock.pBoob->position.y-32)/64 + ((((i/2)%2) * 16) + (((j/2)%2) * 8) + ((k/8)%8))/32.0f;
		terraInst->position.z = (currentBlock.pBoob->position.z-32)/64+ (((i/4) * 16) + ((j/4) * 8)  + (k/64))/32.0f;

		// Now set that the terra inst has been downtraced
		terraInst->hasdowntraced = true;

		// Return success
		return true;
	}
	return false;
}
// Downtrace the instance with the hacky method
bool CVoxelTerrain::TerraGen_Instances_Downtrace_Fallback ( TerraInstance * terraInst, CBoob * pBoob, const LongIntPosition & position )
{
	// First navigate to the wanted boob.
	LongIntPosition currentPosition = position;
	CBoob* currentBoob = pBoob;

	// Turn the terrainstance position into a block position
	BlockInfo currentBlock;
	currentBlock.pBoob = pBoob;
	{
		Vector3d pos = Vector3d(
			terraInst->position.x - ftype( position.x ),
			terraInst->position.y - ftype( position.y ),
			terraInst->position.z - ftype( position.z )
			);
		ftype i, dummy;

		// Find current subblock16
		pos.x = modf( pos.x, &dummy );
		pos.y = modf( pos.y, &dummy );
		pos.z = modf( pos.z, &dummy );
		if ( pos.x < 0 )
			pos.x += 1;
		if ( pos.y < 0 )
			pos.y += 1;
		if ( pos.z < 0 )
			pos.z += 1;

		pos *= 2.0f;

		currentBlock.b16index = 0;
		if ( pos.x > 1.0f )
			currentBlock.b16index += 1;
		if ( pos.y > 1.0f )
			currentBlock.b16index += 2;
		if ( pos.z > 1.0f )
			currentBlock.b16index += 4;
		currentBlock.pBlock16 = &(currentBlock.pBoob->data[currentBlock.b16index]);
		// Found current subblock16

		// Find current subblock8
		pos.x = modf( pos.x, &dummy );
		pos.y = modf( pos.y, &dummy );
		pos.z = modf( pos.z, &dummy );

		pos *= 2.0f;

		currentBlock.b8index = 0;
		if ( pos.x > 1.0f )
			currentBlock.b8index += 1;
		if ( pos.y > 1.0f )
			currentBlock.b8index += 2;
		if ( pos.z > 1.0f )
			currentBlock.b8index += 4;
		currentBlock.pBlock8 = &(currentBlock.pBoob->data[currentBlock.b16index].data[currentBlock.b8index]);
		// Found current subblock8

		// Find current block
		pos.x = modf( pos.x, &dummy );
		pos.y = modf( pos.y, &dummy );
		pos.z = modf( pos.z, &dummy );

		pos *= 8.0f;

		currentBlock.b1index = 0;
		for ( i = 1; i < pos.x; i += 1 )
			currentBlock.b1index += 1;
		for ( i = 1; i < pos.y; i += 1 )
			currentBlock.b1index += 8;
		for ( i = 1; i < pos.z; i += 1 )
			currentBlock.b1index += 64;
		currentBlock.block = currentBlock.pBoob->data[currentBlock.b16index].data[currentBlock.b8index].data[currentBlock.b1index];
		currentBlock.pBlock = &(currentBlock.pBoob->data[currentBlock.b16index].data[currentBlock.b8index].data[currentBlock.b1index]);
		// Found current block
	}


	// Now once we're here, we get the current block that we're at
	bool failed = false;
	while (( currentBlock.block == EB_NONE )||( currentBlock.block == EB_WATER ))
	{
		// and go downwards until we hit ground!
		if ( !TraverseTree( &(currentBlock.pBoob), currentBlock.b16index, currentBlock.b8index, currentBlock.b1index, BOTTOM ) )
		{
			failed = true;
			break;
		}
		currentBlock.block = currentBlock.pBoob->data[currentBlock.b16index].data[currentBlock.b8index].data[currentBlock.b1index];
	}

	// If we hit a block
	if ( !failed )
	{
		// Now we're at the ground, set the new position
		char i = currentBlock.b16index;
		char j = currentBlock.b8index;
		short k = currentBlock.b1index;
		//terraInst->position.x = (currentBlock.pBoob->position.x-32)/64 + (((i%2) * 16) + ((j%2) * 8) + (k%8))/32.0f;
		//terraInst->position.y = (currentBlock.pBoob->position.y-32)/64 + ((((i/2)%2) * 16) + (((j/2)%2) * 8) + ((k/8)%8))/32.0f;
		terraInst->position.z = (currentBlock.pBoob->position.z-32)/64+ (((i/4) * 16) + ((j/4) * 8)  + (k/64))/32.0f;
		// Now set that the terra inst has been downtraced
		terraInst->hasdowntraced = true;

		// Now, because we need it, uptrace.
		TerraGen_Instances_Uptrace_Fallback( terraInst, pBoob, position );

		// Return success
		return true;
	}
	/*else
	{
		// If we didn't hit anything, then we just round the position down
		terraInst->position.z = (ftype)floor( terraInst->position.z );


		// Now set that the terra inst has been downtraced
		terraInst->hasdowntraced = true;

		// Return success
		return true;
	}*/


	return false;
}

// Downtrace the instance with the hacky method
bool CVoxelTerrain::TerraGen_Instances_Uptrace_Fallback ( TerraInstance * terraInst, CBoob * pBoob, const LongIntPosition & position )
{
	// First navigate to the wanted boob.
	LongIntPosition currentPosition = position;
	CBoob* currentBoob = pBoob;

	// Turn the terrainstance position into a block position
	BlockInfo currentBlock;
	currentBlock.pBoob = pBoob;
	{
		Vector3d pos = Vector3d(
			terraInst->position.x - ftype( position.x ),
			terraInst->position.y - ftype( position.y ),
			terraInst->position.z - ftype( position.z )
			);
		ftype i, dummy;

		// Find current subblock16
		pos.x = modf( pos.x, &dummy );
		pos.y = modf( pos.y, &dummy );
		pos.z = modf( pos.z, &dummy );
		if ( pos.x < 0 )
			pos.x += 1;
		if ( pos.y < 0 )
			pos.y += 1;
		if ( pos.z < 0 )
			pos.z += 1;

		pos *= 2.0f;

		currentBlock.b16index = 0;
		if ( pos.x > 1.0f )
			currentBlock.b16index += 1;
		if ( pos.y > 1.0f )
			currentBlock.b16index += 2;
		if ( pos.z > 1.0f )
			currentBlock.b16index += 4;
		currentBlock.pBlock16 = &(currentBlock.pBoob->data[currentBlock.b16index]);
		// Found current subblock16

		// Find current subblock8
		pos.x = modf( pos.x, &dummy );
		pos.y = modf( pos.y, &dummy );
		pos.z = modf( pos.z, &dummy );

		pos *= 2.0f;

		currentBlock.b8index = 0;
		if ( pos.x > 1.0f )
			currentBlock.b8index += 1;
		if ( pos.y > 1.0f )
			currentBlock.b8index += 2;
		if ( pos.z > 1.0f )
			currentBlock.b8index += 4;
		currentBlock.pBlock8 = &(currentBlock.pBoob->data[currentBlock.b16index].data[currentBlock.b8index]);
		// Found current subblock8

		// Find current block
		pos.x = modf( pos.x, &dummy );
		pos.y = modf( pos.y, &dummy );
		pos.z = modf( pos.z, &dummy );

		pos *= 8.0f;

		currentBlock.b1index = 0;
		for ( i = 1; i < pos.x; i += 1 )
			currentBlock.b1index += 1;
		for ( i = 1; i < pos.y; i += 1 )
			currentBlock.b1index += 8;
		for ( i = 1; i < pos.z; i += 1 )
			currentBlock.b1index += 64;
		currentBlock.block = currentBlock.pBoob->data[currentBlock.b16index].data[currentBlock.b8index].data[currentBlock.b1index];
		currentBlock.pBlock = &(currentBlock.pBoob->data[currentBlock.b16index].data[currentBlock.b8index].data[currentBlock.b1index]);
		// Found current block
	}


	// Now once we're here, we get the current block that we're at
	bool failed = false;
	while (( currentBlock.block != EB_NONE )&&( currentBlock.block != EB_WATER ))
	{
		// and go downwards until we hit air!
		if ( !TraverseTree( &(currentBlock.pBoob), currentBlock.b16index, currentBlock.b8index, currentBlock.b1index, TOP ) )
		{
			failed = true;
			break;
		}
		currentBlock.block = currentBlock.pBoob->data[currentBlock.b16index].data[currentBlock.b8index].data[currentBlock.b1index];
	}

	// If we hit a block
	if ( !failed )
	{
		// Now we're at the ground, set the new position
		char i = currentBlock.b16index;
		char j = currentBlock.b8index;
		short k = currentBlock.b1index;
		//terraInst->position.x = (currentBlock.pBoob->position.x-32)/64 + (((i%2) * 16) + ((j%2) * 8) + (k%8))/32.0f;
		//terraInst->position.y = (currentBlock.pBoob->position.y-32)/64 + ((((i/2)%2) * 16) + (((j/2)%2) * 8) + ((k/8)%8))/32.0f;
		terraInst->position.z = (currentBlock.pBoob->position.z-32)/64+ (((i/4) * 16) + ((j/4) * 8)  + (k/64))/32.0f - (1/32.0f);

		// Now set that the terra inst has been downtraced
		//

		// Return success
		return true;
	}


	return false;
}

//= = == === ===== === ===== === == = =//
//      Instance Generation Types      //
//= = == === ===== === ===== === == = =//
char CVoxelTerrain::TerraGen_Instances_RuinSpike ( TerraInstance * terraInst, const Vector3d & blockPosition, char block )
{
	Vector2d difVect = Vector2d( terraInst->position.x - blockPosition.x, terraInst->position.y - blockPosition.y );
	if ( ( difVect.magnitude() < 1.7f/32 )
		&&( abs( (terraInst->position.z+0.4f) - blockPosition.z ) < 0.43f ))
	{

		ftype temp;
		temp = noise_hf->Get( blockPosition.x*9.23f, blockPosition.y*9.23f )*1.2f+0.43f;
		if ( temp < 0 )
			temp = 0;

		if ( blockPosition.z < terraInst->position.z + temp )
		{
			block = EB_STONEBRICK;
		}
	}
	return block;
}
char CVoxelTerrain::TerraGen_Instances_RuinPillar ( TerraInstance * terraInst, const Vector3d & blockPosition, char block )
{
	Vector2d difVect = Vector2d( terraInst->position.x - blockPosition.x, terraInst->position.y - blockPosition.y );
	if ( ( difVect.magnitude() < 2.6f/32 )
		&&( abs( (terraInst->position.z+0.4f) - blockPosition.z ) < 0.47f ))
	{

		ftype temp;
		temp = noise_hf->Get( blockPosition.x*9.23f, blockPosition.y*9.23f )*1.2f+0.43f;
		if ( temp < 0 )
			temp = 0;

		if ( blockPosition.z < terraInst->position.z + temp )
		{
			block = EB_STONEBRICK;
		}
	}
	return block;
}
char CVoxelTerrain::TerraGen_Instances_Ravine ( TerraInstance * terraInst, const Vector3d & blockPosition, char block )
{
	Vector2d difVect = Vector2d( terraInst->position.x - blockPosition.x, terraInst->position.y - blockPosition.y );

	ftype angle = noise_hf->Get3D( terraInst->position.x + blockPosition.x*0.01f, terraInst->position.y + blockPosition.y*0.01f, terraInst->position.z + blockPosition.z*0.01f ) * 6.283f;
	Vector2d dirVect = Vector2d( sin( angle ), cos( angle ) );

	Vector2d distVect = (difVect - dirVect*difVect.dot( dirVect ));

	ftype width = 0.0f
		+ noise_hf->Get3D( blockPosition.x, blockPosition.y, blockPosition.z )*0.02f
		+ noise->Get3D( blockPosition.x, blockPosition.y, blockPosition.z*2.0f )*0.03f
		+ 1.2f/32 
		+ 0.03f/difVect.magnitude() 
		+ sqr(blockPosition.z-terraInst->position.z+2.0f)*0.01f
		+ (noise->Get3D( blockPosition.z*2.8f, terraInst->position.x, terraInst->position.y )+0.5f)*0.09f*(blockPosition.z-terraInst->position.z+2.0f);
		//+ (noise->Get3D( blockPosition.z*3.5f, terraInst->position.x, terraInst->position.y )+0.5f)*0.1f;
	if ( distVect.magnitude() < width )
	{
		ftype temp;
		temp = noise->Get( blockPosition.x, blockPosition.y );

		if ( blockPosition.z > terraInst->position.z+temp-1.4f )
		{
			block = EB_NONE;
		}
	}
	
	return block;
}
char CVoxelTerrain::TerraGen_Instances_Desert_Rocks0	( TerraInstance * terraInst, const Vector3d & blockPosition, char block )
{
	Vector2d difVect = Vector2d( terraInst->position.x - blockPosition.x, terraInst->position.y - blockPosition.y );

	if ( blockPosition.z < (terraInst->position.z + 1.02f) )
	{
		ftype width =
			noise->Get3D( blockPosition.x*0.7f, blockPosition.y*0.7f, blockPosition.z*0.5f ) * 0.2f
			+ 0.09f
			+ noise->Get3D( terraInst->position.x, terraInst->position.y, terraInst->position.z )*0.1f+0.05f
			+ sqr( (terraInst->position.z + 1) - blockPosition.z )*0.25f
			+ noise->Get3D( blockPosition.y*3.2f, blockPosition.z*3.2f, blockPosition.x*5.2f ) * 0.1f;
		if ( difVect.magnitude() < width*0.6f )
		{
			if ( block == EB_NONE )
			{
				if ( blockPosition.z > (terraInst->position.z-1) ) // If up in air, above ground, then set block to stone
				{
					block = EB_STONE;
				}
			}
			else
			{
				block = EB_STONE;
			}
		}
	}

	return block;
}
char CVoxelTerrain::TerraGen_Instances_Desert_RockWtr	( TerraInstance * terraInst, const Vector3d & blockPosition, char block )
{
	return TerraGen_Instances_Desert_Rocks0( terraInst, blockPosition, block );
}
char CVoxelTerrain::TerraGen_Instances_Desert_Oasis	( TerraInstance * terraInst, const Vector3d & blockPosition, char block )
{
	if ( block != EB_NONE )
	{
		Vector2d difVect = Vector2d( terraInst->position.x - blockPosition.x, terraInst->position.y - blockPosition.y );

		ftype width = 
			(terraInst->position.z - blockPosition.z + 1)*0.17f
			+ noise->Get3D( blockPosition.x*4.2f, blockPosition.y*4.2f, blockPosition.z*4.2f )*0.5f
			+ 0.1f;
		ftype mag = difVect.magnitude();
		if (( mag < 0.04f )&&( blockPosition.z < (terraInst->position.x-0.2f) ))
		{
			block = EB_WATERSPRING;
		}
		else if ( mag < width )
		{
			if ( block == EB_SAND )
			{
				block = EB_WATER;
			}
		}
		else if (( mag < width+0.09f )&&( blockPosition.z < (terraInst->position.x-0.1f) )&&( blockPosition.z > (terraInst->position.x-0.2f) ))
		{
			if ( block == EB_SAND )
			{
				block = EB_MUD;
			}
		}
		else if (( mag < width+0.07f )&&( blockPosition.z < (terraInst->position.x-0.2f) ))
		{
			if ( block == EB_SAND )
			{
				block = EB_CLAY;
			}
		}
	}

	return block;
}

char CVoxelTerrain::TerraGen_Instances_Crystal_Spike	( TerraInstance * terraInst, const Vector3d & blockPosition, char block )
{
	Vector2d difVect = Vector2d( terraInst->position.x - blockPosition.x, terraInst->position.y - blockPosition.y );
	if ( ( difVect.magnitude() < 1.7f/32 )
		&&( abs( (terraInst->position.z+0.4f) - blockPosition.z ) < 0.37f ))
	{

		ftype temp;
		temp = noise_hf->Get( blockPosition.x*9.23f, blockPosition.y*9.23f )*0.9f+0.33f;
		if ( temp < 0 )
			temp = 0;

		if (( blockPosition.z < terraInst->position.z + temp )||( blockPosition.z > terraInst->position.z - temp*0.1f ))
		{
			block = EB_CRYSTAL;
		}
	}
	return block;
}

#endif