
// ======== CTerraGenDefault_Samplers =======
// This contains the definition of all the samplers code.
// This includes the sampler initialization, and the priv/pub terrain and biome sampling.

#include "CTerraGenDefault.h"

#include "Perlin.h"
#include "SimplexNoise.h"
#include "Math.h"

using std::iostream;
using std::ifstream;
using std::cout;
using std::endl;
using std::max;
using std::min;

// Grab samplers from file
void CTerraGenDefault::TerraGenGetSamplers ( void ) // should this be moved to the IO file?
{
	if ( !bHasSamplers )
	{
		bHasSamplers = true;

		if ( pSamplerTerrainType == NULL )
		{
			pSamplerTerrainType = new unsigned char [64*64];
			// Read in the terrain sampler
			{
				string sInputFile ( ".res\\terra\\terrainSampler.pgm" );
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
				string sInputFile ( ".res\\terra\\biomeSampler.pgm" );
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

	if ( noise == NULL )
	{
		noise		= new Perlin( 1,2.03f,1,iTerrainSeed );
		noise_hf	= new Perlin( 1,5.07f,1,iTerrainSeed );
		noise_hhf	= new Perlin( 1,15.17f,1,iTerrainSeed+1 );
		noise_lf	= new Perlin( 1,0.53f,1,iTerrainSeed-1 );
		noise_terra	= new Perlin( 2,0.032f*1.3f*0.7f,1,iTerrainSeed+1 );
		noise_biome	= new Perlin( 2,0.043f*1.6f,1,iTerrainSeed-1 );

		/*ofstream out1( ".res/test1.txt" );
		for ( ftype x = -1000; x < 1000; x += 0.23f*1.4f*0.97f*13.0f ) {
			for ( ftype y = -1000; y < 1000; y += 0.23f*1.4f*0.97f*13.0f ) {
				for ( ftype z = -1000; z < 1000; z += 0.23f*1.4f*0.97f*13.0f ) {
					out1 << noise->Get3D(x,y,z) << " ";
				}
				out1 << endl;
			}
		}*/

		//vMainSamplerOffset = Vector3d( 0,0,0 );
		vMainSamplerOffset.x = (noise_terra->Get( 0.4f,0.3f ) * 0.13f * 207.3f);
		vMainSamplerOffset.y = (noise_terra->Get( 1.7f,2.7f ) * 0.17f * 207.3f);
		vMainSamplerOffset.z = (noise_terra->Get( 4.4f,2.3f ) * 0.23f * 207.3f);

		uint32_t *v = reinterpret_cast<uint32_t *>(&vMainSamplerOffset.x);
		v[0] &= iTerrainSeed;
		v[1] &= iTerrainSeed;
		v[2] &= iTerrainSeed;

		vMainSamplerOffset.x = 4.1221412214f*(iTerrainSeed%42) + vMainSamplerOffset.x*412.21412214f;
		vMainSamplerOffset.y = 6.1331613316f*(iTerrainSeed%42) + vMainSamplerOffset.y*613.31613316f;
		vMainSamplerOffset.z = 8.1441814418f*(iTerrainSeed%42) + vMainSamplerOffset.z*814.41814418f;

		vMainSamplerOffset.x = fmod( vMainSamplerOffset.x, 100 );
		vMainSamplerOffset.y = fmod( vMainSamplerOffset.y, 100 );
		vMainSamplerOffset.z = fmod( vMainSamplerOffset.z, 100 );

	}

	if ( noise_pub == NULL )
	{
		noise_pub		= new Perlin( 1,2.03f,1,iTerrainSeed );
		noise_pub_hf	= new Perlin( 1,5.07f,1,iTerrainSeed );
		noise_pub_hhf	= new Perlin( 1,15.17f,1,iTerrainSeed+1 );
		noise_pub_lf	= new Perlin( 1,0.53f,1,iTerrainSeed-1 );
		noise_pub_terra	= new Perlin( 2,0.032f*1.3f*0.7f,1,iTerrainSeed+1 );
		noise_pub_biome	= new Perlin( 2,0.043f*1.6f,1,iTerrainSeed-1 );
	}
}

ftype	CTerraGenDefault::Trd_Noise_HF ( const ftype x, const ftype y, const ftype z )
{
	return noise_hf->Get3D( x,y,z );
}
ftype	CTerraGenDefault::Trd_Noise_Sample ( const ftype x, const ftype y, const ftype z )
{
	return noise_hhf->Get3D( x,y,z );
}

// Type grabber
char	CTerraGenDefault::TerraGen_priv_GetType	( Vector3d const& blockPosition )
{
	TerraGenGetSamplers();

	// Generate terrain type
	static ftype fTempOffs;
	static ftype fRockiness;
	static ftype fUnpredictability;
	static unsigned char cResultTerrain;
	static unsigned char cResultBlock;
	// Input functions for rockiness and unpredictability
	fRockiness			= noise_terra->Unnormalize( noise_terra->Get3D(vMainSamplerOffset.x + blockPosition.x*1.12f,vMainSamplerOffset.y + blockPosition.y*1.12f,0) ) + 0.5f;
	fUnpredictability	= noise_terra->Unnormalize( noise_terra->Get3D(vMainSamplerOffset.y + blockPosition.y*0.7f,vMainSamplerOffset.x + blockPosition.x*0.7f,0) ) + 0.5f;
	// Add another layer of noise to the sample values
	fTempOffs = noise->Get3D( blockPosition.x*1.6f,blockPosition.y*1.6f,blockPosition.z*1.9f )*0.05f;
	fRockiness			= max( 0.0f, min( 1.0f, fRockiness+fTempOffs ) );
	fTempOffs = noise->Get3D( blockPosition.y*1.6f,blockPosition.z*1.9f,blockPosition.x*1.6f )*0.05f;
	fUnpredictability	= max( 0.0f, min( 1.0f, fUnpredictability+fTempOffs ) );
	// Get the terrain type
	cResultTerrain = pSamplerTerrainType[ (int(fUnpredictability*63.8f)) + (int(fRockiness*63.8f)*64) ];
	iCurrentTerraType = cResultTerrain;

	fCurrentRockiness			= fRockiness;
	fCurrentUnpredictability	= fUnpredictability;

	return cResultTerrain;
}
// Elevation grabber
ftype CTerraGenDefault::TerraGen_1p_GetElevation ( RangeVector const& position )
{
	return TerraGen_1p_GetElevation( Vector3d( (ftype)position.x+0.5f, (ftype)position.y+0.5f, (ftype)position.z+0.5f ) );
}
ftype CTerraGenDefault::TerraGen_1p_GetElevation ( Vector3d const& blockPosition )
{
	static ftype seedElevation;
	static ftype detailNoise;
	static ftype subNoise;
	static ftype finalElevation;
	static unsigned char terra;

	terra = TerraGen_priv_GetType( blockPosition );
	//seedElevation = noise_terra->Get3D(blockPosition.y*0.23f,blockPosition.x*0.23f,0.0f) - 0.18f; // [-0.68 to 0.32] (68% of land below sea level)
	//finalElevation = sqr(seedElevation) *  4.4f; // [0 to 0.45]U[0 to 2.03]
		
	detailNoise = noise_terra->Get3D(blockPosition.x*1.7f,blockPosition.y*1.7f,0.0f); // [-0.5 to 0.5]
	subNoise = noise_terra->Get3D(-blockPosition.y*0.8f,-blockPosition.x*0.8f,0.0f); // [-0.5 to 0.5]

	seedElevation = noise_terra->Get3D(blockPosition.y*0.23f,blockPosition.x*0.23f,0.0f) - 0.10f; // [-0.60 to 0.40]
	finalElevation = sqr(seedElevation) * 14.8f * Math.sgn<ftype>(seedElevation); // [-5.328 to 2.368] (69.2% of land below sea level)
	// Make below sea level not as steep
	if ( finalElevation < 0.0f ) {
		finalElevation *= 0.3f;
	}

	switch ( terra ) {
		case TER_OCEAN:
		case TER_SPIRES:
			//finalElevation -= 1.18f; // [-6.508 to 1.188]
			finalElevation += 0.13f;
			if ( finalElevation > 0 ) {
				finalElevation *= 0.2f;
				if ( finalElevation > 0.2f ) {
					//finalElevation -= 1.18f;
					finalElevation -= (finalElevation-0.2f)*4.0f;
				}
			}
			//finalElevation -= 0.59f;
			finalElevation += ( detailNoise - 0.12f ) * 1.2f; // +[-0.744 to 0.456] = [-7.252 to 2.762]
			finalElevation -= ( subNoise + 0.25f ) * 0.8f; // -[-0.2 to 0.6] = [-7.852 to 2.962] 
			break;
		case TER_OUTLANDS:
		case TER_MOUNTAINS:
			if ( finalElevation > 1.2f ) {
				// From 1.2 and beyond, increase by 0.8f
				finalElevation += std::min<ftype>( finalElevation-1.2f, 0.8f );

				finalElevation = sqr(finalElevation-1.2f)*1.6f+1.2f; // [-5.328 to 1.2]U[1.2 to 3.383]
			}
			finalElevation += ( detailNoise + 0.12f ) * 2.2f; // +[-0.836 to 1.364] = [-6.164 to 4.747]
			finalElevation += ( subNoise + 0.12f ) * 1.4f; // +[-0.532 to 0.868] = [6.696 to 5.615]
			break;
		default:
			finalElevation += ( detailNoise + 0.18f ) * 1.2f; // +[-0.384 to 0.816] = [-5.712 to 3.184]
			finalElevation += ( subNoise + 0.18f ) * 0.8f; // +[-0.256 to 0.544] = [-5.968 to 3.728]
			break;
	}

	// Increase elevation at the border of deserts
	if ( terra == TER_OCEAN || terra == TER_DESERT ) {
		detailNoise = noise_terra->Get3D(blockPosition.y*0.7f,blockPosition.x*0.7f,0.0f) + 0.5f; 
		finalElevation += max<ftype>( 0, min<ftype>( 1, 1-(abs( detailNoise-0.75f )/0.12f) ) ) * max<ftype>( 0, -finalElevation*1.2f );
	}

	return finalElevation;
}

// Biome grabber
char	CTerraGenDefault::TerraGen_priv_GetBiome	( Vector3d const& blockPosition )
{
	TerraGenGetSamplers();

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
	//cResultTerrain = TerraGen_priv_GetType( blockPosition );
	// Get elevation
	fCurrentElevation = TerraGen_1p_GetElevation( blockPosition );
	cResultTerrain = iCurrentTerraType;
	// Get Z sampler
	fCurrentSamplerZ = max( min( fCurrentElevation, blockPosition.z ), fCurrentElevation-0.5f );
	// Input functions for temperature, humidity, and density
	fTemperature	= noise_biome->Unnormalize(noise_biome->Get3D(
		vMainSamplerOffset.x + blockPosition.x*1.6f,
		vMainSamplerOffset.y + blockPosition.y*1.6f,
		vMainSamplerOffset.z + fCurrentSamplerZ)) + 0.5f;
	fHumidity		= noise_biome->Get3D(
		vMainSamplerOffset.y + blockPosition.y,
		vMainSamplerOffset.x + blockPosition.x,
		vMainSamplerOffset.z + fCurrentSamplerZ) + 0.5f;
	fDensity		= noise_biome->Unnormalize(noise_biome->Get3D(
		vMainSamplerOffset.y -blockPosition.y*1.2f,
		vMainSamplerOffset.x -blockPosition.x*1.2f,
		vMainSamplerOffset.z + fCurrentSamplerZ )) + 0.5f;
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
	tempVal = std::max<ftype>( tempVal, 0 );
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



// ======= PUBLIC GRABBERS =======
char	CTerraGenDefault::TerraGen_pub_GetType	( Vector3d const& blockPosition )
{
	TerraGenGetSamplers();
	
	// Generate terrain type
	static ftype fTempOffs;
	static ftype fRockiness;
	static ftype fUnpredictability;
	static unsigned char cResultTerrain;
	static unsigned char cResultBlock;
	// Input functions for rockiness and unpredictability
	fRockiness			= noise_pub_terra->Unnormalize( noise_pub_terra->Get3D(vMainSamplerOffset.x + blockPosition.x*1.12f,vMainSamplerOffset.y + blockPosition.y*1.12f,0) ) + 0.5f;
	fUnpredictability	= noise_pub_terra->Unnormalize( noise_pub_terra->Get3D(vMainSamplerOffset.y + blockPosition.y*0.7f,vMainSamplerOffset.x + blockPosition.x*0.7f,0) ) + 0.5f;
	// Add another layer of noise to the sample values
	fTempOffs = noise_pub->Get3D( blockPosition.x*1.6f,blockPosition.y*1.6f,blockPosition.z*1.9f )*0.05f;
	fRockiness			= max( 0.0f, min( 1.0f, fRockiness+fTempOffs ) );
	fTempOffs = noise_pub->Get3D( blockPosition.y*1.6f,blockPosition.z*1.9f,blockPosition.x*1.6f )*0.05f;
	fUnpredictability	= max( 0.0f, min( 1.0f, fUnpredictability+fTempOffs ) );
	// Get the terrain type
	cResultTerrain = pSamplerTerrainType[ (int(fUnpredictability*63.8f)) + (int(fRockiness*63.8f)*64) ];

	return cResultTerrain;
}
ftype	CTerraGenDefault::TerraGen_pub_GetElevation( Vector3d const& blockPosition )
{
	static ftype seedElevation;
	static ftype detailNoise;
	static ftype subNoise;
	static ftype finalElevation;
	static unsigned char terra;

	terra = TerraGen_pub_GetType( blockPosition );
	//seedElevation = noise_terra->Get3D(blockPosition.y*0.23f,blockPosition.x*0.23f,0.0f) - 0.18f; // [-0.68 to 0.32] (68% of land below sea level)
	//finalElevation = sqr(seedElevation) *  4.4f; // [0 to 0.45]U[0 to 2.03]
		
	detailNoise = noise_pub_terra->Get3D(blockPosition.x*1.7f,blockPosition.y*1.7f,0.0f); // [-0.5 to 0.5]
	subNoise = noise_pub_terra->Get3D(-blockPosition.y*0.8f,-blockPosition.x*0.8f,0.0f); // [-0.5 to 0.5]

	seedElevation = noise_pub_terra->Get3D(blockPosition.y*0.23f,blockPosition.x*0.23f,0.0f) - 0.10f; // [-0.60 to 0.40]
	finalElevation = sqr(seedElevation) * 14.8f * Math.sgn<ftype>(seedElevation); // [-5.328 to 2.368] (69.2% of land below sea level)
	// Make below sea level not as steep
	if ( finalElevation < 0.0f ) {
		finalElevation *= 0.3f;
	}

	switch ( terra ) {
		case TER_OCEAN:
		case TER_SPIRES:
			//finalElevation -= 1.18f; // [-6.508 to 1.188]
			finalElevation += 0.13f;
			if ( finalElevation > 0 ) {
				finalElevation *= 0.2f;
				if ( finalElevation > 0.2f ) {
					//finalElevation -= 1.18f;
					finalElevation -= (finalElevation-0.2f)*4.0f;
				}
			}
			//finalElevation -= 0.59f;
			finalElevation += ( detailNoise - 0.12f ) * 1.2f; // +[-0.744 to 0.456] = [-7.252 to 2.762]
			finalElevation -= ( subNoise + 0.25f ) * 0.8f; // -[-0.2 to 0.6] = [-7.852 to 2.962] 
			break;
		case TER_OUTLANDS:
		case TER_MOUNTAINS:
			if ( finalElevation > 1.2f ) {
				// From 1.2 and beyond, increase by 0.8f
				finalElevation += std::min<ftype>( finalElevation-1.2f, 0.8f );

				finalElevation = sqr(finalElevation-1.2f)*1.6f+1.2f; // [-5.328 to 1.2]U[1.2 to 3.383]
			}
			finalElevation += ( detailNoise + 0.12f ) * 2.2f; // +[-0.836 to 1.364] = [-6.164 to 4.747]
			finalElevation += ( subNoise + 0.12f ) * 1.4f; // +[-0.532 to 0.868] = [6.696 to 5.615]
			break;
		default:
			finalElevation += ( detailNoise + 0.18f ) * 1.2f; // +[-0.384 to 0.816] = [-5.712 to 3.184]
			finalElevation += ( subNoise + 0.18f ) * 0.8f; // +[-0.256 to 0.544] = [-5.968 to 3.728]
			break;
	}

	// Increase elevation at the border of deserts
	if ( terra == TER_OCEAN || terra == TER_DESERT ) {
		detailNoise = noise_pub_terra->Get3D(blockPosition.y*0.7f,blockPosition.x*0.7f,0.0f) + 0.5f; 
		finalElevation += max<ftype>( 0, min<ftype>( 1, 1-(abs( detailNoise-0.75f )/0.12f) ) ) * max<ftype>( 0, -finalElevation*1.2f );
	}

	return finalElevation;
}
char	CTerraGenDefault::TerraGen_pub_GetBiome	( Vector3d const& blockPosition )
{
	TerraGenGetSamplers();

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
	// Input functions for temperature, humidity, and density
	fTemperature	= noise_pub_biome->Unnormalize(noise_pub_biome->Get3D(
		vMainSamplerOffset.x + blockPosition.x*1.6f,
		vMainSamplerOffset.y + blockPosition.y*1.6f,
		vMainSamplerOffset.z + fCurrentSamplerZ)) + 0.5f;
	fHumidity		= noise_pub_biome->Get3D(
		vMainSamplerOffset.y + blockPosition.y,
		vMainSamplerOffset.x + blockPosition.x,
		vMainSamplerOffset.z + fCurrentSamplerZ) + 0.5f;
	fDensity		= noise_pub_biome->Unnormalize(noise_pub_biome->Get3D(
		vMainSamplerOffset.y -blockPosition.y*1.2f,
		vMainSamplerOffset.x -blockPosition.x*1.2f,
		vMainSamplerOffset.z + fCurrentSamplerZ )) + 0.5f;
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
	tempVal = std::max<ftype>( tempVal, 0 );
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