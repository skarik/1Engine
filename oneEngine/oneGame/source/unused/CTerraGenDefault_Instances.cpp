
// ======== CTerraGenDefault_Instances =======
// This is the definition for the second pass instance creation.
// One-time objects are handled here.

#include "CTerraGenDefault.h"
#include "CTownManager.h" // Has the definitions for the VXG file loading
#include "Math.h"

using std::cout;
using std::endl;

// ========================================
// == Generation Algorithm for Instances ==
// ========================================
// Instance generation
void CTerraGenDefault::TerraGenInstancePass ( CBoob * pBoob, const RangeVector & position )
{
	TerraGen_Instances_Global_Create();
	for ( char i = -4; i <= 4; i++ )
		for ( char j = -4; j <= 4; j++ )
			for ( char k = -4; k <= 4; k++ )
				TerraGen_Instances_Create( pBoob, RangeVector( position.x+i,position.y+j,position.z+k ) );
	TerraGen_Instances_Generate( pBoob, position );
}
void CTerraGenDefault::TerraGen_Instances_Global_Create ( void )
{
	static bool created = false;
	if ( created ) {
		return;
	}
	created = true;

	// Now, going to start creating so called 'global' instances. 

	// NST_DEADLANDS_CRATER
	{
		// There is 1 deadlands crater on terra worlds
		TerraInstance* terraInst = new TerraInstance;
		terraInst->type = NST_DEADLANDS_CRATER;

		// Set default values
		terraInst->downtrace	= false;
		terraInst->hasdowntraced= false;
		terraInst->massive		= true;
		terraInst->ignore		= false;

		// Generate random position for the crater
		Vector3d vTargetPosition = Vector3d(
			noise_hf->Get3D( 1.1f, 0.53f, 0.24f ),
			noise_hf->Get3D( 0.32f, 1.7f, 0.73f ),
			0 );
		vTargetPosition = vTargetPosition.normal() * 20.0f;
		ftype fTerrainHeight = TerraGen_1p_GetElevation( vTargetPosition );
		vTargetPosition.z = fTerrainHeight;
		terraInst->position		= vTargetPosition;
		// Set position values
		terraInst->startPosition= RangeVector( (int)vTargetPosition.x,(int)vTargetPosition.y,(int)vTargetPosition.z );

		// Create affected areas
		for ( char i = -5; i <= 5; i++ ) {
			for ( char j = -5; j <= 5; j++ ) {
				for ( char k = -3; k <= 3; k++ ) {
					terraInst->affectedAreas.push_back(
						RangeVector(
							terraInst->startPosition.x + i,
							terraInst->startPosition.y + j,
							terraInst->startPosition.z + k )
						);
				}
			}
		}

		// Add the new object to the list
		vInstanceSystems.push_back( terraInst );
	}
	
}
void CTerraGenDefault::TerraGen_Instances_Create ( CBoob * pBoob, const RangeVector & position )
{
	// First check if an instance already exists in the generation list for this section
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

		// Set generation mode
		if ( terraInst->type < NST_SYSTEM_END_PROCEDURAL_LIST ) {
			terraInst->mode = 0;
		}
		else if ( terraInst->type < NST_SYSTEM_END_VXG_LIST ) {
			terraInst->mode = 1;
		}

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
		case NST_VEIN_CRYSTAL:
		case NST_VEIN_CLAY:
			terraInst->downtrace = true;
			break;
		case NST_BOULDER_SMALL:
			terraInst->downtrace = true;
			break;
		case NST_PATH_GRAVEL:
			terraInst->downtrace = true;
			break;
		case NST_ORE_VEIN:
		case NST_ORE_SPLOTCH:
		case NST_ORE_SPLATTER:
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
			// If these objects are too close to an edge, add that edge to the affected areas.

			terraInst->affectedAreas.push_back( RangeVector( position.x, position.y, position.z ) );
			terraInst->affectedAreas.push_back( RangeVector( position.x-1, position.y, position.z ) );
			terraInst->affectedAreas.push_back( RangeVector( position.x+1, position.y, position.z ) );
			terraInst->affectedAreas.push_back( RangeVector( position.x, position.y-1, position.z ) );
			terraInst->affectedAreas.push_back( RangeVector( position.x, position.y+1, position.z ) );
			terraInst->affectedAreas.push_back( RangeVector( position.x, position.y, position.z+1 ) );
			terraInst->affectedAreas.push_back( RangeVector( position.x, position.y, position.z-1 ) );
			terraInst->affectedAreas.push_back( RangeVector( position.x, position.y, position.z+2 ) );
			terraInst->affectedAreas.push_back( RangeVector( position.x, position.y, position.z-2 ) );
			terraInst->affectedAreas.push_back( RangeVector( position.x, position.y, position.z-3 ) );
			break;

		case NST_RAVINE:
		case NST_VEIN_CRYSTAL:
		case NST_VEIN_CLAY:
			// Ravines. Yay.
			for ( char i = -3; i <= 3; i++ )
				for ( char j = -3; j <= 3; j++ )
					for ( char k = -2; k <= 2; k++ )
						terraInst->affectedAreas.push_back( RangeVector( position.x+i, position.y+j, position.z+k ) );
			//
			break;

		case NST_ORE_VEIN:
		case NST_ORE_SPLOTCH:
		case NST_ORE_SPLATTER:
			for ( char i = -2; i <= 2; i++ )
				for ( char j = -2; j <= 2; j++ )
					for ( char k = -2; k <= 2; k++ )
						terraInst->affectedAreas.push_back( RangeVector( position.x+i, position.y+j, position.z+k ) );
			//
			break;

		case NST_PATH_GRAVEL:
			for ( char i = -4; i <= 4; i++ )
				for ( char j = -4; j <= 4; j++ )
					for ( char k = -2; k <= 2; k++ )
						terraInst->affectedAreas.push_back( RangeVector( position.x+i, position.y+j, position.z+k ) );
			//
			break;

		case NST_DESERT_ROCK_FORMATION_0:
		case NST_DESERT_ROCK_WATER_SPRING:
		case NST_DESERT_OASIS:

			terraInst->affectedAreas.push_back( RangeVector( position.x, position.y, position.z+1 ) );

			terraInst->affectedAreas.push_back( RangeVector( position.x, position.y, position.z ) );
			terraInst->affectedAreas.push_back( RangeVector( position.x-1, position.y, position.z ) );
			terraInst->affectedAreas.push_back( RangeVector( position.x+1, position.y, position.z ) );
			terraInst->affectedAreas.push_back( RangeVector( position.x, position.y-1, position.z ) );
			terraInst->affectedAreas.push_back( RangeVector( position.x, position.y+1, position.z ) );

			terraInst->affectedAreas.push_back( RangeVector( position.x, position.y, position.z-1 ) );
			terraInst->affectedAreas.push_back( RangeVector( position.x-1, position.y, position.z-1 ) );
			terraInst->affectedAreas.push_back( RangeVector( position.x+1, position.y, position.z-1 ) );
			terraInst->affectedAreas.push_back( RangeVector( position.x, position.y-1, position.z-1 ) );
			terraInst->affectedAreas.push_back( RangeVector( position.x, position.y+1, position.z-1 ) );
			break;

		case NST_CRYSTAL_SPIKE:
		case NST_BOULDER_SMALL:
			terraInst->affectedAreas.push_back( RangeVector( position.x, position.y, position.z ) );
			terraInst->affectedAreas.push_back( RangeVector( position.x, position.y, position.z-1 ) );
			terraInst->affectedAreas.push_back( RangeVector( position.x, position.y, position.z-2 ) );
			terraInst->affectedAreas.push_back( RangeVector( position.x, position.y, position.z-3 ) );
			terraInst->affectedAreas.push_back( RangeVector( position.x, position.y, position.z+1 ) );

			break;

		default:

			break;
		}

		// Add the new object to the list
		vInstanceSystems.push_back( terraInst );
	}
}

#include "random_vector.h"
// Chooses the instance
EInstanceType CTerraGenDefault::TerraGen_Instances_Choose ( CBoob * pBoob, const RangeVector & position, const Vector3d & generatedPosition )
{
	EInstanceType type = NST_NULL;
	// Grab terrain height
	ftype fTerrainHeight = TerraGen_1p_GetElevation( generatedPosition );
	ftype fTempVal;

	random_vector<EInstanceType> instanceList;

	unsigned char sampledType = TerraGen_priv_GetType( generatedPosition );
	/*if ( sampledType != pBoob->terrain ) {
		cout << "Mismatch: " << (int)sampledType << " : " << (int)pBoob->terrain << endl;
	}*/

	// Based on the terrain type, generate different things.
	if (( generatedPosition.z >= fTerrainHeight-1.2f )&&( generatedPosition.z <= fTerrainHeight+4.2f ))
	{
		// Do above ground choosing.
		switch ( sampledType )
		{
		case TER_ISLANDS:
			fTempVal = noise->Unnormalize(noise_hhf->Get3D( generatedPosition.x*3.2f, generatedPosition.y*3.2f, generatedPosition.z*3.2f ))+0.5f;
			// Either choose a spike or a pillar
			instanceList.push_back( NST_NULL, 1.0 );
			instanceList.push_back( NST_RUIN_PILLAR, 9.0 );
			instanceList.push_back( NST_RUIN_SPIKE, 8.0 );
			instanceList.push_back( NST_ORE_VEIN, 3.0 );
			instanceList.push_back( NST_ORE_SPLOTCH, 4.0 );
			instanceList.push_back( NST_NULL, 43.0 );
			// Cast list
			type = instanceList.get_target_value( fTempVal );
			break;

		case TER_FLATLANDS:
		case TER_HILLLANDS:
			fTempVal = noise->Unnormalize(noise_hhf->Get3D( generatedPosition.x*2.7f, generatedPosition.y*2.7f, generatedPosition.z*2.7f ))+0.5f;
			// Create choice list
			instanceList.push_back( NST_NULL, 20.0 );
			instanceList.push_back( NST_RUIN_PILLAR, 7.0 );
			instanceList.push_back( NST_RUIN_SPIKE, 8.0 );
			instanceList.push_back( NST_RAVINE, 0.4 );
			instanceList.push_back( NST_CRYSTAL_SPIKE, 1.0 );
			instanceList.push_back( NST_VEIN_CRYSTAL, 3.0 );
			instanceList.push_back( NST_BOULDER_SMALL, 3.0 );
			instanceList.push_back( NST_PATH_GRAVEL, 1.0 );
			instanceList.push_back( NST_ORE_VEIN, 3.0 );
			instanceList.push_back( NST_ORE_SPLOTCH, 4.0 );
			instanceList.push_back( NST_ORE_SPLATTER, 5.0 );
			instanceList.push_back( NST_NULL, 40.0 );
			// Cast list
			type = instanceList.get_target_value( fTempVal );
			break;

		case TER_MOUNTAINS:
			fTempVal = noise->Unnormalize(noise_hhf->Get3D( generatedPosition.x*2.7f, generatedPosition.y*2.7f, generatedPosition.z*2.7f ))+0.5f;
			// Create choice list
			instanceList.push_back( NST_NULL, 20.0 );
			instanceList.push_back( NST_VEIN_CRYSTAL, 2.0 );
			instanceList.push_back( NST_VEIN_CLAY, 4.0 );
			instanceList.push_back( NST_RAVINE, 0.4 );
			instanceList.push_back( NST_PATH_GRAVEL, 2.5 );
			instanceList.push_back( NST_ORE_VEIN, 4.0 );
			instanceList.push_back( NST_ORE_SPLOTCH, 3.0 );
			instanceList.push_back( NST_NULL, 40.0 );
			// Cast list
			type = instanceList.get_target_value( fTempVal );
			break;

		case TER_DESERT:
			fTempVal = noise->Unnormalize(noise_hhf->Get3D( generatedPosition.x*4.1f, generatedPosition.y*4.1f, generatedPosition.z*4.1f ))+0.5f;
			// Create choice list
			instanceList.push_back( NST_NULL, 20.0 );
			instanceList.push_back( NST_DESERT_ROCK_FORMATION_0, 7.0 );
			instanceList.push_back( NST_DESERT_ROCK_WATER_SPRING, 2.0 );
			instanceList.push_back( NST_DESERT_OASIS, 0.7 );
			instanceList.push_back( NST_DESERT_MIRAGE, 0.3 );
			instanceList.push_back( NST_VEIN_CLAY, 2.0 );
			instanceList.push_back( NST_ORE_SPLATTER, 0.5 );
			instanceList.push_back( NST_NULL, 40.0 );
			// Cast list
			type = instanceList.get_target_value( fTempVal );
			break;

		case TER_OCEAN:
			fTempVal = noise->Unnormalize(noise_hhf->Get3D( generatedPosition.x*3.2f, generatedPosition.y*3.2f, generatedPosition.z*3.2f ))+0.5f;
			// Create choice list
			instanceList.push_back( NST_NULL, 1.0 );
			instanceList.push_back( NST_RAVINE, 0.8 );
			instanceList.push_back( NST_ORE_VEIN, 2.0 );
			instanceList.push_back( NST_NULL, 57.0 );
			// Cast list
			type = instanceList.get_target_value( fTempVal );
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
		switch ( sampledType )
		{
		case TER_THE_EDGE:
			fTempVal = noise->Unnormalize(noise_hhf->Get3D( generatedPosition.x*3.2f, generatedPosition.y*3.2f, generatedPosition.z*3.2f ))+0.5f;
			// Create choice list
			instanceList.push_back( NST_NULL, 1.0 );
			instanceList.push_back( NST_CRYSTAL_SPIKE, 15.0 );
			instanceList.push_back( NST_NULL, 45.0 );
			// Cast list
			type = instanceList.get_target_value( fTempVal );
			break;

		case TER_FLATLANDS:
		case TER_HILLLANDS:
			fTempVal = noise->Unnormalize(noise_hhf->Get3D( generatedPosition.x*3.2f, generatedPosition.y*3.2f, generatedPosition.z*3.2f ))+0.5f;
			// Create choice list
			instanceList.push_back( NST_NULL, 1.0 );
			instanceList.push_back( NST_CRYSTAL_SPIKE, 5.0 );
			instanceList.push_back( NST_ORE_VEIN, 2.0 );
			instanceList.push_back( NST_ORE_SPLOTCH, 5.0 );
			instanceList.push_back( NST_NULL, 55.0 );
			// Cast list
			type = instanceList.get_target_value( fTempVal );
			break;

		case TER_DEFAULT:
			cout << "Error in generation: TERRAIN TYPE NOT SET" << endl;
			break;

		case TER_MOUNTAINS:
		case TER_BADLANDS:
			fTempVal = noise->Unnormalize(noise_hhf->Get3D( generatedPosition.x*3.2f, generatedPosition.y*3.2f, generatedPosition.z*3.2f ))+0.5f;
			// Create choice list
			instanceList.push_back( NST_NULL, 1.0 );
			instanceList.push_back( NST_CRYSTAL_SPIKE, 3.0 );
			instanceList.push_back( NST_ORE_VEIN, 3.0 );
			instanceList.push_back( NST_ORE_SPLOTCH, 5.0 );
			instanceList.push_back( NST_NULL, 50.0 );
			// Cast list
			type = instanceList.get_target_value( fTempVal );
			break;

		default:

			break;
		}
	}

	return type;
}


// Generate the instances now
void CTerraGenDefault::TerraGen_Instances_Generate( CBoob * pBoob, const RangeVector & position )
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
void CTerraGenDefault::TerraGen_Instances_Generate_Work( TerraInstance * terraInst, CBoob * pBoob, const RangeVector & position )
{
	// If ignore, skip
	if ( terraInst->ignore ) {
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
				//terraInst->position.z = fTerrainHeight - 0.1f;
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
	vector<RangeVector>::iterator searchResult = find( terraInst->affectedAreas.begin(), terraInst->affectedAreas.end(), position );

	// If it can be affected
	if ( searchResult != terraInst->affectedAreas.end() )
	{
		// Check if can generate
		if ((( terraInst->downtrace && terraInst->hasdowntraced ))||( !terraInst->downtrace ))
		{
			//cout << "Instance affected sector: " << position.x << "," << position.y << "," << position.z << endl;
			//cout << "Instance position: " << terraInst->position.x << "," << terraInst->position.y << "," << terraInst->position.z  << endl;
			if ( terraInst->mode == 0 )
			{
				// Then for each block
				for ( char i = 0; i < 8; i += 1 )
				{
					for ( char j = 0; j < 8; j += 1 )
					{
						for ( int k = 0; k < 512; k += 1 )
						{
							// Declared static to save init time
							static Vector3d blockPosition;
							static unsigned short block;
							static bool topVis;

							blockPosition.x = position.x + (((i%2) * 16) + ((j%2) * 8) + (k%8))/32.0f;
							blockPosition.y = position.y + ((((i/2)%2) * 16) + (((j/2)%2) * 8) + ((k/8)%8))/32.0f;
							blockPosition.z = position.z + (((i/4) * 16) + ((j/4) * 8)  + (k/64))/32.0f;

							// Get current block value
							block = pBoob->data[i].data[j].data[k].block;

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
							case NST_PATH_GRAVEL:
								topVis = SideVisible( pBoob, i,j, (terra_t*)pBoob->data[i].data[j].data, k, 8, TOP, 1 );
								block = TerraGen_Instances_Path_Gravel	( terraInst, blockPosition, block );
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
							case NST_VEIN_CRYSTAL:
								block = TerraGen_Instances_Vein_Crystal	( terraInst, blockPosition, block );
								break;
							case NST_VEIN_CLAY:
								block = TerraGen_Instances_Vein_Clay	( terraInst, blockPosition, block );
								break;
							case NST_BOULDER_SMALL:
								block = TerraGen_Instances_Boulder_Small	( terraInst, blockPosition, block );
								break;
							case NST_DEADLANDS_CRATER:
								block = TerraGen_Instances_Deadlands_Crater	( terraInst, blockPosition, block );
								break;
							case NST_ORE_VEIN:
								block = TerraGen_Instances_Ore_Vein	( terraInst, blockPosition, block );
								break;
							case NST_ORE_SPLOTCH:
								block = TerraGen_Instances_Ore_Splotch	( terraInst, blockPosition, block );
								break;
							case NST_ORE_SPLATTER:
								block = TerraGen_Instances_Ore_Splatter	( terraInst, blockPosition, block );
								break;
							case NST_CLEARING:
								block = TerraGen_Instances_Clearing ( terraInst, blockPosition, block );
								break;
							default:
								cout << "Invalid TerraInstance generated." << endl;
								break;
							}

							// Set new block type
							pBoob->data[i].data[j].data[k].block = block;
						}
					}
				}
			}
			else if ( terraInst->mode == 1 ) {
				bool invalid = false;
				// Select the right component to load for the instance
				switch ( terraInst->type )
				{
				case NST_VXG_WATCHTOWER0:
					townManager->VxgToComponent( ".res/terra/instances/guardtower_wood.mcc.VXG" );
					break;
				default:
					invalid = true;
					break;
				}
				if ( !invalid )
				{
					// Paste the instance in at the position.
					SB_PasteVXG( pBoob, position, terraInst->position,
						RangeVector(townManager->component_plan_x,townManager->component_plan_y,townManager->component_plan_z),
						townManager->component_plan_buffer );
					// And that's it!
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
bool CTerraGenDefault::TerraGen_Instances_Downtrace ( TerraInstance * terraInst, CBoob * pBoob, const RangeVector & position )
{
	// First navigate to the wanted boob.
	RangeVector currentPosition = position;
	CBoob* currentBoob = pBoob;

	// If we can't get to the wanted boob, we just quit

	BlockInfo currentBlock;
	if ( terrain->GetBlockInfoAtPosition( terraInst->position, currentBlock ) )
	{
		// Now once we're here, we get the current block that we're at
		while (( currentBlock.block.block == EB_NONE )||( currentBlock.block.block == EB_WATER ))
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
bool CTerraGenDefault::TerraGen_Instances_Downtrace_Fallback ( TerraInstance * terraInst, CBoob * pBoob, const RangeVector & position )
{
	// First navigate to the wanted boob.
	RangeVector currentPosition = position;
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
	while (( currentBlock.block.block == EB_NONE )||( currentBlock.block.block == EB_WATER ))
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
bool CTerraGenDefault::TerraGen_Instances_Uptrace_Fallback ( TerraInstance * terraInst, CBoob * pBoob, const RangeVector & position )
{
	// First navigate to the wanted boob.
	RangeVector currentPosition = position;
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
	while (( currentBlock.block.block != EB_NONE )&&( currentBlock.block.block != EB_WATER ))
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
ushort CTerraGenDefault::TerraGen_Instances_RuinSpike ( TerraInstance * terraInst, const Vector3d & blockPosition, ushort block )
{
	Vector2d difVect = Vector2d( terraInst->position.x - blockPosition.x, terraInst->position.y - blockPosition.y );
	if ( difVect.magnitude() < 1.7f/32 )
	{
		ftype temp;
		temp = noise_hf->Get( blockPosition.x*9.23f, blockPosition.y*9.23f )*1.2f+0.43f;
		
		if ( temp < 0 )
			temp = 0;

		if ( abs( (terraInst->position.z-0.3f) - blockPosition.z ) < 0.63f - temp )
		{
			block = EB_STONEBRICK;
		}
	}
	return block;
}
ushort CTerraGenDefault::TerraGen_Instances_RuinPillar ( TerraInstance * terraInst, const Vector3d & blockPosition, ushort block )
{
	/*Vector2d difVect = Vector2d( terraInst->position.x - blockPosition.x, terraInst->position.y - blockPosition.y );
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
	return block;*/
	//return TerraGen_Instances_RuinSpike( terraInst, blockPosition, block );
	Vector2d difVect = Vector2d( terraInst->position.x - blockPosition.x, terraInst->position.y - blockPosition.y );
	if ( difVect.magnitude() < 2.6f/32 )
	{
		ftype temp;
		temp = noise_hf->Get( blockPosition.x*9.23f, blockPosition.y*9.23f )*1.2f+0.43f;
		
		if ( temp < 0 )
			temp = 0;

		if ( abs( (terraInst->position.z-0.2f) - blockPosition.z ) < 0.73f - temp )
		{
			block = EB_STONEBRICK;
		}
	}
	return block;
}
ushort CTerraGenDefault::TerraGen_Instances_Ravine ( TerraInstance * terraInst, const Vector3d & blockPosition, ushort block )
{
	Vector2d difVect = Vector2d( terraInst->position.x - blockPosition.x, terraInst->position.y - blockPosition.y );

	ftype angle = noise_hf->Get3D( terraInst->position.x + blockPosition.x*0.01f, terraInst->position.y + blockPosition.y*0.01f, terraInst->position.z + blockPosition.z*0.01f ) * 6.283f;
	Vector2d dirVect = Vector2d( sin( angle ), cos( angle ) );

	Vector2d distVect = (difVect - dirVect*difVect.dot( dirVect ));

	ftype width = -(difVect.magnitude() * 0.1f)
		+ noise_hf->Get3D( blockPosition.x, blockPosition.y, blockPosition.z )*0.02f
		+ noise->Get3D( blockPosition.x, blockPosition.y, blockPosition.z*2.0f )*0.03f
		+ 1.2f/32 
		+ 0.04f/(sqr(difVect.x)+sqr(difVect.y))
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
ushort CTerraGenDefault::TerraGen_Instances_Path_Gravel	( TerraInstance * terraInst, const Vector3d & blockPosition, ushort block )
{
	if (( block != EB_NONE )&&( block != EB_WATER ))
	{
		/*if (( cLastCheckedBlock == EB_NONE )||( cLastCheckedBlock == EB_WATER ))
		{
			Vector2d difVect = Vector2d( terraInst->position.x - blockPosition.x, terraInst->position.y - blockPosition.y );

			ftype angle = noise_hf->Get3D( terraInst->position.x + blockPosition.x*0.01f, terraInst->position.y + blockPosition.y*0.01f, terraInst->position.z + blockPosition.z*0.01f ) * 6.283f;
			Vector2d dirVect = Vector2d( sin( angle ), cos( angle ) );

			Vector2d distVect = (difVect - dirVect*difVect.dot( dirVect ));

			ftype width = -(difVect.magnitude() * 0.05f)
				+ noise_hf->Get3D( blockPosition.x, blockPosition.y, blockPosition.z )*0.02f
				+ noise->Get3D( blockPosition.x, blockPosition.y, blockPosition.z*2.0f )*0.03f
				+ 2.1f/32 
				+ 0.005f/(sqr(difVect.x)+sqr(difVect.y))
				+ sqr(blockPosition.z-terraInst->position.z+2.0f)*0.01f;
			if ( distVect.magnitude() < width )
			{
				ftype temp;
				temp = noise->Get( blockPosition.x, blockPosition.y );
				if ( blockPosition.z > terraInst->position.z+temp-1.4f )
				{
					block = EB_GRAVEL;
				}
			}
		}*/
		ftype elevation = TerraGen_pub_GetElevation( blockPosition );
		Vector3d difVect = Vector3d( terraInst->position.x - blockPosition.x, terraInst->position.y - blockPosition.y, elevation + (0.9f/32.0f) - blockPosition.z  );
		
		ftype angle = noise_hf->Get3D( terraInst->position.x + blockPosition.x*0.01f, terraInst->position.y + blockPosition.y*0.01f, terraInst->position.z + blockPosition.z*0.01f ) * 6.283f;
		Vector3d dirVect = Vector3d( sin( angle ), cos( angle ), 0 );

		Vector3d distVect = ( difVect - dirVect*difVect.dot( dirVect ) );
		distVect.x *= 0.7f;
		distVect.y *= 0.7f;

		ftype width = -(difVect.magnitude() * 0.02f )
			+ noise_hf->Get3D( blockPosition.x*0.2f, blockPosition.y*0.2f, blockPosition.z*0.2f )*0.02f
			+ noise->Get3D( blockPosition.x, blockPosition.y, blockPosition.z*2.0f )*0.03f
			+ 2.2f/32;

		if ( distVect.magnitude() < width )
		{
			if (/**/( distVect.z > 1.1f/32.0f ) && (( cLastCheckedBlock == EB_NONE )||( cLastCheckedBlock == EB_WATER ))/**/) {
				block = EB_GRAVEL;
			}
			else {
				block = EB_NONE;
			}
		}
		else if (( distVect.z > 1.1f/32.0f )&&( distVect.magnitude() < width * 1.2f ))
		{
			block = EB_GRAVEL;
		}
	}
	return block;
}
ushort CTerraGenDefault::TerraGen_Instances_Desert_Rocks0	( TerraInstance * terraInst, const Vector3d & blockPosition, ushort block )
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
ushort CTerraGenDefault::TerraGen_Instances_Desert_RockWtr	( TerraInstance * terraInst, const Vector3d & blockPosition, ushort block )
{
	return TerraGen_Instances_Desert_Rocks0( terraInst, blockPosition, block );
}
ushort CTerraGenDefault::TerraGen_Instances_Desert_Oasis	( TerraInstance * terraInst, const Vector3d & blockPosition, ushort block )
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
				//block = EB_WATER;
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

ushort CTerraGenDefault::TerraGen_Instances_Crystal_Spike	( TerraInstance * terraInst, const Vector3d & blockPosition, ushort block )
{
	/*Vector2d difVect = Vector2d( terraInst->position.x - blockPosition.x, terraInst->position.y - blockPosition.y );
	if ( ( difVect.magnitude() < 1.7f/32 )
		&&( abs( terraInst->position.z - blockPosition.z ) < 0.42f ))
	{
		ftype temp;
		temp = noise_hf->Get( blockPosition.x*2.23f, blockPosition.y*2.23f )*0.42f+0.06f;
		if ( temp < 0 )
			temp = 0;

		if (( blockPosition.z < terraInst->position.z + temp )||( blockPosition.z > terraInst->position.z - temp*0.1f ))
		{
			block = EB_CRYSTAL;
		}
	}*/
	Vector2d difVect = Vector2d( terraInst->position.x - blockPosition.x, terraInst->position.y - blockPosition.y );
	if ( difVect.magnitude() < 1.7f/32 )
	{
		ftype temp;
		temp = noise_hf->Get( blockPosition.x*9.23f, blockPosition.y*9.23f )*0.42f+0.13f;
		
		if ( temp < 0 )
			temp = 0;

		if ( abs( terraInst->position.z - blockPosition.z ) < 0.43f - temp )
		{
			block = EB_CRYSTAL;
		}
	}
	return block;
}

ushort CTerraGenDefault::TerraGen_Instances_Vein_Crystal	( TerraInstance * terraInst, const Vector3d & blockPosition, ushort block )
{
	Vector2d center ( terraInst->position.x, terraInst->position.y );
	center.x += noise->Get3D( blockPosition.x*3.4f, blockPosition.y*2.4f, blockPosition.z*3.4f )*0.6f;
	center.y += noise->Get3D( blockPosition.y*3.4f, blockPosition.z*3.4f, blockPosition.x*2.4f )*0.6f;
	Vector2d difVect = Vector2d( center.x - blockPosition.x, center.y - blockPosition.y );
	ftype width;
	width = 0.01f
		+ noise->Get3D( blockPosition.z*4.7f, blockPosition.x*4.7f, blockPosition.y*4.7f )*0.4f
		- (terraInst->position.z - blockPosition.z)*0.1f;
	if ( difVect.magnitude() < width )
	{
		if (( block == EB_STONE )||( block == EB_CLAY )) {
			block = EB_CRYSTAL;
		}
		else if (( block == EB_DIRT )&&( difVect.magnitude() < width*0.5f )) {
			block = EB_CRYSTAL;
		}
	}

	return block;
}

ushort CTerraGenDefault::TerraGen_Instances_Vein_Clay	( TerraInstance * terraInst, const Vector3d & blockPosition, ushort block )
{
	Vector2d center ( terraInst->position.x, terraInst->position.y );
	center.x += noise->Get3D( blockPosition.x*3.4f, blockPosition.y*2.4f, blockPosition.z*3.4f )*0.6f;
	center.y += noise->Get3D( blockPosition.y*3.4f, blockPosition.z*3.4f, blockPosition.x*2.4f )*0.6f;
	Vector2d difVect = Vector2d( center.x - blockPosition.x, center.y - blockPosition.y );
	ftype width;
	width = 0.01f
		+ noise->Get3D( blockPosition.z*4.7f, blockPosition.x*4.7f, blockPosition.y*4.7f )*0.4f
		- (terraInst->position.z - blockPosition.z)*0.1f;
	if ( difVect.magnitude() < width )
	{
		if (( block == EB_STONE )||( block == EB_SANDSTONE )) {
			block = EB_CLAY;
		}
		else if (( block == EB_SAND )&&( difVect.magnitude() < width*0.5f )) {
			block = EB_CLAY;
		}
	}

	return block;
}
ushort CTerraGenDefault::TerraGen_Instances_Ore_Vein	( TerraInstance * terraInst, const Vector3d & blockPosition, ushort block )
{
	Vector2d center ( terraInst->position.x, terraInst->position.y );
	center.x += noise->Get3D( blockPosition.x*3.4f, blockPosition.y*2.4f, blockPosition.z*3.4f )*0.6f;
	center.y += noise->Get3D( blockPosition.y*3.4f, blockPosition.z*3.4f, blockPosition.x*2.4f )*0.6f;
	Vector2d difVect = Vector2d( center.x - blockPosition.x, center.y - blockPosition.y );
	ftype width;
	width = 0.01f
		+ noise->Get3D( blockPosition.z*4.7f, blockPosition.x*4.7f, blockPosition.y*4.7f )*0.4f
		- (terraInst->position.z - blockPosition.z)*0.1f;
	if ( difVect.magnitude() < width )
	{
		if (( block == EB_STONE )||( block == EB_SANDSTONE )||( block == EB_CLAY )) {
			block = EB_SYS_ORE_SPAWN;
		}
		else if (( block == EB_SAND || block == EB_DIRT )&&( difVect.magnitude() < width*0.5f )) {
			block = EB_SYS_ORE_SPAWN;
		}
	}

	return block;
}
ushort CTerraGenDefault::TerraGen_Instances_Ore_Splotch	( TerraInstance * terraInst, const Vector3d & blockPosition, ushort block )
{
	return TerraGen_Instances_Ore_Vein( terraInst, blockPosition, block );
}
ushort CTerraGenDefault::TerraGen_Instances_Ore_Splatter	( TerraInstance * terraInst, const Vector3d & blockPosition, ushort block )
{
	Vector2d center ( terraInst->position.x, terraInst->position.y );
	center.x += noise->Get3D( blockPosition.x*3.4f, blockPosition.y*2.4f, blockPosition.z*3.4f )*0.6f;
	center.y += noise->Get3D( blockPosition.y*3.4f, blockPosition.z*3.4f, blockPosition.x*2.4f )*0.6f;
	Vector2d difVect = Vector2d( center.x - blockPosition.x, center.y - blockPosition.y );
	ftype probability;
	probability = noise_hhf->Get3D( blockPosition.z*4.7f, blockPosition.x*4.7f, blockPosition.y*4.7f ) - fabs(terraInst->position.z - blockPosition.z)*0.1f;
	if ( difVect.magnitude() < probability-0.3f )
	{
		if ( block != EB_NONE && block != EB_WATER && block != EB_WATERSPRING ) {
			block = EB_SYS_ORE_SPAWN;
		}
	}
	
	return block;
}

ushort CTerraGenDefault::TerraGen_Instances_Boulder_Small	( TerraInstance * terraInst, const Vector3d & blockPosition, ushort block )
{
	Vector3d difVect = terraInst->position - blockPosition - Vector3d( 0,0,0.15f );
	ftype width = 0.13f
		+ noise_hf->Get3D( blockPosition.z*1.3f, blockPosition.x*1.3f, blockPosition.y*1.3f )*0.13f;
	if ( difVect.sqrMagnitude() < sqr(width) ) {
		block = EB_STONE;
	}
	return block;
}

ushort CTerraGenDefault::TerraGen_Instances_Deadlands_Crater	( TerraInstance * terraInst, const Vector3d & blockPosition, ushort block )
{
	static ftype val;

	// Judge distance from center
	ftype centerDistance = Vector3d( terraInst->position.x - blockPosition.x, terraInst->position.y - blockPosition.y, 0 ).magnitude();
	// Based on distance to center, do different things to the terrain
	if ( centerDistance < 2.0f ) {
		// Slowly sink downwards
		fCurrentElevation = TerraGen_1p_GetElevation( blockPosition ); // Sets iCurrentTerraType

		if ( blockPosition.z < fCurrentElevation-(2.0f-centerDistance)*0.4f ) {
			return EB_CURSED_DEADSTONE;
		}
		else {
			return EB_NONE;
		}
	}
	else if ( centerDistance < 4.0f ) {
		// Flatten to the normal elevation
		fCurrentElevation = TerraGen_1p_GetElevation( blockPosition ); // Sets iCurrentTerraType

		val = noise->Get3D(blockPosition.x*0.7f,blockPosition.y*0.7f,blockPosition.z);
		//if ( blockPosition.z < fCurrentElevation ) {
		if ( val*(centerDistance-2.0f)*1.5f > (blockPosition.z-fCurrentElevation) ) {
			return EB_DEADSTONE;
		}
		else {
			return EB_NONE;
		}
	}
	else if ( centerDistance < 4.6f ) {
		// Change blocks into deadlands blocks

		val = noise->Get3D(blockPosition.x*0.7f,blockPosition.y*0.7f,blockPosition.z);

		if ( blockPosition.z < fCurrentElevation+(fabs(4.3f-centerDistance)*(val+0.2f)*5.0f) ) {
			return EB_DEADSTONE;
		}
		else if ( block == EB_DIRT || block == EB_GRASS || block == EB_MUD || block == EB_SAND ) {
			return EB_ASH;
		}
		else if ( block == EB_STONE ) {
			return EB_DEADSTONE;
		}
	}
	return block;
}

ushort CTerraGenDefault::TerraGen_Instances_Clearing		( TerraInstance * terraInst, const Vector3d & blockPosition, ushort block )
{
	ftype val = noise->Get3D(blockPosition.x*0.7f,blockPosition.y*0.7f,blockPosition.z);

	// Judge distance from center
	ftype centerDistance = Vector3d( terraInst->position.x - blockPosition.x, terraInst->position.y - blockPosition.y, 0 ).magnitude();

	// Lerp between elevations (should be clearing about 0.7 wide)
	//ftype centerElevation = TerraGen_1p_GetElevation( terraInst->position );
	ftype centerElevation = terraInst->position.z;
	ftype targetElevation = TerraGen_1p_GetElevation( blockPosition );

	ftype elevation = Math.Lerp( (centerDistance-0.6f) * 2.0f , centerElevation, targetElevation );

	val -= centerDistance - 0.5f;

	if ( val > 0 ) {
		if ( blockPosition.z > elevation ) {
			return EB_NONE;
		}
	}

	return block;
}