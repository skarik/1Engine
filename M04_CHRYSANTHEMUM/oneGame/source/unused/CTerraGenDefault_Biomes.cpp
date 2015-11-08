
// ======== CTerraGenDefault_Biomes =======
// This is the definition for the third pass generation.
// Biome creation is handled here. (Possibly water creation as well)

#include "CTerraGenDefault.h"

// Generate Biomes.
void CTerraGenDefault::TerraGenBiomePass ( CBoob * pBoob, RangeVector const& position )
{
	for ( char i = 0; i < 8; i += 1 )
	{
		for ( char j = 0; j < 8; j += 1 )
		{
			for ( int k = 0; k < 512; k += 1 )
			{
				static Vector3d blockPosition = Vector3d( 0,0,0 );
				static bool topVis, botVis;

				// Grab block info and position
				if ( (((i/4) * 16) + ((j/4) * 8)  + (k/64)) != 0 )
				{
					if ( pBoob->data[i].data[j].data[k].block == EB_NONE ) {
						botVis = SideVisible( pBoob, i,j, (terra_t*)pBoob->data[i].data[j].data, k, 8, BOTTOM, 1 );
						if ( !botVis ) {
							if ( cLastCheckedBlock == EB_ANC_GRAVITY_JOKE ) {
								cBottomCheckBlock = EB_NONE;
								botVis = true;
							}
							else {
								cBottomCheckBlock = cLastCheckedBlock;
							}
						}
						else {
							cBottomCheckBlock = cLastCheckedBlock;
						}
					}
					else {
						cBottomCheckBlock = EB_NONE;
						botVis = false;
					}
				}
				else {
					cBottomCheckBlock = EB_NONE;
					botVis = false;
				}
				blockPosition.x = position.x + (((i%2) * 16) + ((j%2) * 8) + (k%8))/32.0f;
				blockPosition.y = position.y + ((((i/2)%2) * 16) + (((j/2)%2) * 8) + ((k/8)%8))/32.0f;
				blockPosition.z = position.z + (((i/4) * 16) + ((j/4) * 8)  + (k/64))/32.0f;
				topVis = SideVisible( pBoob, i,j, (terra_t*)pBoob->data[i].data[j].data, k, 8, TOP, 1 );
				
				// Get current elevation
				fCurrentElevation = TerraGen_1p_GetElevation( blockPosition );
				// Only do biome stuff if above ground-line
				if ( blockPosition.z-fCurrentElevation < -1.0f ) // TODO: separate generation for the underground
					continue;

				// == Sampling ==
				static unsigned char cResultBiome;
				static unsigned short cResultBlock;
				if (( i%4 == 0 )||( j%4 == 0 )||( k%4 == 0 ))
					cResultBiome = TerraGen_priv_GetBiome( blockPosition );
				unsigned char cResultTerrain = iCurrentTerraType;

				// TODO:::
				pBoob->biome = cResultBiome;

				currentBlock.b16index = i;
				currentBlock.b8index = j;
				currentBlock.b1index = k;

				// == Application ==
				// Apply the biome type
				switch ( cResultBiome )
				{
				case BIO_DESERT: // temp for now
				case BIO_GRASS_DESERT: // This is typically the badlands
					cResultBlock = TerraGen_3p_GrassDesert( pBoob, blockPosition, pBoob->data[i].data[j].data[k].block, topVis );
					break;
				case BIO_GRASSLAND:
				case BIO_SAVANNA:
					cResultBlock = TerraGen_3p_Grassland( pBoob, blockPosition, pBoob->data[i].data[j].data[k].block, topVis );
					break;
				case BIO_SWAMP:
				case BIO_MOIST:
					cResultBlock = TerraGen_3p_Swamp( pBoob, blockPosition, pBoob->data[i].data[j].data[k].block, topVis );
					break;
				case BIO_TAIGA:
					cResultBlock = TerraGen_3p_Taiga( pBoob, blockPosition, pBoob->data[i].data[j].data[k].block, topVis, botVis );
					break;
				case BIO_TUNDRA:
					cResultBlock = TerraGen_3p_Tundra( pBoob, blockPosition, pBoob->data[i].data[j].data[k].block, topVis, botVis );
					break;
				case BIO_S_TUNDRA:
					cResultBlock = TerraGen_3p_SuperTundra( pBoob, blockPosition, pBoob->data[i].data[j].data[k].block, topVis, botVis );
					break;
				case BIO_OUTLANDS:
				case BIO_WET_OUTLANDS:
					cResultBlock = TerraGen_3p_Default( pBoob, blockPosition, pBoob->data[i].data[j].data[k].block, topVis );
					break;
				default: // Default generation
					cResultBlock = TerraGen_3p_Default( pBoob, blockPosition, pBoob->data[i].data[j].data[k].block, topVis );
					break;
				}


				pBoob->data[i].data[j].data[k].block = cResultBlock;
				
			}
		}
	}
}

// ===================
// == Biome utility ==
// ===================
bool CTerraGenDefault::TerraGen_3p_CanMakeTree ( const Vector3d & blockPosition, ftype density )
{
	ftype divSize = (1/32.0f)/density;
	Vector3d gridOffset (
		fmodf( fabs(blockPosition.x), divSize ) - divSize/2,
		fmodf( fabs(blockPosition.y), divSize ) - divSize/2,
		fmodf( fabs(blockPosition.z*2.0f), divSize ) - divSize/2
		);
	gridOffset.x += noise_hhf->Get3D( gridOffset.x,blockPosition.x*4.2f,blockPosition.z*3.4f )*divSize*0.632f*0.5f;
	gridOffset.y += noise_hhf->Get3D( blockPosition.y*4.3f,gridOffset.y,blockPosition.z*3.3f )*divSize*0.632f*0.5f;
	gridOffset.z += noise_hhf->Get3D( blockPosition.z*4.4f,blockPosition.y*3.2f,gridOffset.z )*divSize*0.632f*0.5f;
	gridOffset *= 32;
	if ( gridOffset.sqrMagnitude() < sqr(0.632f) ) {
		return true;
	}
	return false;
}
// =================
// == Biome types ==
// =================
ushort CTerraGenDefault::TerraGen_3p_Default ( CBoob * pBoob, Vector3d const& blockPosition, ushort inBlock, bool topOpen )
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

			val *= 1.7f;
			//if ( val > 0.6f )
			if (( val > 0.1f )&&( TerraGen_3p_CanMakeTree( blockPosition,0.25f ) ))
			{
				if ( val < 0.94f )
					TerraGen_NewFoliage( pBoob, blockPosition * 64.0f + Vector3d( 1,1,2 ), "CTreeBase" );
				else
					TerraGen_NewFoliage( pBoob, blockPosition * 64.0f + Vector3d( 1,1,2 ), "FoliageTreePine" );
			}
			else if ( (val < -0.1) && TerraGen_3p_CanMakeTree( blockPosition,0.17f ) ) {
				if ( TerraGen_GetComponentCount( pBoob, "PropFloraLifedrop" ) < 2 ) {
					TerraGen_NewComponent( pBoob, currentBlock, blockPosition * 64.0f + Vector3d( 1,1,2 ), "PropFloraLifedrop" );
				}
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
				TerraGen_NewGrass( pBoob, grassPos, EG_DEFAULT );
			}

			return EB_GRASS;
		}
	}
	// Return the input block (no change)
	return inBlock;
}
ushort CTerraGenDefault::TerraGen_3p_Swamp ( CBoob * pBoob, Vector3d const& blockPosition, ushort inBlock, bool topOpen )
{
	static ftype val;
	static ftype val2;
	// Check if we can generate vegetation
	if ( inBlock == EB_DIRT )
	{
		val = noise->Get3D( blockPosition.x*1.4f,blockPosition.y*1.4f,blockPosition.z*1.4f );
		if ( val > 0.1f )
		{
			return EB_MUD;
		}
		else if (( topOpen )&&( cLastCheckedBlock != EB_WATER ))
		{
			val = noise_hf->Get3D(blockPosition.z,blockPosition.x,blockPosition.y);
			//if ( val < blockPosition.z*2.4f+0.3f )
			if ( val < blockPosition.z*1.2f )
			{
				// Generate grass and trees
				val2 = noise_hhf->Get3D(blockPosition.x*10.4f,blockPosition.y*10.4f,blockPosition.z*10.4f) * 1.8f;
				//val = noise_hf->Get3D(blockPosition.z,blockPosition.x,blockPosition.y)*0.9f + val2*val2*val2;
				val = noise_hf->Get3D(blockPosition.z,blockPosition.x,blockPosition.y)*2.0f;

				val *= 1.7f;
				val2 *= 1.7f;
				if ( val > 0.75f ) {
					if ( TerraGen_3p_CanMakeTree( blockPosition,0.4f ) ) {
					//if ( val2 > 0.82f ) {
						TerraGen_NewFoliage( pBoob, blockPosition * 64.0f + Vector3d( 1,1,2 ), "CTreeBase" );
					//}
					}
				}
				else {
					if ( val2 > 0.79f ) {
						TerraGen_NewFoliage( pBoob, blockPosition * 64.0f + Vector3d( 1,1,2 ), "FoliageBushGreen" );
					}
					else if ( (val2 < -0.3) && TerraGen_3p_CanMakeTree( blockPosition,0.4f ) ) {
						if ( TerraGen_GetComponentCount( pBoob, "PropFloraLifedrop" ) < 4 ) {
							TerraGen_NewComponent( pBoob, currentBlock, blockPosition * 64.0f + Vector3d( 1,1,2 ), "PropFloraLifedrop" );
						}
					}
				}

				// Create some grass for this block
				for ( float fi = 0.1f; fi < val+0.05; fi += 1.3f )
				{
					Vector3d grassPos = blockPosition*64.0f + Vector3d(
						1+noise_hhf->Get3D(blockPosition.x*10.4f+fi*1.7f,blockPosition.y*10.4f,blockPosition.z*10.4f+fi*3.2f)*2,
						1+noise_hhf->Get3D(blockPosition.y*10.4f+fi*4.1f,blockPosition.z*10.4f+fi*6.5f,blockPosition.x*10.4f)*2,
						2 );
					if ( noise->Get3D( blockPosition.x*3.2f,blockPosition.y*3.2f,blockPosition.z*3.2f ) > -0.1f )
						TerraGen_NewGrass( pBoob, grassPos, EG_DEFAULT );
					else
						TerraGen_NewGrass( pBoob, grassPos, EG_TALL );

				}

				return EB_GRASS;
			}
		}
	}
	else if ( inBlock == EB_SAND ) {
		return EB_MUD;
	}
	// Return the input block (no change)
	return inBlock;
}

ushort CTerraGenDefault::TerraGen_3p_Grassland ( CBoob * pBoob, Vector3d const& blockPosition, ushort inBlock, bool topOpen )
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

			val *= 1.7f;
			val2 *= 1.7f;
			if ( val > 0.75f )
			{
				//if ( val2 > 0.82f )
				if ( (val2 > 0.1f) && TerraGen_3p_CanMakeTree( blockPosition,0.34f ) ) {
					TerraGen_NewFoliage( pBoob, blockPosition * 64.0f + Vector3d( 1,1,2 ), "CTreeBase" );
				}
			}
			else
			{
				if ( val2 > 0.79f ) {
					TerraGen_NewFoliage( pBoob, blockPosition * 64.0f + Vector3d( 1,1,2 ), "FoliageBushGreen" );
				}
				else if ( (val2 < -0.1f) && TerraGen_3p_CanMakeTree( blockPosition,0.15f ) ) {
					if ( TerraGen_GetComponentCount( pBoob, "PropFloraLifedrop" ) < 2 ) {
						TerraGen_NewComponent( pBoob, currentBlock, blockPosition * 64.0f + Vector3d( 1,1,2 ), "PropFloraLifedrop" );
					}
				}
			}

			// Create some grass for this block
			for ( float fi = 0.1f; fi < val+0.05; fi += 1.3f )
			{
				Vector3d grassPos = blockPosition*64.0f + Vector3d(
					1+noise_hhf->Get3D(blockPosition.x*10.4f+fi*1.7f,blockPosition.y*10.4f,blockPosition.z*10.4f+fi*3.2f)*2,
					1+noise_hhf->Get3D(blockPosition.y*10.4f+fi*4.1f,blockPosition.z*10.4f+fi*6.5f,blockPosition.x*10.4f)*2,
					2 );
				TerraGen_NewGrass( pBoob, grassPos, EG_DEFAULT );
			}

			return EB_GRASS;
		}
	}
	// Return the input block (no change)
	return inBlock;
}

ushort CTerraGenDefault::TerraGen_3p_GrassDesert	( CBoob * pBoob, Vector3d const& blockPosition, ushort inBlock, bool topOpen )
{
	static ftype val;
	static ftype val2;
	// Check if we can generate vegetation
	if (( inBlock == EB_DIRT )&&( topOpen )&&( cLastCheckedBlock != EB_WATER ))
	{
		val = noise->Get3D(blockPosition.z,blockPosition.x,blockPosition.y);
		val *= 1.7f;
		//if ( val < blockPosition.z*2.4f-0.4f )
		if ( val > 0.32f )
		{
			// Generate grass and trees
			val2 = noise_hhf->Get3D(blockPosition.x,blockPosition.y,blockPosition.z) * 1.8f;
			val = noise_hf->Get3D(blockPosition.z,blockPosition.x,blockPosition.y)*0.9f + val2*val2*val2;

			val *= 1.7f;
			if ( val > 0.7f ) {
				TerraGen_NewFoliage( pBoob, blockPosition * 64.0f + Vector3d( 1,1,2 ), "FoliageBushGreen" );
			}
			return EB_GRASS;
		}
		else
		{
			//if ( val < -0.45f ) {
			if ( (val < -0.1f) && TerraGen_3p_CanMakeTree( blockPosition,0.1f ) ) {
				TerraGen_NewFoliage( pBoob, blockPosition * 64.0f + Vector3d( 1,1,2 ), "FoliageCactusDefault" );
			}
		}
	}
	if (( inBlock == EB_SAND )&&( topOpen )&&( cLastCheckedBlock != EB_WATER ))
	{
		val = noise->Get3D(blockPosition.z,blockPosition.x,blockPosition.y);
		val *= 1.7f;
		if ( val > 0.32f )
		{
			// Generate grass and trees
			val2 = noise_hhf->Get3D(blockPosition.x,blockPosition.y,blockPosition.z) * 1.8f;
			val = noise_hf->Get3D(blockPosition.z,blockPosition.x,blockPosition.y)*0.9f + val2*val2*val2;

			val *= 2.7f;

			if ( val > 0.7f ) {
				TerraGen_NewFoliage( pBoob, blockPosition * 64.0f + Vector3d( 1,1,2 ), "FoliageCactusDefault" );
			}
		}
	}
	// Return the input block (no change)
	return inBlock;
}

ushort CTerraGenDefault::TerraGen_3p_Taiga	( CBoob * pBoob, Vector3d const& blockPosition, ushort inBlock, bool topOpen, bool botOpen )
{
	static ftype val;
	static ftype val2;
	// Check if we can generate vegetation
	if (( inBlock == EB_DIRT )&&( topOpen )&&( cLastCheckedBlock != EB_WATER ))
	{
		val = noise_hf->Get3D(blockPosition.z,blockPosition.x,blockPosition.y);
		if ( val < blockPosition.z*2.7f+0.3f )
		{
			// Generate grass and trees
			val2 = noise_hhf->Get3D(blockPosition.x*3.4f,blockPosition.y*3.4f,blockPosition.z*0.4f) * 3.6f;
			val = noise_hf->Get3D(blockPosition.z,blockPosition.x,blockPosition.y)*0.9f + val2*val2*val2;
			val += std::max<ftype>( noise->Get3D(blockPosition.z,blockPosition.x,blockPosition.y)*1.2f, -0.1f );

			//if ( val > 0.48f )
			if ( (val > 0.08f) && TerraGen_3p_CanMakeTree( blockPosition,0.35f ) )
			{
				//if ( val < 0.94f )
				//	TerraGen_NewFoliage( pBoob, blockPosition * 64.0f + Vector3d( 1,1,2 ), "CTreeBase" );
				//else
					TerraGen_NewFoliage( pBoob, blockPosition * 64.0f + Vector3d( 1,1,2 ), "FoliageTreePine" );
			}

			// Create some grass for this block
			for ( float fi = 0.1f; fi < val-0.07f; fi += 1.3f )
			{
				Vector3d grassPos = blockPosition*64.0f + Vector3d(
					1+noise_hhf->Get3D(blockPosition.x*10.4f+fi*1.7f,blockPosition.y*10.4f,blockPosition.z*10.4f+fi*3.2f)*2,
					1+noise_hhf->Get3D(blockPosition.y*10.4f+fi*4.1f,blockPosition.z*10.4f+fi*6.5f,blockPosition.x*10.4f)*2,
					2 );
				TerraGen_NewGrass( pBoob, grassPos, EG_FROSTLEAF );
			}

			return EB_GRASS;
		}
	}
	else if (( inBlock == EB_NONE )&&( !botOpen )) {
		// Check block below
		//if (( cBottomCheckBlock == EB_DIRT )||( cBottomCheckBlock == EB_GRASS )) {
		if (( cBottomCheckBlock != EB_NONE )&&( cBottomCheckBlock != EB_WATER )&&( cBottomCheckBlock != EB_SNOW )) {
			val = noise_hf->Get3D(blockPosition.z,blockPosition.x,blockPosition.y);
			if ( val < blockPosition.z*2.7f+0.3f )
			{
				// Generate snow
				val2 = noise_hhf->Get3D(blockPosition.x*3.4f,blockPosition.y*3.4f,blockPosition.z*0.4f) * 3.6f;
				val = noise_hf->Get3D(blockPosition.z,blockPosition.x,blockPosition.y)*0.9f + val2*val2*val2;
				val += noise->Get3D(blockPosition.z,blockPosition.x,blockPosition.y)*1.2f;

				if ( val < -0.1f ) {
					return EB_SNOW;
				}
			}
		}
	}
	else if ( inBlock == EB_WATER ) {
		if (( true||topOpen )&&( cLastCheckedBlock != EB_WATER )&&( cLastCheckedBlock != EB_ICE )) {
			// Generate ice
			val2 = noise_hhf->Get3D(blockPosition.x,blockPosition.y,blockPosition.z) * 1.8f;
			val = noise_hf->Get3D(blockPosition.z,blockPosition.x,blockPosition.y)*0.9f + val2*val2*val2;

			if ( val < 0 ) {
				return EB_ICE;
			}
		}
	}
	return inBlock;
}

ushort CTerraGenDefault::TerraGen_3p_Tundra	( CBoob * pBoob, Vector3d const& blockPosition, ushort inBlock, bool topOpen, bool botOpen )
{
	static ftype val;
	static ftype val2;
	// Check if we can generate vegetation
	if (( inBlock == EB_DIRT )&&( topOpen )&&( cLastCheckedBlock != EB_WATER ))
	{
		val = noise_hf->Get3D(blockPosition.z,blockPosition.x,blockPosition.y);
		if ( val < blockPosition.z*2.7f+0.3f )
		{
			// Generate grass and trees
			val2 = noise_hhf->Get3D(blockPosition.x*10.4f,blockPosition.y*10.4f,blockPosition.z*10.4f) * 1.8f;
			val = noise_hf->Get3D(blockPosition.z,blockPosition.x,blockPosition.y)*2.0f;

			val *= 1.7f;
			val2 *= 1.7f;
			if ( val > 0.75f ) {
				//if ( val2 > 0.82f ) {
				if ( ( val2 > 0.4f ) && TerraGen_3p_CanMakeTree( blockPosition,0.14f ) ) {
					TerraGen_NewFoliage( pBoob, blockPosition * 64.0f + Vector3d( 1,1,2 ), "FoliageTreePine" );
				}
			}
			else {
				if ( val2 > 0.79f ) {
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
				TerraGen_NewGrass( pBoob, grassPos, EG_FROSTLEAF );
			}

			return EB_GRASS;
		}
	}
	else if (( inBlock == EB_NONE )&&( !botOpen )) {
		// Check block below
		//if (( cBottomCheckBlock == EB_DIRT )||( cBottomCheckBlock == EB_GRASS )) {
		if (( cBottomCheckBlock != EB_NONE )&&( cBottomCheckBlock != EB_WATER )&&( cBottomCheckBlock != EB_SNOW )) {
			val = noise_hf->Get3D(blockPosition.z,blockPosition.x,blockPosition.y);
			if ( val < blockPosition.z*2.7f+0.3f )
			{
				// Generate snow
				val2 = noise_hhf->Get3D(blockPosition.x,blockPosition.y,blockPosition.z) * 1.8f;
				val = noise_hf->Get3D(blockPosition.z,blockPosition.x,blockPosition.y)*0.9f + val2*val2*val2;

				if ( cBottomCheckBlock == EB_ICE ) {
					if ( val < -0.1f ) {
						return EB_SNOW;
					}
				}
				else {
					if ( val < 0.0f ) {
						return EB_SNOW;
					}
				}
			}
		}
	}
	else if ( inBlock == EB_WATER ) {
		if (( true||topOpen )&&( cLastCheckedBlock != EB_WATER )&&( cLastCheckedBlock != EB_ICE )) {
			// Generate ice
			val2 = noise_hhf->Get3D(blockPosition.x,blockPosition.y,blockPosition.z) * 1.8f;
			val = noise_hf->Get3D(blockPosition.z,blockPosition.x,blockPosition.y)*0.9f + val2*val2*val2;

			if ( val < 0.1f ) {
				return EB_ICE;
			}
		}
	}
	return inBlock;
}

ushort CTerraGenDefault::TerraGen_3p_SuperTundra	( CBoob * pBoob, Vector3d const& blockPosition, ushort inBlock, bool topOpen, bool botOpen )
{
	static ftype val;
	static ftype val2;
	if (( inBlock == EB_DIRT )&&( topOpen )&&( cLastCheckedBlock != EB_WATER ))
	{
		val = noise->Get3D(blockPosition.z,blockPosition.x,blockPosition.y);
		if ( val > 0.14f )
		{
			return EB_GRASS;
		}
	}
	else if (( inBlock == EB_NONE )&&( !botOpen )) {
		if (( cBottomCheckBlock != EB_NONE )&&( cBottomCheckBlock != EB_WATER )&&( cBottomCheckBlock != EB_SNOW )) {
			if ( cBottomCheckBlock == EB_ICE ) {
				// Generate ice
				val2 = noise_hhf->Get3D(blockPosition.x,blockPosition.y,blockPosition.z) * 1.8f;
				val = noise_hf->Get3D(blockPosition.z,blockPosition.x,blockPosition.y)*0.9f + val2*val2*val2;
				if ( val < 0.2f ) {
					return EB_SNOW;
				}
			}
			else {
				if ( val > 0.268f ) {
					if ( TerraGen_GetComponentCount( pBoob, "PropFloraSnowLily" ) < 2 ) {
						TerraGen_NewComponent( pBoob, currentBlock, blockPosition * 64.0f + Vector3d( 1,1,3 ), "PropFloraSnowLily" );
					}
				}

				return EB_SNOW;
			}
		}
	}
	else if ( inBlock == EB_WATER ) {
		if (( true||topOpen )&&( cLastCheckedBlock != EB_WATER )&&( cLastCheckedBlock != EB_ICE )) {
			return EB_ICE;
		}
	}
	return inBlock;
}