
// ======== CTerraGenDefault =======
// This is the definition for the constructor, destructor, 
// and high-level generation routines of the default generation.
// Initialization of the generator is handled in the constructor.


#include "CTerraGenDebug.h"
#include "CGameSettings.h"
#include "CTimeProfiler.h"
#include "Perlin.h"

// Initialize variables
CTerraGenDebug::CTerraGenDebug ( CVoxelTerrain* inTerrain, long int seed )
	: CTerrainGenerator ( inTerrain, seed )
{
	{
		noise		= new Perlin( 1,2.03f,1,iTerrainSeed );
		noise_terra	= new Perlin( 2,0.032f*1.3f*0.7f,1,iTerrainSeed+1 );
		noise_biome	= new Perlin( 2,0.043f*1.6f,1,iTerrainSeed-1 );

		noise_pub		= new Perlin( 1,2.03f,1,iTerrainSeed );
		noise_pub_biome	= new Perlin( 2,0.032f*1.3f*0.7f,1,iTerrainSeed+1 );
		noise_pub_terra = new Perlin( 2,0.043f*1.6f,1,iTerrainSeed-1 );
	}
}

CTerraGenDebug::~CTerraGenDebug ( void )
{
	// Free Memory
	delete noise;
	delete noise_biome;
	delete noise_terra;

	delete noise_pub;
	delete noise_pub_biome;
	delete noise_pub_terra;
}



// Main Generation Routine

// === TERRAIN GENERATION PASSES ===
// == First Pass == 
void CTerraGenDebug::GenerateTerrainSector ( CBoob * pBoob, RangeVector const& position )
{
	if (( pBoob != NULL )&&( pBoob->bitinfo[3] == false ))
	{
		//TimeProfiler.BeginTimeProfile( "terraGen" );

		pBoob->bitinfo[3] = true;	// Set terrain as already generated
		pBoob->bitinfo[0] = true;

		// Set position
		pBoob->position = Vector3d( ftype(position.x*64+32),ftype(position.y*64+32),ftype(position.z*64+32) );
		pBoob->li_position = position;

		// Spread terrain generation. TODO: FIX THIS SOMEDAY
		if ( bSpreadTerrain )
		{
			if ( position.z >= 0 )
				GenerateTerrainSector( pBoob->top, RangeVector( position.x, position.y, position.z+1 ) );
			if ( position.x >= 0 )
				GenerateTerrainSector( pBoob->front, RangeVector( position.x+1, position.y, position.z ) );
			if ( position.y >= 0 )
				GenerateTerrainSector( pBoob->left, RangeVector( position.x, position.y+1, position.z ) );
			if ( position.z <= 0 )
				GenerateTerrainSector( pBoob->bottom, RangeVector( position.x, position.y, position.z-1 ) );
			if ( position.x <= 0 )
				GenerateTerrainSector( pBoob->back, RangeVector( position.x-1, position.y, position.z ) );
			if ( position.y <= 0 )
				GenerateTerrainSector( pBoob->right, RangeVector( position.x, position.y-1, position.z ) );
		}
		// Generate the terrain
		TerraGenLandscapePass( pBoob, position );

		//TimeProfiler.EndPrintTimeProfile( "terraGen" );
	}
}

void CTerraGenDebug::TerraGenLandscapePass ( CBoob * pBoob, RangeVector const& position )
{
	// Declare value of terrain
	unsigned short iTerrainWeight [256];
	for ( short i = 0; i < 256; ++i )
		iTerrainWeight[i] = 0;
	
	// Reset elevation
	pBoob->elevation = 0;

	// Eventually, this will become a sort of 8x sample + blend between several different generation algorithms.
	// The current algorithm is the basic test terrain algorithm. Other variations include say flatlands, mountains, and other such things.
	// Create the basic terrain
	for ( char i = 0; i < 8; i += 1 )
	{
		for ( char j = 0; j < 8; j += 1 )
		{
			for ( short k = 0; k < 512; k += 1 )
			{
				// Declare static to save stack time
				static Vector3d blockPosition;
				static unsigned char cResultTerrain;
				static unsigned short cResultBlock;

				// Get the block's position
				blockPosition.x = position.x + (((i%2) * 16) + ((j%2) * 8) + (k%8))/32.0f;
				blockPosition.y = position.y + ((((i/2)%2) * 16) + (((j/2)%2) * 8) + ((k/8)%8))/32.0f;
				blockPosition.z = position.z + (((i/4) * 16) + ((j/4) * 8)  + (k/64))/32.0f;

				// Weight this terrain type
				cResultTerrain = TerraGen_priv_GetType( blockPosition );
				iTerrainWeight[cResultTerrain] += 1;
				iCurrentTerraType = cResultTerrain;

				// Do generation
				cResultBlock = TerraGen_Default( pBoob, blockPosition );

				pBoob->data[i].data[j].data[k].block = cResultBlock;

				// Add test components
				BlockInfo currentBlock;
				currentBlock.b16index = i;
				currentBlock.b8index = j;
				currentBlock.b1index = k;
				currentBlock.block = pBoob->data[i].data[j].data[k];
				if ( iCurrentTerraType == TER_DESERT )
				{
					if ( cResultBlock == EB_SAND ) {
						if (blockPosition.z > 0.16875f) {
							if ( (abs(blockPosition.x)<0.7) && (abs(blockPosition.y)<0.7) )
							{
								if ( TerraGen_GetComponentCount( pBoob, "PropChestBase" ) == 0 ) {
									TerraGen_NewComponent( pBoob, currentBlock, blockPosition * 64.0f + Vector3d(1,1,2), "PropChestBase" );
								}
							}
							else if ( (abs(blockPosition.x)>0.9) && (abs(blockPosition.y)>0.9) )
							{
								if ( TerraGen_GetComponentCount( pBoob, "PropDoorBase" ) == 0 ) {
									TerraGen_NewComponent( pBoob, currentBlock, blockPosition * 64.0f + Vector3d(0,1,2), "PropDoorBase" );
								}
							}
						}
					}
				}
				//
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


ushort CTerraGenDebug::TerraGen_Default ( CBoob * pBoob, Vector3d const& blockPos )
{
	if ( fabs(blockPos.x) < 2.0f && fabs(blockPos.y) < 2.0f )
	{
		switch (iCurrentTerraType) {
		case TER_SPIRES:
			if ( fabs(blockPos.z) < 0.5f ) {
				return EB_WOOD;
			}
			break;
		case TER_DESERT:
			{
				ftype offs = noise->Get3D(blockPos.x,blockPos.y,blockPos.z);
				offs = cub(offs)*2.5f;
				//if ( fabs(blockPos.z) < 0.2f+offs ) {
				if ( blockPos.z > -0.3f && blockPos.z < 0.2f+offs ) {
					return EB_SAND;
				}
			}
			break;
		case TER_FLATLANDS:
			if ( fabs(blockPos.x+0.5f) < 0.1f && fabs(blockPos.y-0.5f) < 0.1f && fabs(blockPos.z) < 0.2f ) {
				return EB_HEMATITE;
			}
			else if ( fabs(blockPos.z) < 0.2f-0.03125f ) {
				return EB_DIRT;
			}
			else if ( blockPos.z > 0.0f && blockPos.z < 0.2f ) {
				return EB_GRASS;
			}
			break;
		case TER_HILLLANDS:
			{
				ftype ele = 0.2f + noise->Get3D(blockPos.x,blockPos.y,blockPos.z)*0.4f;
				if ( fabs(blockPos.z) < ele ) {
					return EB_DIRT;
				}
				else if ( blockPos.z > 0.0f && blockPos.z < 0.03125f+ele ) {
					return EB_GRASS;
				}
			}
			break;
		case TER_ISLANDS:
			if ( fabs(blockPos.z) < 0.2f ) {
				if ( noise->Get3D(blockPos.x,blockPos.y,blockPos.z) > 0.1f ) {
					return EB_SAND;
				}
			}
			break;
		}
	}
	
	return EB_NONE;
}



// Biome samplers
char	CTerraGenDebug::TerraGen_pub_GetType	( Vector3d const& pos )
{
	if ( fabs(pos.x) < 0.1f && fabs(pos.y) < 0.1f ) {
		return TER_SPIRES;
	}
	if ( pos.x < 0 ) {
		if ( pos.y < 0 ) {
			return TER_DESERT;
		}
		else {
			return TER_FLATLANDS;
		}
	}
	else {
		if ( pos.y < 0 ) {
			return TER_HILLLANDS;
		}
		else {
			return TER_ISLANDS;
		}
	}
}
char	CTerraGenDebug::TerraGen_pub_GetBiome	( Vector3d const& pos )
{
	if ( fabs(pos.x) < 0.1f && fabs(pos.y) < 0.1f ) {
		return BIO_TAIGA;
	}
	if ( pos.x < 0 ) {
		if ( pos.y < 0 ) {
			return BIO_DESERT;
		}
		else {
			return BIO_GRASSLAND;
		}
	}
	else {
		if ( pos.y < 0 ) {
			return BIO_S_TUNDRA;
		}
		else {
			return BIO_TROPICAL;
		}
	}
}
ftype	CTerraGenDebug::TerraGen_pub_GetElevation ( Vector3d const& )
{
	return 0;
}

char	CTerraGenDebug::TerraGen_priv_GetType	( Vector3d const& pos )
{
	if ( fabs(pos.x) < 0.1f && fabs(pos.y) < 0.1f ) {
		return TER_SPIRES;
	}
	if ( pos.x < 0 ) {
		if ( pos.y < 0 ) {
			return TER_DESERT;
		}
		else {
			return TER_FLATLANDS;
		}
	}
	else {
		if ( pos.y < 0 ) {
			return TER_HILLLANDS;
		}
		else {
			return TER_ISLANDS;
		}
	}
}
char	CTerraGenDebug::TerraGen_priv_GetBiome	( Vector3d const& pos )
{
	if ( fabs(pos.x) < 0.1f && fabs(pos.y) < 0.1f ) {
		return BIO_TAIGA;
	}
	if ( pos.x < 0 ) {
		if ( pos.y < 0 ) {
			return BIO_DESERT;
		}
		else {
			return BIO_GRASSLAND;
		}
	}
	else {
		if ( pos.y < 0 ) {
			return BIO_S_TUNDRA;
		}
		else {
			return BIO_TROPICAL;
		}
	}
}
ftype	CTerraGenDebug::TerraGen_1p_GetElevation ( Vector3d const& )
{
	return 0;
}