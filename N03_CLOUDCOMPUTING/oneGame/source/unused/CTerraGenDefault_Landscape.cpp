
// ======== CTerraGenDefault_Landscape =======
// This is the definition for the first pass landscape generation.
// Terrain types are handled here.

#include "CTerraGenDefault.h"


void CTerraGenDefault::TerraGenLandscapePass ( CBoob * pBoob, RangeVector const& position )
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

				// Get the block's noise position
				blockPosition.x = position.x + (((i%2) * 16) + ((j%2) * 8) + (k%8))/32.0f;
				blockPosition.y = position.y + ((((i/2)%2) * 16) + (((j/2)%2) * 8) + ((k/8)%8))/32.0f;
				blockPosition.z = position.z + (((i/4) * 16) + ((j/4) * 8)  + (k/64))/32.0f;

				// Generate terrain type
				//cResultTerrain = TerraGen_priv_GetType( blockPosition ); // Sets fCurrentElevation
				fCurrentElevation = TerraGen_1p_GetElevation( blockPosition ); // Sets iCurrentTerraType
				cResultTerrain = iCurrentTerraType;

				// Add elevation to boob
				pBoob->elevation += fCurrentElevation / 32768.0f;

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

				pBoob->data[i].data[j].data[k].block = cResultBlock;
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


// Terrain types
ushort CTerraGenDefault::TerraGen_1p_Default ( CBoob * pBoob, Vector3d const& blockPosition )
{
	static ftype val;
	val = noise->Get3D(blockPosition.x,blockPosition.y,blockPosition.z);
	if ( val > blockPosition.z*1.02f ) //5-6 grass lands, 0.2 outlands
		return EB_DIRT;
	else
		return EB_NONE;
}
ushort CTerraGenDefault::TerraGen_1p_Flatlands ( CBoob * pBoob, Vector3d const& blockPosition )
{
	static ftype val;
	val = noise->Get3D(blockPosition.x,blockPosition.y,blockPosition.z);
	if ( val > (blockPosition.z-fCurrentElevation)*4.8f ) //5-6 grass lands, 0.2 outlands
		return EB_DIRT;
	else
		return EB_NONE;
}
ushort CTerraGenDefault::TerraGen_1p_Outlands ( CBoob * pBoob, Vector3d const& blockPosition )
{
	static ftype val;
	val = noise->Get3D(blockPosition.x,blockPosition.y,blockPosition.z);
	if ( val > (blockPosition.z-fCurrentElevation)*0.2f ) //5-6 grass lands, 0.2 outlands
		return EB_DIRT;
	else
		return EB_NONE;
}
ushort CTerraGenDefault::TerraGen_1p_Hilllands ( CBoob * pBoob, Vector3d const& blockPosition )
{
	static ftype val;
	val = noise->Get3D(blockPosition.x*0.7f,blockPosition.y*0.7f,blockPosition.z);
	if ( val > (blockPosition.z-fCurrentElevation)*1.04f ) //5-6 grass lands, 0.2 outlands
		return EB_DIRT;
	else
		return EB_NONE;
}
ushort CTerraGenDefault::TerraGen_1p_Mountains ( CBoob * pBoob, Vector3d const& blockPosition )
{
	static ftype val;
	static ftype val2;
	static ftype val3;
	static ftype elevline;
	val = noise->Get3D(blockPosition.x*0.7f,blockPosition.y*0.7f,blockPosition.z); // from -0.5 to 0.5
	val2 = (noise_hf->Get(blockPosition.x*0.13f,blockPosition.y*0.13f)+0.5f)*2.0f; // from 0 to 2
	val3 = noise->Get(blockPosition.x*0.03f,blockPosition.y*0.03f)+0.4f; // from -0.1 to 0.9
	val3 = cub( val3 )*2.0f; // from -0.002 to 1.458
	// if random sample is > than (distance to avg elevation*1.04 + [-2.8 to 0.2])
	elevline = (blockPosition.z-fCurrentElevation)*1.04f;
	//elevline += (0.1f - val2 - val3) * std::max<ftype>( (val3+0.1f)*0.2f, std::min<ftype>( 1, (fCurrentElevation-1.8f)*1.8f ) );
	elevline += (0.1f - val2 - val3 +(0.8f)) * std::max<ftype>( 0, std::min<ftype>( 1, (fCurrentRockiness-0.77f)*8.0f ) );
	// multiplying the rockiness by elevation should smooth the transition line
	// except that idea is broke as fuck so
	//elevline += (0.1f - val2 - val3);

	if ( val > elevline ) 
		return EB_DIRT;
	else
		return EB_NONE;
}
ushort CTerraGenDefault::TerraGen_1p_Islands ( CBoob * pBoob, Vector3d const& blockPosition )
{
	static ftype val;
	val = noise->Get3D(blockPosition.x*0.4f,blockPosition.y*0.4f,blockPosition.z);
	static ftype cmpVal;
	static ftype zVal;
	zVal = (blockPosition.z-fCurrentElevation);
	//cmpVal = ( zVal*0.9f + 0.04f );
	cmpVal = ( zVal*0.9f + 0.08f );
	if ( val > cmpVal ) //5-6 grass lands, 0.2 outlands
	{
		if ( fabs( val-cmpVal ) < -0.16f*(zVal-0.12f) )
			return EB_SAND;
		else
			return EB_DIRT;
	}
	else
	{
		if ( blockPosition.z >= -0.01f )
			return EB_NONE;
		else
			return EB_WATER;
			//return EB_NONE;
			//return EB_WATER;
	}
}
ushort CTerraGenDefault::TerraGen_1p_Ocean ( CBoob * pBoob, Vector3d const& blockPosition )
{
	static ftype val;
	static ftype val2;
	//val = 2.0f - sqrt( sin( noise->Get3D(blockPosition.y*0.6f,blockPosition.x*0.6f,blockPosition.z) * ftype(PI) ) + 1.0f );
	val = sin( noise->Get3D(blockPosition.y*0.65f,blockPosition.x*0.65f,blockPosition.z) * ftype(PI) );
	val2 = (noise_hf->Get(blockPosition.x*0.14f,blockPosition.y*0.14f)+0.2f)*1.2f;
	if ( val > ((blockPosition.z-fCurrentElevation)*4.6f) + val2 )
	{
		return EB_SAND;
	}
	else
	{
		if ( blockPosition.z >= -0.01f )
			return EB_NONE;
		else
			return EB_WATER;
	}
}
ushort CTerraGenDefault::TerraGen_1p_Desert ( CBoob * pBoob, Vector3d const& blockPosition )
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
ushort CTerraGenDefault::TerraGen_1p_Badlands ( CBoob * pBoob, Vector3d const& blockPosition )
{
	static ftype val;
	val = noise_lf->Get3D(blockPosition.x*0.7f,blockPosition.y*0.7f,blockPosition.z*0.13f) *(1.5f);
	if ( val > 0.2f )
		val = std::max( 0.25f, val*0.35f+0.1f );
	else
		val *= 0.15f;


	static ftype val2;
	val2 = noise->Get3D(blockPosition.x*1.2f,blockPosition.y*1.2f,blockPosition.z*0.23f)*2.0f *(1.5f);
	val += cub(val2)*0.7f;

	static ftype val3;
	val3 = noise_lf->Get3D(-blockPosition.y*0.7f,-blockPosition.x*0.7f,blockPosition.z*0.05f)*2.0f *(1.5f);
	val3 = cub( val3 ) * cub( val3 ) * 0.4f;

	if ( val > (blockPosition.z-fCurrentElevation)*0.5f + val3 )
	{
		//return EB_DIRT; //todo: change to red dirt, not just no grass
		val = noise_lf->Unnormalize( noise_lf->Get3D( blockPosition.y*0.03f, blockPosition.x*0.03f, blockPosition.z*8.5f ) );
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
ushort CTerraGenDefault::TerraGen_1p_Spires ( CBoob * pBoob, Vector3d const& blockPosition )
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
	val2 = noise->Get3D( blockPosition.x*0.43f, blockPosition.y*0.43f, blockPosition.z*0.005f ) * 0.1f *(1.7f);

	val = noise_hf->Get3D( blockPosition.y*0.47f,blockPosition.x*0.47f,blockPosition.z*0.015f ) *(1.7f);
	if ( val < 0 )
		val = 0;
	val -= 0.14f + val2;
	if ( val > blockPosition.z*0.06f )
	{
		if ( val > (blockPosition.z+0.7f)*0.11f )
			return EB_STONE;
		else
			return EB_DIRT;
	}
	else
	{
		return TerraGen_1p_Ocean( pBoob, blockPosition );
	}
}


// After creating terrain, switch to stone
void CTerraGenDefault::TerraGenStonePass ( CBoob * pBoob, RangeVector const& position )
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

				if (( pBoob->data[i].data[j].data[k].block != EB_NONE )&&( pBoob->data[i].data[j].data[k].block != EB_WATER ))
				{
					blockPosition.x = position.x + (((i%2) * 16) + ((j%2) * 8) + (k%8))/32.0f;
					blockPosition.y = position.y + ((((i/2)%2) * 16) + (((j/2)%2) * 8) + ((k/8)%8))/32.0f;
					blockPosition.z = position.z + (((i/4) * 16) + ((j/4) * 8)  + (k/64))/32.0f;

					// Get elevation
					fCurrentElevation = TerraGen_1p_GetElevation( blockPosition ) - 0.02f;
					//fCurrentElevation = 0;
					//if ( pBoob->data[i].data[j].data[k].block == EB_SAND ) {
					//	fCurrentElevation -= 0.03f; // Lower target elevation if is all sand
					//}
					// Mod target elevation based on terrain
					switch ( iCurrentTerraType ) {
						case TER_SPIRES:
						case TER_OCEAN:
						case TER_ISLANDS:
							fCurrentElevation -= 0.04f;
							break;
						case TER_MOUNTAINS:
							fCurrentElevation += 0.08f;
							break;
					}

					// Get noise
					fMainTer = noise->Get3D( blockPosition.x,blockPosition.y,blockPosition.z );
					fSubTer = noise_hf->Get3D( blockPosition.x,blockPosition.y,blockPosition.z );
					if ( fMainTer > (blockPosition.z-fCurrentElevation)*1.7f+0.24f+fSubTer*0.42f )
						pBoob->data[i].data[j].data[k].block = EB_STONE;
				}
			}
		}
	}
}

// With all stone, create splotches of dirt. They need to be less common the deeper one goes.
void CTerraGenDefault::TerraGenUnderdirtPass ( CBoob * pBoob, RangeVector const& position )
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

				//if (( pBoob->data[i].data[j].data[k] != EB_NONE )&&( pBoob->data[i].data[j].data[k] != EB_WATER ))
				if (( pBoob->data[i].data[j].data[k].block == EB_STONE ))
				{
					blockPosition.x = position.x + (((i%2) * 16) + ((j%2) * 8) + (k%8))/32.0f;
					blockPosition.y = position.y + ((((i/2)%2) * 16) + (((j/2)%2) * 8) + ((k/8)%8))/32.0f;
					blockPosition.z = position.z + (((i/4) * 16) + ((j/4) * 8)  + (k/64))/32.0f;

					// Get elevation
					fCurrentElevation = TerraGen_1p_GetElevation( blockPosition );

					// Get noise
					/*fMainTer = noise->Get3D( blockPosition.x,blockPosition.y,blockPosition.z );
					fSubTer = noise_hf->Get3D( blockPosition.x,blockPosition.y,blockPosition.z );
					if ( fMainTer > (blockPosition.z-fCurrentElevation)*1.7f+0.24f+fSubTer*0.42f )
						pBoob->data[i].data[j].data[k] = EB_STONE;*/
					fMainTer = noise_hf->Get3D( blockPosition.y*0.6f,blockPosition.x*0.6f,blockPosition.z );
					/*if ( fMainTer*0.5f > (fCurrentElevation-blockPosition.z*0.01f) )
						pBoob->data[i].data[j].data[k] = EB_DIRT;*/
					fSubTer = std::max( 0.1f, (fCurrentElevation-blockPosition.z)*0.06f -0.05f );
					if ( fMainTer*0.5f > fSubTer )
						pBoob->data[i].data[j].data[k].block = EB_DIRT;
				}
			}
		}
	}
}