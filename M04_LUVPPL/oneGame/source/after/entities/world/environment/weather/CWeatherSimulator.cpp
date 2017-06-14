
// == Includes ==
#include "CWeatherSimulator.h"
#include "core/time/time.h"
#include "renderer/camera/CCamera.h"
#include "after/states/CWorldState.h"
#include "core/math/noise/Perlin.h"
#include "after/terrain/VoxelTerrain.h"
//#include "COctreeTerrain.h"

#include "after/terrain/Zones.h"

#include "CCloudMaster.h"
#include "CWeatherCloud.h"

#include "core-ext/threads/Jobs.h"

using std::cout;
using std::endl;
using std::max;
using std::min;

// == Static Variables
CWeatherSimulator*	CWeatherSimulator::pActiveSim = NULL;

// == Constructor/Destructor ==
CWeatherSimulator::CWeatherSimulator ( short xSize, short ySize, short zSize )
	: CGameBehavior ()
{
	// Stuff!
	size.x = xSize;
	size.y = ySize;
	size.z = zSize;
	size.max = size.x * size.y * size.z;

	// Create data
	data		= new sWeatherSimCell [size.max];
	data_temp	= new sWeatherSimCell [size.max];

	// Set root to zero
	root_position	= RangeVector( 0,0,0 );
	// Set stored camera position to zero
	vCameraPos		= Vector3d( 0,0,0 );

	// Init update state
	bNeedsUpdate = true;
	fUpdateTimer = 0.0f;

	// Set active instance
	if ( pActiveSim == NULL )
	{
		pActiveSim = this;
	}
	else
	{
		cout << "Warning: Two weather simulations have been created! Note that this will likely cause errors!" << endl;
		cout << "  (note weather has a SetActive() to toggle between multiple simulations)." << endl;
	}

	// Create the perlin source
	noise = new Perlin( 1,2,1, 0x15CC );

	// Init cloud system
	CCloudMaster::Init();
}

CWeatherSimulator::~CWeatherSimulator ( void )
{
	delete noise;

	delete [] data;
	data = NULL;
	delete [] data_temp;
	data_temp = NULL;

	// Release active
	if ( pActiveSim == this )
	{
		pActiveSim = NULL;
	}

	// Free cloud system
	CCloudMaster::Free();
}

// == Static Active ==
CWeatherSimulator* CWeatherSimulator::Get ( void )
{
	return pActiveSim;
}

// == Update ==
void CWeatherSimulator::Update ( void )
{
	UpdateRoot();
	if ( !bNeedsUpdate )
	{
		fUpdateTimer += Time::smoothDeltaTime;
		if ( fUpdateTimer > 0.1f )
		{
			bNeedsUpdate = true;
		} 
	}
	else
	{
		bNeedsUpdate = false;
		fUpdateTimer = 0;
		// TODO: Make threaded.
		/*
		
		*/
		/*static Jobs::result_t result;
		Jobs::System::Perform( &CWeatherSimulator::UpdateTick, this, &result );*/
		static std::thread* jobthread = NULL;

		if ( jobthread ) {
			jobthread->join();
			delete jobthread;
		}
		jobthread = new std::thread( &CWeatherSimulator::UpdateTick, this );
	}
}
// System Simulate
void CWeatherSimulator::UpdateTick ( void )
{
	GrabTerrainValues();
	GenerateHumidityValues();
	SimulateTemperatureValues();
	Simulate();
	GenerateClouds();
	SimulateWeather();
}

// Update the root
void CWeatherSimulator::UpdateRoot ( void )
{
//	if (( !CVoxelTerrain::terrainList.empty() )&&( CVoxelTerrain::terrainList[0] != NULL ))
//		SetRoot( CVoxelTerrain::terrainList[0]->GetRootPosition() );
	if ( CVoxelTerrain::GetActive() ) {
		SetRoot( CVoxelTerrain::GetActive()->GetCenterSector() );
	}
	/*else if ( COctreeTerrain::GetActive() ) {
	 	SetRoot( COctreeTerrain::GetActive()->GetStateWorldCenterIndex() );
	}*/
	else
	{
		// Do it manually. (TODO for SnS ep2)
	}
}

// Set the root position ( shifts the simulation )
void CWeatherSimulator::SetRoot ( const RangeVector& new_position )
{
	int x,y,z;
	// X Shift
	if ( root_position.x > new_position.x )
	{
		root_position.x -= 1;

		// Need to shift everything forward on the X
		for ( x = size.x - 1; x > 0; --x )
		{
			for ( y = 0; y < size.y; ++y )
			{
				for ( z = 0; z < size.z; ++z )
				{
					get( data, x,y,z ) = get( data, x-1,y,z );
				}
			}
		}
		// Clear last row
		for ( y = 0; y < size.y; ++y )
		{
			for ( z = 0; z < size.z; ++z )
			{
				get( data, 0,y,z ) = sWeatherSimCell();
			}
		}
	}
	else if ( root_position.x < new_position.x )
	{
		root_position.x += 1;

		// Need to shift everything back on the X
		for ( x = 0; x < size.x-1; ++x )
		{
			for ( y = 0; y < size.y; ++y )
			{
				for ( z = 0; z < size.z; ++z )
				{
					get( data, x,y,z ) = get( data, x+1,y,z );
				}
			}
		}
		// Clear last row
		for ( y = 0; y < size.y; ++y )
		{
			for ( z = 0; z < size.z; ++z )
			{
				get( data, size.x-1,y,z ) = sWeatherSimCell();
			}
		}
	}

	// Y Shift
	if ( root_position.y > new_position.y )
	{
		root_position.y -= 1;

		// Need to shift everything forward on the X
		for ( y = size.y - 1; y > 0; --y )
		{
			for ( x = 0; x < size.x; ++x )
			{
				for ( z = 0; z < size.z; ++z )
				{
					get( data, x,y,z ) = get( data, x,y-1,z );
				}
			}
		}
		// Clear last row
		for ( x = 0; x < size.x; ++x )
		{
			for ( z = 0; z < size.z; ++z )
			{
				get( data, x,0,z ) = sWeatherSimCell();
			}
		}
	}
	else if ( root_position.y < new_position.y )
	{
		root_position.y += 1;

		// Need to shift everything back on the X
		for ( y = 0; y < size.y-1; ++y )
		{
			for ( x = 0; x < size.x; ++x )
			{
				for ( z = 0; z < size.z; ++z )
				{
					get( data, x,y,z ) = get( data, x,y+1,z );
				}
			}
		}
		// Clear last row
		for ( x = 0; x < size.x; ++x )
		{
			for ( z = 0; z < size.z; ++z )
			{
				get( data, x,size.y-1,z ) = sWeatherSimCell();
			}
		}
	}

	// Z Shift
	if ( root_position.z > new_position.z )
	{
		root_position.z -= 1;

		// Need to shift everything forward on the X
		for ( z = size.z - 1; z > 0; --z )
		{
			for ( x = 0; x < size.x; ++x )
			{
				for ( y = 0; y < size.y; ++y )
				{
					get( data, x,y,z ) = get( data, x,y,z-1 );
				}
			}
		}
		// Clear last row
		for ( y = 0; y < size.y; ++y )
		{
			for ( x = 0; x < size.x; ++x )
			{
				get( data, x,y,0 ) = sWeatherSimCell();
			}
		}
	}
	else if ( root_position.z < new_position.z )
	{
		root_position.z += 1;

		// Need to shift everything back on the X
		for ( z = 0; z < size.z-1; ++z )
		{
			for ( x = 0; x < size.x; ++x )
			{
				for ( y = 0; y < size.y; ++y )
				{
					get( data, x,y,z ) = get( data, x,y,z+1 );
				}
			}
		}
		// Clear last row
		for ( y = 0; y < size.y; ++y )
		{
			for ( x = 0; x < size.x; ++x )
			{
				get( data, x,y,size.z-1 ) = sWeatherSimCell();
			}
		}
	}
}

// == Grab Terrain Values ==
void CWeatherSimulator::GrabTerrainValues ( void )
{
	//if (( !CVoxelTerrain::terrainList.empty() )&&( CVoxelTerrain::terrainList[0] != NULL ))
	if ( CVoxelTerrain::GetActive() )
	{
		Vector3d pos;
		int x, y, z;
		for ( x = 0; x < size.x; ++x )
		{
			for ( y = 0; y < size.y; ++y )
			{
				for ( z = 0; z < size.z; ++z )
				{
					sWeatherSimCell& currentCell = get( data,x,y,z );
					if (( currentCell.terrain == Terrain::TER_DEFAULT )||( currentCell.terrain == Terrain::BIO_DEFAULT ))
					{
						pos = getpos( x,y,z ) / 64.0f + Vector3d( 0.5f,0.5f,0.5f ); // TODO!
						currentCell.terrain	= Zones.GetTerrainTypeAt( pos*64.0f );
						currentCell.biome	= Zones.GetTerrainBiomeAt( pos*64.0f );
						//currentCell.terrain	= CVoxelTerrain::terrainList[0]->TerraGen_pub_GetType( pos );
						//currentCell.biome	= CVoxelTerrain::terrainList[0]->TerraGen_pub_GetBiome( pos );
						//currentCell.ground_position = CVoxelTerrain::terrainList[0]->TerraGen_pub_GetElevation( pos );
						//currentCell.ground_position = Zones.GetTerrainElevationAt( pos*64.0f );
						
#ifdef _ENGINE_DEBUG
						currentCell.match = RangeVector( rangeint(pos.x), rangeint(pos.y), rangeint(pos.z) );
					}
					else
					{
						pos = getpos( x,y,z ) / 64.0f + Vector3d( 0.5f,0.5f,0.5f ); // TODO!
						RangeVector target = RangeVector( rangeint(pos.x), rangeint(pos.y), rangeint(pos.z) );
						if ( currentCell.match != target ) {
							cout << "MASSIVE gamebreaking warning: MISMATCH ON WEATHERCELL!" << endl;
						}
#endif
					}
				}
			}
		}
	}
}

// == Value Generation ==
// Generates humidity values
void CWeatherSimulator::GenerateHumidityValues ( void )
{
	int x, y, z;
	for ( x = 0; x < size.x; ++x )
	{
		for ( y = 0; y < size.y; ++y )
		{
			for ( z = 0; z < size.z; ++z )
			{
				sWeatherSimCell& currentCell = get( data,x,y,z );
				currentCell.humidity = 1.0f + noise->Get3D(
					(ftype)root_position.x + x,
					(ftype)root_position.y + y,
					(ftype)root_position.z + z*0.1f + (ftype)(ActiveGameWorld->fCurrentTime*0.01f) ) * 0.7f;

				switch ( currentCell.biome )
				{
				case Terrain::BIO_COLD_DESERT:
				case Terrain::BIO_DESERT:
				case Terrain::BIO_GRASS_DESERT:
				case Terrain::BIO_TUNDRA:
					currentCell.humidity *= 0.5f; // Todo: SEASONS
					break;
				case Terrain::BIO_MOIST:
				case Terrain::BIO_RAINFOREST:
				case Terrain::BIO_WET_OUTLANDS:
				case Terrain::BIO_SWAMP:
				case Terrain::BIO_TROPICAL:
					currentCell.humidity *= 1.2f; // Todo: SEASONS
					break;
				case Terrain::BIO_S_TUNDRA:
					currentCell.humidity *= 3.0f;
					break;
				}
			}
		}
	}
}
// Generates temperature values
//  The biome is the largest factor (though season does factor in)
void CWeatherSimulator::SimulateTemperatureValues ( void )
{
	int x, y, z;
	ftype temperatureTarget;
	for ( x = 0; x < size.x; ++x )
	{
		for ( y = 0; y < size.y; ++y )
		{
			for ( z = 0; z < size.z; ++z )
			{
				sWeatherSimCell& currentCell = get( data,x,y,z );

				switch ( currentCell.biome )
				{
				case Terrain::BIO_COLD_DESERT:
				case Terrain::BIO_S_TUNDRA:
				case Terrain::BIO_TUNDRA:
				case Terrain::BIO_TAIGA:
					temperatureTarget = -0.7f;
					break;
				case Terrain::BIO_EVERGREEN:
					temperatureTarget = -0.1f;
					break;
				case Terrain::BIO_MOIST:
				case Terrain::BIO_WET_OUTLANDS:
					temperatureTarget = 0.2f;
					break;
				case Terrain::BIO_RAINFOREST:
				case Terrain::BIO_SWAMP:
				case Terrain::BIO_TROPICAL:
					temperatureTarget = 0.3f;
					break;
				case Terrain::BIO_DESERT:
					temperatureTarget = 0.83f+Random.Range(-0.03f,0.03f); 
					break;
				case Terrain::BIO_GRASS_DESERT:
					temperatureTarget = 0.75f; 
					break;
				case Terrain::BIO_THE_EDGE:
					temperatureTarget = -0.2f;
					break;
				default:
					temperatureTarget = 0.0f;
					break;
				}

				// Add noise
				temperatureTarget += noise->Get3D(
					(ftype)root_position.y + x + 3.2f,
					(ftype)root_position.z + y + 3.2f,
					(ftype)root_position.x + z*0.1f + (ftype)(ActiveGameWorld->fCurrentTime*0.012f) ) * 0.2f;

				currentCell.temperature += (1+temperatureTarget - currentCell.temperature)*0.2f;
			}
		}
	}

	// Now blend nearby areas
	for ( x = 1; x < size.x-1; ++x )
	{
		for ( y = 1; y < size.y-1; ++y )
		{
			for ( z = 1; z < size.z-1; ++z )
			{
				// now blur them temps
				temperatureTarget = ( get(data,x,y,z).temperature*4
					+ get(data,x+1,y,z).temperature
					+ get(data,x-1,y,z).temperature
					+ get(data,x,y+1,z).temperature
					+ get(data,x,y-1,z).temperature
					+ get(data,x,y,z+1).temperature
					+ get(data,x,y,z-1).temperature );

				get(data,x,y,z).temperature = temperatureTarget/10.0f;
			}
		}
	}
}

// Spawn clouds!!
void CWeatherSimulator::GenerateClouds ( void )
{
	ftype spawn_chance, spawn_check;
	const ftype seasonModifier = 1.0f;

	// Loop through every cell
	int x, y, z;
	for ( x = 0; x < size.x; ++x )
	{
		for ( y = 0; y < size.y; ++y )
		{
			for ( z = 0; z < size.z; ++z )
			{
				Vector3d spawnpos = getpos( x,y,z );
				sWeatherSimCell& currentCell = get( data,x,y,z );

				spawn_chance = currentCell.humidity * ( fabs( currentCell.temperature - 1.0f ) + 0.14f )
					* currentCell.dir.sqrMagnitude()*0.2f * seasonModifier * 0.001f;
				// don't generate below ground
				//ftype elevationWeight = (spawnpos.z/64.0f - currentCell.ground_position - 0.3f)*2.0f;
				ftype elevationWeight, elevationWeight1, elevationWeight2, elevationWeight3;
				elevationWeight1 = 1 - fabs(spawnpos.z/64.0f - (currentCell.ground_position + 1.8f));
				elevationWeight1 = max( min( elevationWeight1, 0.6f), 0.0f );
				elevationWeight2 = 1 - fabs(spawnpos.z/64.0f - (currentCell.ground_position + 4.8f));
				elevationWeight2 = max( min( elevationWeight2*2, 1.3f), 0.0f );
				elevationWeight3 = 1 - fabs(spawnpos.z/64.0f - (currentCell.ground_position + 7.8f));
				elevationWeight3 = max( min( elevationWeight2*3, 2.6f), 0.0f );

				elevationWeight = elevationWeight1 + elevationWeight2 + elevationWeight3;
				spawn_chance = spawn_chance * sqr( elevationWeight );

				spawn_check = random_range( 0.0f, 1.0f );
				if ( spawn_check < spawn_chance )
				{
					// Create cloud at position
					CWeatherCloud* newCloud = new CWeatherCloud ( spawnpos, Vector3d( 96,96,72 ) );
					//newCloud->AddReference();
				}
			}
		}
	}

	// FUCK IT, LETS COMBINE CLOUDS HERE TOO
	//for ( vector<CWeatherCloud*>::iterator it = C
	CCloudMaster::pActive->UpdateClouds();
}

// == Simulation ==
void CWeatherSimulator::SimulateWeather ( void )
{
	int x, y, z;
	// temperature is (mostly) constant (changes slightly with seasons)
	// humidity is variable with respect to time
	
	ftype lightChance = 0.0f;	// Chance to go back
	ftype heavyChance = 0.0f;	// Chance to go heavier
	ftype coldChance = 0.0f;	// Chance for cold
	ftype stormChance = 0.0f;	// Chance for VERY bad weather

	ftype lightChanceSample = Random.Range(0,1)+0.2f;
	ftype heavyChanceSample = Random.Range(0,1)+0.2f;
	ftype coldChanceSample = Random.Range(0,1)+0.2f;
	ftype stormChanceSample = Random.Range(0,1)+0.2f;

	// Loop through them ALL, and do different behavior based on the current weather
	for ( x = 0; x < size.x; ++x )
	{
		for ( y = 0; y < size.y; ++y )
		{
			for ( z = 0; z < size.z; ++z )
			{
				sWeatherSimCell& currentCell = get( data,x,y,z );

				uchar prevWeather = currentCell.weather;

				// Save time since weather changed
				currentCell.time_since_last_weather_change += 0.1f;

				// Base chance on that time
				lightChance = currentCell.time_since_last_weather_change*0.0046f;
				heavyChance = currentCell.time_since_last_weather_change*0.0017f;
				coldChance = currentCell.time_since_last_weather_change*0.001f;
				stormChance = currentCell.time_since_last_weather_change*0.001f;

				// Increase storm chance by wind speed
				stormChance += currentCell.dir.magnitude() * 0.4f;
				// Modify cold chance by temperature
				coldChance -= (currentCell.temperature-1.0f)*0.8f;
				// Increase heavy chance if it's near freezing
				heavyChance += std::max<ftype>(0.6f - currentCell.temperature, 0);

				// Bias weather by biome
				switch ( currentCell.biome )
				{
				case Terrain::BIO_TROPICAL:
				case Terrain::BIO_MOIST:
					heavyChance *= 4.0f;
					coldChance *= 0.1f;
					break;
				case Terrain::BIO_WET_OUTLANDS:
					heavyChance *= 4.0f;
					stormChance *= 3.0f;
					coldChance *= 0.3f;
					break;
				case Terrain::BIO_DESERT:
					lightChance *= 3.0f;
					heavyChance *= 0.2f;
					coldChance *= 0.1f;
					break;
				case Terrain::BIO_COLD_DESERT:
					lightChance *= 2.0f;
					heavyChance *= 0.2f;
					break;
				case Terrain::BIO_GRASS_DESERT:
					heavyChance *= 0.4f;
					coldChance *= 0.5f;
					break;
				case Terrain::BIO_TAIGA:
				case Terrain::BIO_TUNDRA:
					heavyChance *= 0.5f;
					lightChance *= 0.5f;
					coldChance *= 2.0f;
					break;
				case Terrain::BIO_GRASSLAND:
				case Terrain::BIO_TEMPERATE:
					heavyChance *= 0.5f;
					lightChance *= 0.5f;
					coldChance *= 0.5f;
					break;
				}

				// Do weather "simulation"
				switch ( currentCell.weather )
				{
				case Terrain::WTH_NONE:
				case Terrain::WTH_FOG_LIGHT:
					// Nothing happening, increase chances for something to happen.
					//heavyChance *= 2.0f; // YEAH, SURE.
					if ( currentCell.weather == Terrain::WTH_FOG_LIGHT ) {
						if ( ActiveGameWorld->bIsDaytime ) {
							lightChance *= 3.0f;
						}
						if ( currentCell.biome == Terrain::BIO_DESERT || currentCell.biome == Terrain::BIO_GRASS_DESERT || currentCell.biome == Terrain::BIO_COLD_DESERT ) {
							lightChance *= 5.0f;
						}
					}

					if ( heavyChanceSample < heavyChance ) {
						// Based on the temperature, humidity, and wind speed, change the weather
						if ( stormChanceSample < stormChance ) {
							currentCell.weather = Terrain::WTH_LIGHT_RAIN_THUNDER;
						}
						else if ( coldChanceSample < coldChance ) {
							currentCell.weather = Terrain::WTH_LIGHT_SNOW;
						}
						else {
							currentCell.weather = Terrain::WTH_LIGHT_RAIN;
						}
					}
					else if ( heavyChanceSample < heavyChance*2.0f ) {
						if ( stormChanceSample < stormChance ) {
							currentCell.weather = Terrain::WTH_FOG_LIGHT;
						}
						else if ( coldChanceSample < coldChance ) {
							currentCell.weather = Terrain::WTH_FOG_LIGHT;
						}
					}
					else if ( lightChanceSample < lightChance ) {
						currentCell.weather = Terrain::WTH_NONE;
					}

					// And of course, the heavy biases
					if ( currentCell.biome == Terrain::BIO_S_TUNDRA ) {
						currentCell.weather = Terrain::WTH_LIGHT_SNOW;
					}
					break;
				case Terrain::WTH_LIGHT_RAIN:
				case Terrain::WTH_HEAVY_RAIN:
					// Stuff happening, decrease chances of things happening
					if ( currentCell.weather == Terrain::WTH_HEAVY_RAIN ) {
						heavyChance *= 0.5f;
					}
					
					// Check to lighten up the weather
					if ( lightChanceSample < lightChance ) {
						if ( currentCell.weather == Terrain::WTH_HEAVY_RAIN ) {
							currentCell.weather = Terrain::WTH_LIGHT_RAIN;
						}
						else {
							currentCell.weather = Terrain::WTH_NONE;
						}
					}
					else if ( heavyChanceSample < heavyChance ) { // Or to make it heavy
						// Based on the temperature, humidity, and wind speed, change the weather
						if ( stormChanceSample < stormChance ) {
							currentCell.weather = Terrain::WTH_HEAVY_RAIN_THUNDER;
						}
						else if ( coldChanceSample < coldChance ) {
							currentCell.weather = Terrain::WTH_HEAVY_SNOW;
						}
						else {
							currentCell.weather = Terrain::WTH_HEAVY_RAIN;
						}
					}

					break;
				case Terrain::WTH_LIGHT_RAIN_THUNDER:
				case Terrain::WTH_HEAVY_RAIN_THUNDER:
					// Stuff happening, decrease chances of things happening
					if ( currentCell.weather == Terrain::WTH_HEAVY_RAIN_THUNDER ) {
						heavyChance *= 0.5f;
					}

					// Check to lighten up the weather
					if ( lightChanceSample < lightChance ) {
						if ( currentCell.weather == Terrain::WTH_HEAVY_RAIN_THUNDER ) {
							currentCell.weather = Terrain::WTH_LIGHT_RAIN_THUNDER;
						}
						else {
							// Thunderstorm turns into regular rain when letting up
							currentCell.weather = Terrain::WTH_LIGHT_RAIN;
						}
					}
					else if ( heavyChanceSample < heavyChance ) { // Or to make it heavy
						// Based on the temperature, humidity, and wind speed, change the weather
						if ( stormChanceSample < stormChance ) {
							currentCell.weather = Terrain::WTH_HEAVY_RAIN_THUNDER;
						}
						else if ( coldChanceSample < coldChance ) {
							currentCell.weather = Terrain::WTH_HEAVY_SNOW;
						}
					}

					break;
				case Terrain::WTH_LIGHT_SNOW:
				case Terrain::WTH_HEAVY_SNOW:
					// Stuff happening, decrease chances of things happening
					if ( currentCell.weather == Terrain::WTH_HEAVY_SNOW ) {
						heavyChance *= 0.5f;
					}

					// Check to lighten up the weather
					if ( lightChanceSample < lightChance ) {
						if ( currentCell.weather == Terrain::WTH_HEAVY_SNOW ) {
							currentCell.weather = Terrain::WTH_LIGHT_SNOW;
						}
						else {
							currentCell.weather = Terrain::WTH_NONE;
						}
					}
					else if ( heavyChanceSample < heavyChance ) { // Or to make it heavy
						// Based on the temperature, humidity, and wind speed, change the weather
						if ( currentCell.weather == Terrain::WTH_HEAVY_SNOW ) {
							if ( stormChanceSample < stormChance ) {
								currentCell.weather = Terrain::WTH_STORM_SNOW; // This is BAD if it happens.
							}
						}
						else {
							currentCell.weather = Terrain::WTH_HEAVY_SNOW;
						}
					}

					// And of course, the heavy biases
					if ( currentCell.biome == Terrain::BIO_S_TUNDRA ) {
						currentCell.weather = Terrain::WTH_STORM_SNOW;
					}
					break;
				case Terrain::WTH_STORM_SNOW:
					// Can't get worse than this
					heavyChance *= 0.5f;
					lightChanceSample *= 4.0f;

					// So lighten it up
					if ( lightChanceSample < lightChance ) {
						if ( stormChanceSample < stormChance ) {
							currentCell.weather = Terrain::WTH_HEAVY_SNOW;
						}
						else {
							currentCell.weather = Terrain::WTH_LIGHT_SNOW;
						}
					}

					// And of course, the heavy biases
					if ( currentCell.biome == Terrain::BIO_S_TUNDRA ) {
						currentCell.weather = Terrain::WTH_STORM_SNOW;
					}
					break;
				default:
					currentCell.weather = Terrain::WTH_NONE;
					break;
				}

				// Reset weather time if changed
				if ( currentCell.weather != prevWeather ) {
					currentCell.time_since_last_weather_change = 0.0f;
				}
				//
			}
		}
	}
}

void CWeatherSimulator::Simulate ( void )
{
	Vector3d temp;
	int x, y, z;

	// First copy over current data or 'board'
	for ( x = 0; x < size.max; ++x )
	{
		data_temp[x] = data[x];
	}
	
	// first sim part is do damping on velocity
	for ( x = 0; x < size.max; ++x )
	{
		data[x].dir *= 0.5f;
	}

	// === TERRAIN INJECTION ===
	// do sim for certain terrains or biomes
	for ( x = 0; x < size.x; ++x )
	{
		for ( y = 0; y < size.y; ++y )
		{
			for ( z = 0; z < size.z; ++z )
			{
				sWeatherSimCell& currentCell = get( data,x,y,z );
				if ( currentCell.terrain == Terrain::TER_SPIRES )
				{
					currentCell.pressure -= 0.1f;
					currentCell.pressure -= std::max<ftype>( 0.0f, std::min<ftype>( 0.6f, (root_position.z+z-size.z/4)*0.2f ) );

					// Also do a pressure removal in a circle around
					Vector2d offset;
					offset.x = sin( Time::currentTime * 0.5f )*1.8f;
					offset.y = cos( Time::currentTime * 0.5f )*1.8f;

					short tx, ty;
					tx = x + short(offset.x);
					ty = y + short(offset.y);
					if (( tx >= 0 )&&( tx < size.x )&&( ty >= 0 )&&( ty < size.y ))
					{
						get( data, tx,ty,z ).pressure -= 0.2f;
					}
				}
				else if ( currentCell.terrain == Terrain::TER_THE_EDGE )
				{
					currentCell.pressure += 0.14f;
				}
				else if ( currentCell.terrain == Terrain::TER_MOUNTAINS )
				{
					currentCell.pressure += 0.1f;
				}

				if ( currentCell.biome == Terrain::BIO_TROPICAL )
				{
					currentCell.pressure -= 0.1f;
				}
				else if (( currentCell.biome == Terrain::BIO_SWAMP )||( currentCell.biome == Terrain::BIO_WET_OUTLANDS ))
				{
					currentCell.pressure += 0.1f;
				}
				else if ( currentCell.biome == Terrain::BIO_DESERT )
				{
					currentCell.pressure *= 0.95f;
				}
			}
		}
	}
	// === END TERRAIN INJECTION ===
	// === TEMPERATURE AND HUMIDITY INJECTION ===
	// do pressure sim for temperature and humidity
	for ( x = 0; x < size.x; ++x )
	{
		for ( y = 0; y < size.y; ++y )
		{
			for ( z = 0; z < size.z; ++z )
			{
				sWeatherSimCell& currentCell = get( data,x,y,z );
				currentCell.pressure += currentCell.humidity*(currentCell.temperature-1.0f)*0.12f;
			}
		}
	}
	// === END TEMPERATURE AND HUMIDITY INJECTION ===

	// now check neighbors for more pressure
	for ( x = 1; x < size.x-1; ++x )
	{
		for ( y = 1; y < size.y-1; ++y )
		{
			for ( z = 1; z < size.z-1; ++z )
			{
				// add some weights! :D
				temp = Vector3d( 0,0,0 );

				ftype fCurPressure = get(data,x,y,z).pressure;

				temp.x += fCurPressure-get(data,x+1,y,z).pressure;
				temp.x -= fCurPressure-get(data,x-1,y,z).pressure;
				temp.y += fCurPressure-get(data,x,y+1,z).pressure;
				temp.y -= fCurPressure-get(data,x,y-1,z).pressure;
				temp.z += fCurPressure-get(data,x,y,z+1).pressure;
				temp.z -= fCurPressure-get(data,x,y,z-1).pressure;
	
				temp.x = cub( temp.x );
				temp.y = cub( temp.y );
				temp.z = cub( temp.z );

				get(data,x,y,z).dir += temp*0.25f;
			}
		}
	}

	// do wind equalizer
	for ( x = 0; x < size.x; ++x )
	{
		for ( y = 0; y < size.y; ++y )
		{
			for ( z = 0; z < size.z; ++z )
			{
				if (( x == 0 )||( x == size.x-1 )||( y == 0 )||( y == size.y-1 )||( z == 0 )||( z == size.z-1 ))
				{
					// add some weights! :D
					temp = Vector3d( 0,0,0 );

					ftype fCurPressure = get(data,x,y,z).pressure;

					if ( x != size.x-1 )
						temp.x += fCurPressure-get(data,x+1,y,z).pressure;
					if ( x != 0 )
						temp.x -= fCurPressure-get(data,x-1,y,z).pressure;
					if ( y != size.y-1 )
						temp.y += fCurPressure-get(data,x,y+1,z).pressure;
					if ( y != 0 )
						temp.y -= fCurPressure-get(data,x,y-1,z).pressure;
					if ( z != size.z-1 )
						temp.z += fCurPressure-get(data,x,y,z+1).pressure;
					if ( z != 0 )
						temp.z -= fCurPressure-get(data,x,y,z-1).pressure;

					temp.x = cub( temp.x );
					temp.y = cub( temp.y );
					temp.z = cub( temp.z );

					get(data,x,y,z).dir += temp*0.515f;
				}
			}
		}
	}

	// next do limits
	for ( x = 0; x < size.max; ++x )
	{
		if ( data[x].dir.magnitude() > 1 )
			data[x].dir = data[x].dir.normal();
	}

	// now check neighbors for more pressure
	for ( x = 1; x < size.x-1; ++x )
	{
		for ( y = 1; y < size.y-1; ++y )
		{
			for ( z = 1; z < size.z-1; ++z )
			{
				// now move them pressure
				temp = get(data,x,y,z).dir;

				get(data,x+1,y,z).pressure += temp.x;
				get(data,x-1,y,z).pressure -= temp.x;
				get(data,x,y+1,z).pressure += temp.y;
				get(data,x,y-1,z).pressure -= temp.y;
				get(data,x,y,z+1).pressure += temp.z;
				get(data,x,y,z-1).pressure -= temp.z;
			}
		}
	}

	// next do pressure equalizer
	for ( x = 0; x < size.x; ++x )
	{
		for ( y = 0; y < size.y; ++y )
		{
			for ( z = 0; z < size.z; ++z )
			{
				if (( x == 0 )||( x == size.x-1 )||( y == 0 )||( y == size.y-1 )||( z == 0 )||( z == size.z-1 ))
				{
					get( data,x,y,z ).pressure = 0;
				}
			}
		}
	}

	// move/blur results
	for ( x = 1; x < size.x-1; ++x )
	{
		for ( y = 1; y < size.y-1; ++y )
		{
			for ( z = 1; z < size.z-1; ++z )
			{
				// Sample surrounding six
				get( data_temp, x,y,z ).pressure =
					( get( data, x,y,z ).pressure +
					get( data, x+1,y,z ).pressure +
					get( data, x-1,y,z ).pressure +
					get( data, x,y+1,z ).pressure +
					get( data, x,y-1,z ).pressure +
					get( data, x,y,z+1 ).pressure +
					get( data, x,y,z-1 ).pressure )/7.0f;
			}
		}
	}
	for ( x = 1; x < size.x-1; ++x )
	{
		for ( y = 1; y < size.y-1; ++y )
		{
			for ( z = 1; z < size.z-1; ++z )
			{
				get( data, x,y,z ).pressure = ( 
					get( data_temp, x+1,y,z ).pressure +
					get( data_temp, x-1,y,z ).pressure +
					get( data_temp, x,y+1,z ).pressure +
					get( data_temp, x,y-1,z ).pressure +
					get( data_temp, x,y,z+1 ).pressure +
					get( data_temp, x,y,z-1 ).pressure )/6.0f;
			}
		}
	}
	// end simulation
}