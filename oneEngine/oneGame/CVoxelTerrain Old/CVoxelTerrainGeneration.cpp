// Includes
#include "CVoxelTerrain.h"
#include "CFoliageTree.h"

// Include Foliage Types
#include "CTreeBase.h"
#include "FoliageBushGreen.h"
#include "sTerraGrass.h"

//==Static variable declares==
/*
Perlin					CVoxelTerrain::noise		= Perlin( 4,4,1,0 );
*/
#ifdef _DEAD_
// Grab samplers from file
void CVoxelTerrain::TerraGenGetSamplers ( void ) // should this be moved to the IO file?
{
	if ( !bHasSamplers )
	{
		bHasSamplers = true;

		if ( pSamplerTerrainType == NULL )
		{
			pSamplerTerrainType = new unsigned char [64*64];
			// Read in the terrain sampler
			{
				string sInputFile ( ".res\\system\\terrainSampler.pgm" );
				ifstream fileInput ( sInputFile.c_str(), iostream::binary );
				if ( fileInput.is_open() )
				{
					string sTemp;
					int iTemp;
					fileInput >> sTemp;
					fileInput >> iTemp;
					fileInput >> iTemp;
					fileInput >> iTemp;
					fileInput.get();
					fileInput.read( (char*)pSamplerTerrainType, 64*64 );
					fileInput.close();
				}
				else // Print error if failed
				{
					cout << "WARNING: Could not read terrain sampler! Expect terrain generation to be FUCKED." << endl;
				}
			}
		}

		if ( pSamplerBiomeType == NULL )
		{
			pSamplerBiomeType = new unsigned char [64*64];
			{
				// Read in the biome sampler
				string sInputFile ( ".res\\system\\biomeSampler.pgm" );
				ifstream fileInput ( sInputFile.c_str(), iostream::binary );
				if ( fileInput.is_open() )
				{
					string sTemp;
					int iTemp;
					fileInput >> sTemp;
					fileInput >> iTemp;
					fileInput >> iTemp;
					fileInput >> iTemp;
					fileInput.get();
					fileInput.read( (char*)pSamplerBiomeType, 64*64 );
					fileInput.close();
				}
				else // Print error if failed
				{
					cout << "WARNING: Could not read biome sampler! Expect terrain generation to be STRANGE." << endl;
				}
			}
		}
	}
}

// Begin to generate the terrain
void CVoxelTerrain::GenerateTerrain ( CBoob * pBoob, LongIntPosition const& position )
{
	if ( noise == NULL )
	{
		noise		= new Perlin( 1,2,1,iTerrainSeed );
		noise_hf	= new Perlin( 1,5,1,iTerrainSeed );
		noise_hhf	= new Perlin( 1,15,1,iTerrainSeed+1 );
		noise_lf	= new Perlin( 1,0.5f,1,iTerrainSeed-1 );
		noise_terra	= new Perlin( 2,0.03f,1,iTerrainSeed+1 );
		noise_biome	= new Perlin( 2,0.04f,1,iTerrainSeed-1 );
	}

	TerraGenGetSamplers();
	// Primary main terrain pass
	DebugConsole::PrintMessage( "   Primary pass (terrain) subdef_dungeon+negativedungeon...\n" );
	GenerateTerrainSector( pBoob, position );
	// Grass and various object generation pass
	DebugConsole::PrintMessage( "   Second pass (features) subdef_instances...\n" );
	GenerateTerrainSectorSecondPass( pBoob, position );
	// Grass and various object generation pass
	DebugConsole::PrintMessage( "   Final pass (biomes, grass and shiz) subdef_biome...\n" );
	GenerateTerrainSectorThirdPass( pBoob, position );
	// Clean up our mess of variables
	DebugConsole::PrintMessage( "   Cleanup pass (lol bad vars) subdef_cleanuplol...\n" );
	GenerateTerrainSectorCleanup( pBoob );

	/*{
		delete noise;
		delete noise_hf;
		delete noise_hhf;
		delete noise_lf;
		delete noise_biome;
		delete noise_terra;
	}*/
}
// Same as previous functions, just without the large comments
void CVoxelTerrain::GenerateSingleTerrain ( CBoob * pBoob, LongIntPosition const& position )
{
	bSpreadTerrain = false;
	//DebugConsole::PrintMessage( " Generating sector..." );
	pBoob->bitinfo[0] = false;
	pBoob->bitinfo[3] = false;
	pBoob->bitinfo[7] = false;
	DebugConsole::DisableOutput();
	GenerateTerrain( pBoob, position );
	DebugConsole::EnableOutput();
	//DebugConsole::PrintMessage( "Done.\n" );
	bSpreadTerrain = true;
}

// == First Pass == 
void CVoxelTerrain::GenerateTerrainSector ( CBoob * pBoob, LongIntPosition const& position )
{
	if (( pBoob != NULL )&&( pBoob->bitinfo[3] == false ))
	{
		pBoob->bitinfo[3] = true;	// Set terrain as already generated

		// Set position
		pBoob->position = Vector3d( ftype(position.x*64+32),ftype(position.y*64+32),ftype(position.z*64+32) );
		pBoob->li_position = position;

		// Spread terrain generation. TODO: FIX THIS SOMEDAY
		if ( bSpreadTerrain )
		{
			if ( position.z >= 0 )
				GenerateTerrainSector( pBoob->top, LongIntPosition( position.x, position.y, position.z+1 ) );
			if ( position.x >= 0 )
				GenerateTerrainSector( pBoob->front, LongIntPosition( position.x+1, position.y, position.z ) );
			if ( position.y >= 0 )
				GenerateTerrainSector( pBoob->left, LongIntPosition( position.x, position.y+1, position.z ) );
			if ( position.z <= 0 )
				GenerateTerrainSector( pBoob->bottom, LongIntPosition( position.x, position.y, position.z-1 ) );
			if ( position.x <= 0 )
				GenerateTerrainSector( pBoob->back, LongIntPosition( position.x-1, position.y, position.z ) );
			if ( position.y <= 0 )
				GenerateTerrainSector( pBoob->right, LongIntPosition( position.x, position.y-1, position.z ) );
		}
		// Generate the terrain
		TerraGenLandscapePass( pBoob, position );
		// Put stone in the dirt
		TerraGenStonePass( pBoob, position );
		// Put dirt in the stone
		// Make caves
		TerraGenCavePass( pBoob, position );
		// Make bridges
		TerraGenBridgePass( pBoob, position );
	}
}

void CVoxelTerrain::TerraGenLandscapePass ( CBoob * pBoob, LongIntPosition const& position )
{
	// Declare value of terrain
	unsigned short iTerrainWeight [256];
	for ( short i = 0; i < 256; ++i )
		iTerrainWeight[i] = 0;

	// Eventually, this will become a sort of 8x sample + blend between several different generation algorithms.
	// The current algorithm is the basic test terrain algorithm. Other variations include say flatlands, mountains, and other such things.
	// Create the basic terrain
	for ( char i = 0; i < 8; i += 1 )
	{
		for ( char j = 0; j < 8; j += 1 )
		{
			for ( short k = 0; k < 512; k += 1 )
			{
				// Declared static to save init time
				static Vector3d blockPosition;

				// Get the block's noise position
				blockPosition.x = position.x + (((i%2) * 16) + ((j%2) * 8) + (k%8))/32.0f;
				blockPosition.y = position.y + ((((i/2)%2) * 16) + (((j/2)%2) * 8) + ((k/8)%8))/32.0f;
				blockPosition.z = position.z + (((i/4) * 16) + ((j/4) * 8)  + (k/64))/32.0f;

				// Generate terrain type
				static ftype fTempOffs;
				static ftype fRockiness;
				static ftype fUnpredictability;
				//static ftype fCurrentElevation;
				static ftype fCurrentSamplerZ;
				static unsigned char cResultTerrain;
				static unsigned char cResultBlock;
				// Get elevation
				//fCurrentElevation = TerraGen_1p_GetElevation( position );
				fCurrentElevation = TerraGen_1p_GetElevation( blockPosition );
				// Get Z sampler
				fCurrentSamplerZ = max( min( fCurrentElevation, blockPosition.z ), fCurrentElevation-0.5f );
				// Input functions for rockiness and unpredictability
				fRockiness			= noise_terra->Get3D(blockPosition.x*1.6f,blockPosition.y*1.6f,fCurrentSamplerZ) + 0.5f;
				fUnpredictability	= noise_terra->Get3D(blockPosition.y,blockPosition.x,fCurrentSamplerZ) + 0.5f;
				// Add another layer of noise to the sample values
				fTempOffs = noise->Get3D( blockPosition.x*1.6f,blockPosition.y*1.6f,blockPosition.z*1.6f )*0.05f;
				fRockiness			= max( 0.0f, min( 1.0f, fRockiness+fTempOffs ) );
				fTempOffs = noise->Get3D( blockPosition.y*1.3f,blockPosition.z*1.6f,blockPosition.x*1.6f )*0.05f;
				fUnpredictability	= max( 0.0f, min( 1.0f, fUnpredictability+fTempOffs ) );
				// Get the terrain type
				cResultTerrain = pSamplerTerrainType[ (int(fUnpredictability*63.8f)) + (int(fRockiness*63.8f)*64) ];

				// Weight this terrain type
				iTerrainWeight[cResultTerrain] += 1;

				// Get the block data
				switch ( cResultTerrain )
				{
				case TER_SPIRES:
					cResultBlock = TerraGen_1p_Spires( pBoob, blockPosition );
					break;
				case TER_OCEAN:
					cResultBlock = TerraGen_1p_Ocean( pBoob, blockPosition );
					break;
				case TER_ISLANDS:
					cResultBlock = TerraGen_1p_Islands( pBoob, blockPosition );
					break;
				case TER_FLATLANDS:
					cResultBlock = TerraGen_1p_Flatlands( pBoob, blockPosition );
					break;
				case TER_HILLLANDS:
					cResultBlock = TerraGen_1p_Hilllands( pBoob, blockPosition );
					break;
				case TER_MOUNTAINS:
					cResultBlock = TerraGen_1p_Mountains( pBoob, blockPosition );
					break;
				case TER_DESERT:
					cResultBlock = TerraGen_1p_Desert( pBoob, blockPosition );
					break;
				case TER_BADLANDS:
					cResultBlock = TerraGen_1p_Badlands( pBoob, blockPosition );
					break;
				case TER_OUTLANDS:
					cResultBlock = TerraGen_1p_Outlands( pBoob, blockPosition );
					break;
				case TER_THE_EDGE:
					cResultBlock = EB_NONE;
					break;
				default:
					//cResultBlock = TerraGen_1p_Default( pBoob, blockPosition );
					cResultBlock = EB_NONE;
					break;
				}

				pBoob->data[i].data[j].data[k] = cResultBlock;
			}
		}
	}

	// Now, we set the terrain's terrain type based on the terrain that had the most effect
	// The weights are computed based on the terrain type of each block.
	unsigned char cFinalTerrain = 0;
	for ( short i = 1; i < 256; ++i )
	{
		if ( iTerrainWeight[i] > iTerrainWeight[cFinalTerrain] )
		{
			cFinalTerrain = unsigned char(i);
		}
	}
	pBoob->terrain = cFinalTerrain;
}

char	CVoxelTerrain::TerraGen_priv_GetType	( Vector3d const& blockPosition )
{
	TerraGenGetSamplers();
	if ( noise == NULL )
	{
		noise		= new Perlin( 1,2,1,iTerrainSeed );
		noise_hf	= new Perlin( 1,5,1,iTerrainSeed );
		noise_hhf	= new Perlin( 1,15,1,iTerrainSeed+1 );
		noise_lf	= new Perlin( 1,0.5f,1,iTerrainSeed-1 );
		noise_terra	= new Perlin( 2,0.03f,1,iTerrainSeed+1 );
		noise_biome	= new Perlin( 2,0.04f,1,iTerrainSeed-1 );
	}

	// Generate terrain type
	static ftype fTempOffs;
	static ftype fRockiness;
	static ftype fUnpredictability;
	//static ftype fCurrentElevation;
	static ftype fCurrentSamplerZ;
	static unsigned char cResultTerrain;
	static unsigned char cResultBlock;
	// Get elevation
	fCurrentElevation = TerraGen_1p_GetElevation( blockPosition );
	// Get Z sampler
	fCurrentSamplerZ = max( min( fCurrentElevation, blockPosition.z ), fCurrentElevation-0.5f );
	// Input functions for rockiness and unpredictability
	fRockiness			= noise_terra->Get3D(blockPosition.x*1.6f,blockPosition.y*1.6f,fCurrentSamplerZ) + 0.5f;
	fUnpredictability	= noise_terra->Get3D(blockPosition.y,blockPosition.x,fCurrentSamplerZ) + 0.5f;
	// Add another layer of noise to the sample values
	fTempOffs = noise->Get3D( blockPosition.x*1.6f,blockPosition.y*1.6f,blockPosition.z*1.6f )*0.05f;
	fRockiness			= max( 0.0f, min( 1.0f, fRockiness+fTempOffs ) );
	fTempOffs = noise->Get3D( blockPosition.y*1.3f,blockPosition.z*1.6f,blockPosition.x*1.6f )*0.05f;
	fUnpredictability	= max( 0.0f, min( 1.0f, fUnpredictability+fTempOffs ) );
	// Get the terrain type
	cResultTerrain = pSamplerTerrainType[ (int(fUnpredictability*63.8f)) + (int(fRockiness*63.8f)*64) ];

	return cResultTerrain;
}
ftype CVoxelTerrain::TerraGen_1p_GetElevation ( LongIntPosition const& position )
{
	static ftype val;
	static ftype val3;
	//val = 0.02f + noise_biome->Get3D(ftype(position.x)*1.6f,ftype(position.y)*1.6f,0.0f);
	//return 0.02f;
	val = 0.02f + (noise_terra->Get3D(ftype(position.x)*1.6f,ftype(position.y)*1.6f,0.0f)+0.18f)*1.8f;
	
	// second-level noise
	val3 = noise_terra->Get3D(-ftype(position.y)*1.1f,-ftype(position.x)*1.1f,0.0f);
	val += sqr(val3)*2.0f;

	if ( val < 0.0f )
		val = -sqr(val);
	else if ( val > 1.0f )
		val = sqr(val);
	return val;
}
ftype CVoxelTerrain::TerraGen_1p_GetElevation ( Vector3d const& blockPosition )
{
	static ftype val;
	static ftype val2;
	static ftype val3;
	//val = 0.02f + (noise_biome->Get3D(blockPosition.x*1.6f,blockPosition.y*1.6f,0.0f)+0.7f-0.5f)*0.4f;
	//return 0.02f;
	//val = 0.02f + (noise_biome->Get3D(blockPosition.x*1.6f,blockPosition.y*1.6f,0.0f)+0.7f-0.5f)*1.8f;
	val = 0.02f + (noise_terra->Get3D(blockPosition.x*1.6f,blockPosition.y*1.6f,0.0f)+0.18f)*1.8f;

	// second-level noise
	val3 = noise_terra->Get3D(-blockPosition.y*1.1f,-blockPosition.x*1.1f,0.0f);
	val += sqr(val3)*2.0f;

	// desert-ocean border fix
	val2 = noise_terra->Get3D(blockPosition.y,blockPosition.x,0.0f) + 0.5f;
	val2 = max( 0, min( 1, 1-abs((val2-0.75f)/0.06f) ) ) * max( 0, -val*1.1f );
	val += val2;

	if ( val < 0.0f )
		val = -sqr(val*2.1f);
	else if ( val > 1.0f )
		val = sqr(val);
	return val;
}

// Terrain types
char CVoxelTerrain::TerraGen_1p_Default ( CBoob * pBoob, Vector3d const& blockPosition )
{
	static ftype val;
	val = noise->Get3D(blockPosition.x,blockPosition.y,blockPosition.z);
	if ( val > blockPosition.z*1.02f ) //5-6 grass lands, 0.2 outlands
		return EB_DIRT;
	else
		return EB_NONE;
}
char CVoxelTerrain::TerraGen_1p_Flatlands ( CBoob * pBoob, Vector3d const& blockPosition )
{
	static ftype val;
	val = noise->Get3D(blockPosition.x,blockPosition.y,blockPosition.z);
	if ( val > (blockPosition.z-fCurrentElevation)*4.8f ) //5-6 grass lands, 0.2 outlands
		return EB_DIRT;
	else
		return EB_NONE;
}
char CVoxelTerrain::TerraGen_1p_Outlands ( CBoob * pBoob, Vector3d const& blockPosition )
{
	static ftype val;
	val = noise->Get3D(blockPosition.x,blockPosition.y,blockPosition.z);
	if ( val > (blockPosition.z-fCurrentElevation)*0.2f ) //5-6 grass lands, 0.2 outlands
		return EB_DIRT;
	else
		return EB_NONE;
}
char CVoxelTerrain::TerraGen_1p_Hilllands ( CBoob * pBoob, Vector3d const& blockPosition )
{
	static ftype val;
	val = noise->Get3D(blockPosition.x*0.7f,blockPosition.y*0.7f,blockPosition.z);
	if ( val > (blockPosition.z-fCurrentElevation)*1.04f ) //5-6 grass lands, 0.2 outlands
		return EB_DIRT;
	else
		return EB_NONE;
}
char CVoxelTerrain::TerraGen_1p_Mountains ( CBoob * pBoob, Vector3d const& blockPosition )
{
	static ftype val;
	static ftype val2;
	static ftype val3;
	val = noise->Get3D(blockPosition.x*0.7f,blockPosition.y*0.7f,blockPosition.z);
	val2 = (noise_hf->Get(blockPosition.x*0.13f,blockPosition.y*0.13f)+0.5f)*2.0f;
	val3 = noise->Get(blockPosition.x*0.03f,blockPosition.y*0.03f)+0.4f;
	val3 = cub( val3 )*2.0f;
	if ( val > (blockPosition.z-fCurrentElevation)*1.04f + 0.1f - val2 - val3 ) //5-6 grass lands, 0.2 outlands
		return EB_DIRT;
	else
		return EB_NONE;
}
char CVoxelTerrain::TerraGen_1p_Islands ( CBoob * pBoob, Vector3d const& blockPosition )
{
	static ftype val;
	val = noise->Get3D(blockPosition.x*0.4f,blockPosition.y*0.4f,blockPosition.z);
	static ftype cmpVal;
	static ftype zVal;
	zVal = (blockPosition.z-fCurrentElevation);
	cmpVal = ( zVal*0.9f + 0.04f );
	if ( val > cmpVal ) //5-6 grass lands, 0.2 outlands
	{
		if ( fabs( val-cmpVal ) < -0.12f*(zVal-0.12f) )
			return EB_SAND;
		else
			return EB_DIRT;
	}
	else
	{
		if ( blockPosition.z >= -0.0001f )
			return EB_NONE;
		else
			return EB_WATER;
			//return EB_NONE;
			//return EB_WATER;
	}
}
char CVoxelTerrain::TerraGen_1p_Ocean ( CBoob * pBoob, Vector3d const& blockPosition )
{
	static ftype val;
	static ftype val2;
	//val = 2.0f - sqrt( sin( noise->Get3D(blockPosition.y*0.6f,blockPosition.x*0.6f,blockPosition.z) * ftype(PI) ) + 1.0f );
	val = sin( noise->Get3D(blockPosition.y*0.65f,blockPosition.x*0.65f,blockPosition.z) * ftype(PI) );
	val2 = (noise_hf->Get(blockPosition.x*0.14f,blockPosition.y*0.14f)+0.5f)*1.2f;
	if ( val > ((blockPosition.z-fCurrentElevation)*4.6f) + 0.2f + val2 )
	{
		return EB_SAND;
	}
	else
	{
		if ( blockPosition.z >= -0.0001f )
			return EB_NONE;
		else
			return EB_WATER;
	}
}
char CVoxelTerrain::TerraGen_1p_Desert ( CBoob * pBoob, Vector3d const& blockPosition )
{
	static ftype val;
	static ftype val2;
	val = 2.0f - sqrt( sin( noise->Get3D(blockPosition.y*0.6f,blockPosition.x*0.6f,blockPosition.z) * ftype(PI) ) + 1.0f );
	//if ( val > (blockPosition.z-fCurrentElevation)*5.4f*1.2f - 0.2f )
	if ( val > (blockPosition.z-fCurrentElevation)*5.4f*1.2f + 0.3f )
	{
		val2 = noise->Get3D(blockPosition.z*1.4f,blockPosition.y*1.4f,blockPosition.x*1.4f);

		if ( val2 > 0.45f )
			return EB_DIRT;
		else
			return EB_SAND;
	}
	else
		return EB_NONE;
}
char CVoxelTerrain::TerraGen_1p_Badlands ( CBoob * pBoob, Vector3d const& blockPosition )
{
	static ftype val;
	val = noise_lf->Get3D(blockPosition.x*0.7f,blockPosition.y*0.7f,blockPosition.z*0.13f);
	if ( val > 0.2f )
		val = max( 0.25f, val*0.35f+0.1f );
	else
		val *= 0.15f;


	static ftype val2;
	val2 = noise->Get3D(blockPosition.x*1.2f,blockPosition.y*1.2f,blockPosition.z*0.23f)*2.0f;
	val += cub(val2)*0.7f;

	static ftype val3;
	val3 = noise_lf->Get3D(-blockPosition.y*0.7f,-blockPosition.x*0.7f,blockPosition.z*0.05f)*2.0f;
	val3 = cub( val3 ) * cub( val3 ) * 0.4f;

	if ( val > (blockPosition.z-fCurrentElevation)*0.5f + val3 )
	{
		//return EB_DIRT; //todo: change to red dirt, not just no grass
		val = noise_lf->Get3D( blockPosition.y*0.03f, blockPosition.x*0.03f, blockPosition.z*8.5f );
		if ( val > 0.3 )
			return EB_SAND;
		else if ( val > 0.2 )
			return EB_STONE;
		else if ( val > 0.1 )
			return EB_CLAY;
		else if ( val > -0.1 )
			return EB_DIRT;
		else if ( val > -0.2 )
			return EB_CLAY;
		else if ( val > -0.3 )
			return EB_SAND;
		else
			return EB_DIRT;
	}
	else
		return EB_NONE;
}
char CVoxelTerrain::TerraGen_1p_Spires ( CBoob * pBoob, Vector3d const& blockPosition )
{
	/*static ftype val;
	//val = 2.0f - sqrt( sin( noise->Get3D(blockPosition.y*0.6f,blockPosition.x*0.6f,blockPosition.z) * ftype(PI) ) + 1.0f );
	val = sin( noise->Get3D(blockPosition.y*0.6f,blockPosition.x*0.6f,blockPosition.z) * ftype(PI) );
	if ( val > ((blockPosition.z-fCurrentElevation)*4.4f) + 0.3f )
	{
		return EB_SAND;
	}
	else
	{
		if ( blockPosition.z >= -0.0001f )
			return EB_NONE;
		else
			return EB_WATER;
	}*/
	//return TerraGen_1p_Ocean( pBoob, blockPosition );
	/*static ftype val;
	static ftype val2;
	//val = 2.0f - sqrt( sin( noise->Get3D(blockPosition.y*0.6f,blockPosition.x*0.6f,blockPosition.z) * ftype(PI) ) + 1.0f );
	val = sin( noise->Get3D(blockPosition.y*0.65f,blockPosition.x*0.65f,blockPosition.z) * ftype(PI) );
	val2 = (noise_hf->Get(blockPosition.x*0.14f,blockPosition.y*0.14f)+0.5f)*1.2f;
	if ( val > ((blockPosition.z-fCurrentElevation)*4.6f) + 0.2f + val2 )
	{
		return EB_SAND;
	}
	else
	{
		if ( blockPosition.z >= -0.0001f )
			return EB_NONE;
		else
			return EB_WATER;
	}*/
	//char val = TerraGen_1p_Ocean( pBoob, blockPosition );
	
	// Close and epic looking
	/*
	static ftype val;
	val = noise_hf->Get3D( blockPosition.y*0.75f,blockPosition.x*0.75f,blockPosition.z*0.05f );
	if ( val < 0 )
		val = 0;
	val -= 0.14f;
	if ( val > blockPosition.z*0.07f )
	{
		return EB_DIRT;
	}
	else
	{
		return TerraGen_1p_Ocean( pBoob, blockPosition );
	}
	*/

	static ftype val;
	static ftype val2;
	val2 = noise->Get3D( blockPosition.x*0.43f, blockPosition.y*0.43f, blockPosition.z*0.005f ) * 0.1f;

	val = noise_hf->Get3D( blockPosition.y*0.47f,blockPosition.x*0.47f,blockPosition.z*0.015f );
	if ( val < 0 )
		val = 0;
	val -= 0.14f + val2;
	if ( val > blockPosition.z*0.06f )
	{
		return EB_DIRT;
	}
	else
	{
		return TerraGen_1p_Ocean( pBoob, blockPosition );
	}
}

// After creating terrain, switch to stone
void CVoxelTerrain::TerraGenStonePass ( CBoob * pBoob, LongIntPosition const& position )
{
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

				if (( pBoob->data[i].data[j].data[k] != EB_NONE )&&( pBoob->data[i].data[j].data[k] != EB_WATER ))
				{
					blockPosition.x = position.x + (((i%2) * 16) + ((j%2) * 8) + (k%8))/32.0f;
					blockPosition.y = position.y + ((((i/2)%2) * 16) + (((j/2)%2) * 8) + ((k/8)%8))/32.0f;
					blockPosition.z = position.z + (((i/4) * 16) + ((j/4) * 8)  + (k/64))/32.0f;

					// Get elevation
					fCurrentElevation = TerraGen_1p_GetElevation( blockPosition ) - 0.02f;
					//fCurrentElevation = 0;

					// Get noise
					fMainTer = noise->Get3D( blockPosition.x,blockPosition.y,blockPosition.z );
					fSubTer = noise_hf->Get3D( blockPosition.x,blockPosition.y,blockPosition.z );
					if ( fMainTer > (blockPosition.z-fCurrentElevation)*1.7f+0.24f+fSubTer*0.42f )
						pBoob->data[i].data[j].data[k] = EB_STONE;
				}
			}
		}
	}
}

// == Second Pass ==
void CVoxelTerrain::GenerateTerrainSectorSecondPass ( CBoob * pBoob, LongIntPosition const& position )
{
	if (( pBoob != NULL )&&( pBoob->bitinfo[0] == false ))
	{
		pBoob->bitinfo[0] = true;
		// TODO: FIX THIS SOMEDAY
		if ( bSpreadTerrain )
		{
			if ( position.z >= 0 )
				GenerateTerrainSectorSecondPass( pBoob->top, LongIntPosition( position.x, position.y, position.z+1 ) );
			if ( position.x >= 0 )
				GenerateTerrainSectorSecondPass( pBoob->front, LongIntPosition( position.x+1, position.y, position.z ) );
			if ( position.y >= 0 )
				GenerateTerrainSectorSecondPass( pBoob->left, LongIntPosition( position.x, position.y+1, position.z ) );
			if ( position.z <= 0 )
				GenerateTerrainSectorSecondPass( pBoob->bottom, LongIntPosition( position.x, position.y, position.z-1 ) );
			if ( position.x <= 0 )
				GenerateTerrainSectorSecondPass( pBoob->back, LongIntPosition( position.x-1, position.y, position.z ) );
			if ( position.y <= 0 )
				GenerateTerrainSectorSecondPass( pBoob->right, LongIntPosition( position.x, position.y-1, position.z ) );
		}
		// Create instanced stuff
		TerraGenInstancePass( pBoob, position );
	}
}

// == Third Pass ==
void CVoxelTerrain::GenerateTerrainSectorThirdPass ( CBoob * pBoob, LongIntPosition const& position )
{
	if (( pBoob != NULL )&&( pBoob->bitinfo[7] == false ))
	{
		pBoob->bitinfo[7] = true;
		// TODO: FIX THIS SOMEDAY
		if ( bSpreadTerrain )
		{
			if ( position.z >= 0 )
				GenerateTerrainSectorThirdPass( pBoob->top, LongIntPosition( position.x, position.y, position.z+1 ) );
			if ( position.x >= 0 )
				GenerateTerrainSectorThirdPass( pBoob->front, LongIntPosition( position.x+1, position.y, position.z ) );
			if ( position.y >= 0 )
				GenerateTerrainSectorThirdPass( pBoob->left, LongIntPosition( position.x, position.y+1, position.z ) );
			if ( position.z <= 0 )
				GenerateTerrainSectorThirdPass( pBoob->bottom, LongIntPosition( position.x, position.y, position.z-1 ) );
			if ( position.x <= 0 )
				GenerateTerrainSectorThirdPass( pBoob->back, LongIntPosition( position.x-1, position.y, position.z ) );
			if ( position.y <= 0 )
				GenerateTerrainSectorThirdPass( pBoob->right, LongIntPosition( position.x, position.y-1, position.z ) );
		}
		// Generate the grass, small details, and settle liquids
		//TerraGenGrassPass( pBoob, position );
		//TerraGenTreePass ( pBoob, position );
		TerraGenBiomePass ( pBoob, position );
	}
}

// Generate Biomes.
void CVoxelTerrain::TerraGenBiomePass ( CBoob * pBoob, LongIntPosition const& position )
{
	for ( char i = 0; i < 8; i += 1 )
	{
		for ( char j = 0; j < 8; j += 1 )
		{
			for ( int k = 0; k < 512; k += 1 )
			{
				static Vector3d blockPosition = Vector3d( 0,0,0 );
				static bool topVis;

				// Grab block info and position
				blockPosition.x = position.x + (((i%2) * 16) + ((j%2) * 8) + (k%8))/32.0f;
				blockPosition.y = position.y + ((((i/2)%2) * 16) + (((j/2)%2) * 8) + ((k/8)%8))/32.0f;
				blockPosition.z = position.z + (((i/4) * 16) + ((j/4) * 8)  + (k/64))/32.0f;
				topVis = SideVisible( pBoob, i,j, (char*)pBoob->data[i].data[j].data, k, 8, TOP, 1, blockPosition );
				

				// == Sampling ==
				static unsigned char cResultBiome;
				static unsigned char cResultBlock;
				if (( i%4 == 0 )||( j%4 == 0 )||( k%4 == 0 ))
					cResultBiome = TerraGen_priv_GetBiome( blockPosition );

				// TODO:::
				pBoob->biome = cResultBiome;

				// == Application ==
				// Apply the biome type
				switch ( cResultBiome )
				{
				case BIO_DESERT: // temp for now
				case BIO_GRASS_DESERT: // This is typically the badlands
					cResultBlock = TerraGen_3p_GrassDesert( pBoob, blockPosition, pBoob->data[i].data[j].data[k], topVis );
					break;
				case BIO_GRASSLAND:
				case BIO_SAVANNA:
					cResultBlock = TerraGen_3p_Grassland( pBoob, blockPosition, pBoob->data[i].data[j].data[k], topVis );
					break;
				default: // Default generation
					cResultBlock = TerraGen_3p_Default( pBoob, blockPosition, pBoob->data[i].data[j].data[k], topVis );
					break;
				}


				pBoob->data[i].data[j].data[k] = cResultBlock;
				
			}
		}
	}
}
char	CVoxelTerrain::TerraGen_priv_GetBiome	( Vector3d const& blockPosition )
{
	TerraGenGetSamplers();
	if ( noise == NULL )
	{
		noise		= new Perlin( 1,2,1,iTerrainSeed );
		noise_hf	= new Perlin( 1,5,1,iTerrainSeed );
		noise_hhf	= new Perlin( 1,15,1,iTerrainSeed+1 );
		noise_lf	= new Perlin( 1,0.5f,1,iTerrainSeed-1 );
		noise_terra	= new Perlin( 2,0.03f,1,iTerrainSeed+1 );
		noise_biome	= new Perlin( 2,0.04f,1,iTerrainSeed-1 );
	}

	// == Sampling ==
	// Generate biome type
	static ftype fTemperature;
	static ftype fHumidity;
	static ftype fDensity;

	static ftype fCurrentSamplerZ;

	static ftype tempVal;

	static unsigned char cResultBiome;
	static unsigned char cResultTerrain;
	// Get terrain
	cResultTerrain = TerraGen_priv_GetType( blockPosition );
	// Get elevation
	fCurrentElevation = TerraGen_1p_GetElevation( blockPosition );
	// Get Z sampler
	fCurrentSamplerZ = max( min( fCurrentElevation, blockPosition.z ), fCurrentElevation-0.5f );
	// Input functions for temperature, humidity, and density
	fTemperature	= noise_biome->Get3D(blockPosition.x*1.6f,blockPosition.y*1.6f,fCurrentSamplerZ) + 0.5f;
	fHumidity		= noise_biome->Get3D(blockPosition.y,blockPosition.x,fCurrentSamplerZ) + 0.5f;
	fDensity		= noise_biome->Get3D(-blockPosition.y*1.2f,-blockPosition.x*1.2f,fCurrentSamplerZ ) + 0.5f;
	// Add another layer of noise to the sample values
	fTemperature	+= noise->Get3D( blockPosition.x*1.3f,blockPosition.y*1.3f,blockPosition.z*1.3f )*0.05f;
	fHumidity		+= noise->Get3D( blockPosition.y*1.2f,blockPosition.z*1.2f,blockPosition.x*1.2f )*0.05f;


	// Find the result biome at the position
	//cResultBiome = BIO_DEFAULT;

	// Terrain Based Weights
	switch ( cResultTerrain )
	{
	case TER_BADLANDS:
		// Perform a slight pull to the center of the generation
		fTemperature	+= (0.65f-fTemperature)*0.12f;
		fHumidity		+= (0.5f-fHumidity)*0.1f;
		break;
	case TER_DESERT:
		// Perform a slight pull to the far left area of generation
		fTemperature	+= (0.9f-fTemperature)*0.05f;
		fHumidity		+= (0.0f-fHumidity)*0.1f;
		break;
	case TER_MOUNTAINS:
		// Perform a slight pull to the lower evergreen area
		fTemperature	+= (0.4f-fTemperature)*0.1f;
		break;
	case TER_OCEAN:
	case TER_SPIRES:
	case TER_ISLANDS:
		// Perform a stronger pull to the rainforest area
		fTemperature	+= (1.0f-fTemperature)*0.15f;
		fHumidity		+= (1.1f-fHumidity)*0.15f;
		break;
	}

	// Calculate weights
	// The larger the Y coordinate, the farther that all values are pulled to zero.
	tempVal = abs(blockPosition.y) / ( abs(blockPosition.y) + 100 );
	fTemperature	-= (tempVal*1.1f-0.1f)*fTemperature;
	fHumidity		-= (tempVal*1.1f-0.1f)*fHumidity;	// But the closer to 0, there's a slight push upwards
	fDensity		-= tempVal*fDensity;

	// The larger the elevation, the lower the temperature and density
	tempVal = fCurrentElevation / ( fCurrentElevation + 9 );
	tempVal = max( tempVal, 0 );
	fTemperature	-= tempVal*fTemperature*0.5f;
	fDensity		-= tempVal*fDensity*0.2f;

	// Limit the sample values
	fTemperature	= max( 0.0f, min( 1.0f, fTemperature ) );
	fHumidity		= max( 0.0f, min( 1.0f, fHumidity ) );

	// Get the biome type
	cResultBiome = pSamplerBiomeType[ (int(fHumidity*63.8f)) + (int(fTemperature*63.8f)*64) ];

	// Check for special terrains
	switch ( cResultTerrain )
	{
	case TER_OUTLANDS:
		if (( cResultBiome == BIO_RAINFOREST )||( cResultBiome == BIO_SWAMP ))
			cResultBiome = BIO_WET_OUTLANDS;
		else
			cResultBiome = BIO_OUTLANDS;
		break;
	case TER_THE_EDGE:
		cResultBiome = BIO_THE_EDGE;
		break;
	case TER_DESERT:
		if ( cResultBiome == BIO_TAIGA )
			cResultBiome = BIO_COLD_DESERT;
		else if (( cResultBiome == BIO_RAINFOREST )||( cResultBiome == BIO_SWAMP ))
			cResultBiome = BIO_GRASS_DESERT;
		else
			cResultBiome = BIO_DESERT;
		break;
	case TER_BADLANDS:
		if ( cResultBiome == BIO_TAIGA )
			cResultBiome = BIO_COLD_DESERT;
		else if ( cResultBiome == BIO_EVERGREEN )
			cResultBiome = BIO_DESERT;
		else
			cResultBiome = BIO_GRASS_DESERT;
		break;
	}

	// Change the result based on the density
	if ( fDensity < 0.45f )
	{
		switch ( cResultBiome )
		{
		case BIO_RAINFOREST:
			cResultBiome = BIO_TROPICAL;
			break;
		case BIO_SWAMP:
			cResultBiome = BIO_MOIST;
			break;
		case BIO_TEMPERATE:
			cResultBiome = BIO_GRASSLAND;
			break;
		case BIO_EVERGREEN:
			cResultBiome = BIO_SAVANNA;
			break;
		case BIO_TAIGA:
			cResultBiome = BIO_TUNDRA;
			break;
		}
	}

	return cResultBiome;
}

// Biome types
char CVoxelTerrain::TerraGen_3p_Default ( CBoob * pBoob, Vector3d const& blockPosition, char inBlock, bool topOpen )
{
	static ftype val;
	static ftype val2;
	// Check if we can generate vegetation
	if (( inBlock == EB_DIRT )&&( topOpen )&&( cLastCheckedBlock != EB_WATER ))
	{
		val = noise_hf->Get3D(blockPosition.z,blockPosition.x,blockPosition.y);
		if ( val < blockPosition.z*2.4f+0.4f )
		{
			// Generate grass and trees
			val2 = noise_hhf->Get3D(blockPosition.x,blockPosition.y,blockPosition.z) * 1.8f;
			val = noise_hf->Get3D(blockPosition.z,blockPosition.x,blockPosition.y)*0.9f + val2*val2*val2;
			if ( val > 0.6f )
			{
				/*CTreeBase* pNewTree = new CTreeBase();
				pNewTree->pBoob = pBoob;
				pNewTree->transform.position = blockPosition * 64.0f + Vector3d( 1,1,2 );
				pNewTree->Generate();
				pBoob->v_foliage.push_back( pNewTree );*/
				/*TerraFoiliage	newFoliageRequest;
				newFoliageRequest.foliage_index	= GetFoliageType( "CTreeBase" );
				newFoliageRequest.position	= foliagePos;
				newFoliageRequest.bitmask	= inData;
				vFoliageQueue.push_back( newFoliageRequest );*/
				if ( val < 0.94f )
					TerraGen_NewFoliage( pBoob, blockPosition * 64.0f + Vector3d( 1,1,2 ), "CTreeBase" );
				else
					TerraGen_NewFoliage( pBoob, blockPosition * 64.0f + Vector3d( 1,1,2 ), "FoliageTreePine" );
			}

			// Create some grass for this block
			for ( float fi = 0.1f; fi < val-0.3f; fi += 1.3f )
			{
				/*sTerraGrass newGrass;
				newGrass.position = blockPosition*64.0f + Vector3d(
					1+noise_hhf->Get3D(blockPosition.x*10.4f+fi*1.7f,blockPosition.y*10.4f,blockPosition.z*10.4f+fi*3.2f)*2,
					1+noise_hhf->Get3D(blockPosition.y*10.4f+fi*4.1f,blockPosition.z*10.4f+fi*6.5f,blockPosition.x*10.4f)*2,
					2 );
				newGrass.type = 0;
				pBoob->v_grass.push_back( newGrass );*/
				Vector3d grassPos = blockPosition*64.0f + Vector3d(
					1+noise_hhf->Get3D(blockPosition.x*10.4f+fi*1.7f,blockPosition.y*10.4f,blockPosition.z*10.4f+fi*3.2f)*2,
					1+noise_hhf->Get3D(blockPosition.y*10.4f+fi*4.1f,blockPosition.z*10.4f+fi*6.5f,blockPosition.x*10.4f)*2,
					2 );
				TerraGen_NewGrass( pBoob, grassPos, 0 );
			}

			return EB_GRASS;
		}
	}
	// Return the input block (no change)
	return inBlock;
}
char CVoxelTerrain::TerraGen_3p_Grassland ( CBoob * pBoob, Vector3d const& blockPosition, char inBlock, bool topOpen )
{
	static ftype val;
	static ftype val2;
	// Check if we can generate vegetation
	if (( inBlock == EB_DIRT )&&( topOpen )&&( cLastCheckedBlock != EB_WATER ))
	{
		val = noise_hf->Get3D(blockPosition.z,blockPosition.x,blockPosition.y);
		if ( val < blockPosition.z*2.4f+0.3f )
		{
			// Generate grass and trees
			val2 = noise_hhf->Get3D(blockPosition.x*10.4f,blockPosition.y*10.4f,blockPosition.z*10.4f) * 1.8f;
			//val = noise_hf->Get3D(blockPosition.z,blockPosition.x,blockPosition.y)*0.9f + val2*val2*val2;
			val = noise_hf->Get3D(blockPosition.z,blockPosition.x,blockPosition.y)*2.0f;
			if ( val > 0.75f )
			{
				if ( val2 > 0.82f )
				{
					TerraGen_NewFoliage( pBoob, blockPosition * 64.0f + Vector3d( 1,1,2 ), "CTreeBase" );
				}
			}
			else
			{
				if ( val2 > 0.79f )
				{
					TerraGen_NewFoliage( pBoob, blockPosition * 64.0f + Vector3d( 1,1,2 ), "FoliageBushGreen" );
				}
			}

			// Create some grass for this block
			for ( float fi = 0.1f; fi < val+0.05; fi += 1.3f )
			{
				Vector3d grassPos = blockPosition*64.0f + Vector3d(
					1+noise_hhf->Get3D(blockPosition.x*10.4f+fi*1.7f,blockPosition.y*10.4f,blockPosition.z*10.4f+fi*3.2f)*2,
					1+noise_hhf->Get3D(blockPosition.y*10.4f+fi*4.1f,blockPosition.z*10.4f+fi*6.5f,blockPosition.x*10.4f)*2,
					2 );
				TerraGen_NewGrass( pBoob, grassPos, 0 );
			}

			return EB_GRASS;
		}
	}
	// Return the input block (no change)
	return inBlock;
}
char CVoxelTerrain::TerraGen_3p_GrassDesert	( CBoob * pBoob, Vector3d const& blockPosition, char inBlock, bool topOpen )
{
	static ftype val;
	static ftype val2;
	// Check if we can generate vegetation
	if (( inBlock == EB_DIRT )&&( topOpen )&&( cLastCheckedBlock != EB_WATER ))
	{
		val = noise->Get3D(blockPosition.z,blockPosition.x,blockPosition.y);
		//if ( val < blockPosition.z*2.4f-0.4f )
		if ( val > 0.45f )
		{
			// Generate grass and trees
			val2 = noise_hhf->Get3D(blockPosition.x,blockPosition.y,blockPosition.z) * 1.8f;
			val = noise_hf->Get3D(blockPosition.z,blockPosition.x,blockPosition.y)*0.9f + val2*val2*val2;
			if ( val > 0.7 )
			{
				/*FoliageBushGreen* pNewTree = new FoliageBushGreen();
				pNewTree->pBoob = pBoob;
				pNewTree->transform.position = blockPosition * 64.0f + Vector3d( 1,1,2 );
				pNewTree->Generate();
				pBoob->v_foliage.push_back( pNewTree );*/
				/*TerraFoiliage	newFoliageRequest;
				newFoliageRequest.foliage_index	= GetFoliageType( "FoliageBushGreen" );
				newFoliageRequest.position	= blockPosition * 64.0f + Vector3d( 1,1,2 );
				newFoliageRequest.bitmask	= 0;
				newFoliageRequest.myBoob = pBoob;
				vFoliageQueue.push_back( newFoliageRequest );*/
				TerraGen_NewFoliage( pBoob, blockPosition * 64.0f + Vector3d( 1,1,2 ), "FoliageBushGreen" );
			}

			return EB_GRASS;
		}
	}
	// Return the input block (no change)
	return inBlock;
}

// == Old Method of Vegetation Generation ==
// Generate grass
void CVoxelTerrain::TerraGenGrassPass ( CBoob * pBoob, LongIntPosition const& position )
{
	for ( char i = 0; i < 8; i += 1 )
	{
		for ( char j = 0; j < 8; j += 1 )
		{
			for ( int k = 0; k < 512; k += 1 )
			{
				// Declared static to save init time
				static Vector3d blockPosition = Vector3d( 0,0,0 );
				static ftype val;
				
				if ( pBoob->data[i].data[j].data[k] == EB_DIRT )
				{
					if ( SideVisible( pBoob, i,j, (char*)pBoob->data[i].data[j].data, k, 8, TOP, 1, blockPosition ) )
					{
						if ( cLastCheckedBlock != EB_WATER ) // because it'll say visible if it's water
						{
							blockPosition.x = position.x + (((i%2) * 16) + ((j%2) * 8) + (k%8))/32.0f;
							blockPosition.y = position.y + ((((i/2)%2) * 16) + (((j/2)%2) * 8) + ((k/8)%8))/32.0f;
							blockPosition.z = position.z + (((i/4) * 16) + ((j/4) * 8)  + (k/64))/32.0f;
							val = noise_hf->Get3D(blockPosition.z,blockPosition.x,blockPosition.y);
							if ( val < blockPosition.z*2.4f+0.4f )
								pBoob->data[i].data[j].data[k] = EB_GRASS;
						}
					}
				}	
			}
		}
	}
}
// Generate trees
void CVoxelTerrain::TerraGenTreePass ( CBoob * pBoob, LongIntPosition const& position )
{
	for ( char i = 0; i < 8; i += 1 )
	{
		for ( char j = 0; j < 8; j += 1 )
		{
			for ( int k = 0; k < 512; k += 1 )
			{
				// Declared static to save init time
				static Vector3d blockPosition = Vector3d( 0,0,0 );
				static ftype val;
				static ftype val2;
				
				if ( pBoob->data[i].data[j].data[k] == EB_GRASS )
				{
					if ( SideVisible( pBoob, i,j, (char*)pBoob->data[i].data[j].data, k, 8, TOP, 1, blockPosition ) )
					{
						// And we DO NOT generate a tree in water!
						//if ( cLastCheckedBlock == EB_WATER ) //only generates trees on grass
						blockPosition.x = position.x + (((i%2) * 16) + ((j%2) * 8) + (k%8))/32.0f;
						blockPosition.y = position.y + ((((i/2)%2) * 16) + (((j/2)%2) * 8) + ((k/8)%8))/32.0f;
						blockPosition.z = position.z + (((i/4) * 16) + ((j/4) * 8)  + (k/64))/32.0f;
						val2 = noise_hhf->Get3D(blockPosition.x,blockPosition.y,blockPosition.z) * 1.8f;
						val = noise_hf->Get3D(blockPosition.z,blockPosition.x,blockPosition.y)*0.9f + val2*val2*val2;
						/*if ( val < blockPosition.z*2.4f+0.4f )
						{
							pBoob->data[i].data[j].data[k] = EB_GRASS;*/
						//if ( val < blockPosition.z )
						if ( val > 0.6 )
						{
							//CFoliageTree* pNewTree = new CFoliageTree();
							CTreeBase* pNewTree = new CTreeBase();
							pNewTree->pBoob = pBoob;
							pNewTree->transform.position = blockPosition * 64.0f + Vector3d( 1,1,2 );
							//pNewTree->transform.rotation.z = val*360.0f;
							//pNewTree->transform.scale = Vector3d( 1,1,1 ) * ( 0.12f + val*0.03f );
							pNewTree->Generate();
							//pNewTree->RemoveReference();
							pBoob->v_foliage.push_back( pNewTree );

						}
					}
				}	
			}
		}
	}
}


// == Cleanup Pass ==
void CVoxelTerrain::GenerateTerrainSectorCleanup ( CBoob * pBoob )
{
	if (( pBoob != NULL )&&( pBoob->bitinfo[0] == true ))
	{
		pBoob->bitinfo[0] = false;
		pBoob->bitinfo[7] = false;

		// Boob now has data
		pBoob->hasData = true;

		if ( bSpreadTerrain )
		{
			// TODO: FIX THIS SOMEDAY
			GenerateTerrainSectorCleanup( pBoob->top );
			GenerateTerrainSectorCleanup( pBoob->front );
			GenerateTerrainSectorCleanup( pBoob->left );
			GenerateTerrainSectorCleanup( pBoob->bottom );
			GenerateTerrainSectorCleanup( pBoob->back );
			GenerateTerrainSectorCleanup( pBoob->right );
		}
	}
}

/// == Freaking Pubs ==
char	CVoxelTerrain::TerraGen_pub_GetType	( Vector3d const& blockPosition )
{
	TerraGenGetSamplers();
	if ( noise_pub == NULL )
	{
		noise_pub		= new Perlin( 1,2,1,iTerrainSeed );
		noise_pub_hf	= new Perlin( 1,5,1,iTerrainSeed );
		noise_pub_hhf	= new Perlin( 1,15,1,iTerrainSeed+1 );
		noise_pub_lf	= new Perlin( 1,0.5f,1,iTerrainSeed-1 );
		noise_pub_terra	= new Perlin( 2,0.03f,1,iTerrainSeed+1 );
		noise_pub_biome	= new Perlin( 2,0.04f,1,iTerrainSeed-1 );
	}

	// Generate terrain type
	static ftype fTempOffs;
	static ftype fRockiness;
	static ftype fUnpredictability;
	static ftype fPubCurrentElevation;
	//static ftype fCurrentElevation;
	static ftype fCurrentSamplerZ;
	static unsigned char cResultTerrain;
	static unsigned char cResultBlock;
	// Get elevation
	fPubCurrentElevation = TerraGen_pub_GetElevation( blockPosition );
	// Get Z sampler
	fCurrentSamplerZ = max( min( fPubCurrentElevation, blockPosition.z ), fPubCurrentElevation-0.5f );
	// Input functions for rockiness and unpredictability
	fRockiness			= noise_pub_terra->Get3D(blockPosition.x*1.6f,blockPosition.y*1.6f,fCurrentSamplerZ) + 0.5f;
	fUnpredictability	= noise_pub_terra->Get3D(blockPosition.y,blockPosition.x,fCurrentSamplerZ) + 0.5f;
	// Add another layer of noise to the sample values
	fTempOffs = noise_pub->Get3D( blockPosition.x*1.6f,blockPosition.y*1.6f,blockPosition.z*1.6f )*0.05f;
	fRockiness			= max( 0.0f, min( 1.0f, fRockiness+fTempOffs ) );
	fTempOffs = noise_pub->Get3D( blockPosition.y*1.3f,blockPosition.z*1.6f,blockPosition.x*1.6f )*0.05f;
	fUnpredictability	= max( 0.0f, min( 1.0f, fUnpredictability+fTempOffs ) );
	// Get the terrain type
	cResultTerrain = pSamplerTerrainType[ (int(fUnpredictability*63.8f)) + (int(fRockiness*63.8f)*64) ];

	return cResultTerrain;
}
ftype	CVoxelTerrain::TerraGen_pub_GetElevation( Vector3d const& blockPosition )
{
	static ftype val;
	static ftype val2;
	static ftype val3;
	//val = 0.02f + (noise_biome->Get3D(blockPosition.x*1.6f,blockPosition.y*1.6f,0.0f)+0.7f-0.5f)*0.4f;
	//return 0.02f;
	//val = 0.02f + (noise_biome->Get3D(blockPosition.x*1.6f,blockPosition.y*1.6f,0.0f)+0.7f-0.5f)*1.8f;
	val = 0.02f + (noise_pub_terra->Get3D(blockPosition.x*1.6f,blockPosition.y*1.6f,0.0f)+0.18f)*1.8f;

	// second-level noise
	val3 = noise_pub_terra->Get3D(-blockPosition.y*1.1f,-blockPosition.x*1.1f,0.0f);
	val += sqr(val3)*2.0f;

	// desert-ocean border fix
	val2 = noise_pub_terra->Get3D(blockPosition.y,blockPosition.x,0.0f) + 0.5f;
	val2 = max( 0, min( 1, 1-abs((val2-0.75f)/0.06f) ) ) * max( 0, -val*1.1f );
	val += val2;

	if ( val < 0.0f )
		val = -sqr(val*2.1f);
	else if ( val > 1.0f )
		val = sqr(val);
	return val;
}
char	CVoxelTerrain::TerraGen_pub_GetBiome	( Vector3d const& blockPosition )
{
	TerraGenGetSamplers();
	if ( noise_pub == NULL )
	{
		noise_pub		= new Perlin( 1,2,1,iTerrainSeed );
		noise_pub_hf	= new Perlin( 1,5,1,iTerrainSeed );
		noise_pub_hhf	= new Perlin( 1,15,1,iTerrainSeed+1 );
		noise_pub_lf	= new Perlin( 1,0.5f,1,iTerrainSeed-1 );
		noise_pub_terra	= new Perlin( 2,0.03f,1,iTerrainSeed+1 );
		noise_pub_biome	= new Perlin( 2,0.04f,1,iTerrainSeed-1 );
	}

	// == Sampling ==
	// Generate biome type
	static ftype fTemperature;
	static ftype fHumidity;
	static ftype fDensity;

	static ftype fPubCurrentElevation;
	static ftype fCurrentSamplerZ;

	static ftype tempVal;

	static unsigned char cResultBiome;
	static unsigned char cResultTerrain;
	// Get terrain
	cResultTerrain = TerraGen_pub_GetType( blockPosition );
	// Get elevation
	fPubCurrentElevation = TerraGen_pub_GetElevation( blockPosition );
	// Get Z sampler
	fCurrentSamplerZ = max( min( fPubCurrentElevation, blockPosition.z ), fPubCurrentElevation-0.5f );
	// Input functions for temperature, humidity, and density
	fTemperature	= noise_pub_biome->Get3D(blockPosition.x*1.6f,blockPosition.y*1.6f,fCurrentSamplerZ) + 0.5f;
	fHumidity		= noise_pub_biome->Get3D(blockPosition.y,blockPosition.x,fCurrentSamplerZ) + 0.5f;
	fDensity		= noise_pub_biome->Get3D(-blockPosition.y*1.2f,-blockPosition.x*1.2f,fCurrentSamplerZ ) + 0.5f;
	// Add another layer of noise to the sample values
	fTemperature	+= noise_pub->Get3D( blockPosition.x*1.3f,blockPosition.y*1.3f,blockPosition.z*1.3f )*0.05f;
	fHumidity		+= noise_pub->Get3D( blockPosition.y*1.2f,blockPosition.z*1.2f,blockPosition.x*1.2f )*0.05f;


	// Find the result biome at the position
	//cResultBiome = BIO_DEFAULT;

	// Terrain Based Weights
	switch ( cResultTerrain )
	{
	case TER_BADLANDS:
		// Perform a slight pull to the center of the generation
		fTemperature	+= (0.65f-fTemperature)*0.12f;
		fHumidity		+= (0.5f-fHumidity)*0.1f;
		break;
	case TER_DESERT:
		// Perform a slight pull to the far left area of generation
		fTemperature	+= (0.9f-fTemperature)*0.05f;
		fHumidity		+= (0.0f-fHumidity)*0.1f;
		break;
	case TER_MOUNTAINS:
		// Perform a slight pull to the lower evergreen area
		fTemperature	+= (0.4f-fTemperature)*0.1f;
		break;
	case TER_OCEAN:
	case TER_SPIRES:
	case TER_ISLANDS:
		// Perform a stronger pull to the rainforest area
		fTemperature	+= (1.0f-fTemperature)*0.15f;
		fHumidity		+= (1.1f-fHumidity)*0.15f;
		break;
	}

	// Calculate weights
	// The larger the Y coordinate, the farther that all values are pulled to zero.
	tempVal = abs(blockPosition.y) / ( abs(blockPosition.y) + 100 );
	fTemperature	-= (tempVal*1.1f-0.1f)*fTemperature;
	fHumidity		-= (tempVal*1.1f-0.1f)*fHumidity;	// But the closer to 0, there's a slight push upwards
	fDensity		-= tempVal*fDensity;

	// The larger the elevation, the lower the temperature and density
	tempVal = fPubCurrentElevation / ( fPubCurrentElevation + 9 );
	tempVal = max( tempVal, 0 );
	fTemperature	-= tempVal*fTemperature*0.5f;
	fDensity		-= tempVal*fDensity*0.2f;

	// Limit the sample values
	fTemperature	= max( 0.0f, min( 1.0f, fTemperature ) );
	fHumidity		= max( 0.0f, min( 1.0f, fHumidity ) );

	// Get the biome type
	cResultBiome = pSamplerBiomeType[ (int(fHumidity*63.8f)) + (int(fTemperature*63.8f)*64) ];

	// Check for special terrains
	switch ( cResultTerrain )
	{
	case TER_OUTLANDS:
		if (( cResultBiome == BIO_RAINFOREST )||( cResultBiome == BIO_SWAMP ))
			cResultBiome = BIO_WET_OUTLANDS;
		else
			cResultBiome = BIO_OUTLANDS;
		break;
	case TER_THE_EDGE:
		cResultBiome = BIO_THE_EDGE;
		break;
	case TER_DESERT:
		if ( cResultBiome == BIO_TAIGA )
			cResultBiome = BIO_COLD_DESERT;
		else if (( cResultBiome == BIO_RAINFOREST )||( cResultBiome == BIO_SWAMP ))
			cResultBiome = BIO_GRASS_DESERT;
		else
			cResultBiome = BIO_DESERT;
		break;
	case TER_BADLANDS:
		if ( cResultBiome == BIO_TAIGA )
			cResultBiome = BIO_COLD_DESERT;
		else if ( cResultBiome == BIO_EVERGREEN )
			cResultBiome = BIO_DESERT;
		else
			cResultBiome = BIO_GRASS_DESERT;
		break;
	}

	// Change the result based on the density
	if ( fDensity < 0.45f )
	{
		switch ( cResultBiome )
		{
		case BIO_RAINFOREST:
			cResultBiome = BIO_TROPICAL;
			break;
		case BIO_SWAMP:
			cResultBiome = BIO_MOIST;
			break;
		case BIO_TEMPERATE:
			cResultBiome = BIO_GRASSLAND;
			break;
		case BIO_EVERGREEN:
			cResultBiome = BIO_SAVANNA;
			break;
		case BIO_TAIGA:
			cResultBiome = BIO_TUNDRA;
			break;
		}
	}

	return cResultBiome;
}
#endif