// CWeatherSimulator
// Controls the simulation for wind, pressure, temperature, and humidity.
// 


#ifndef _C_WEATHER_SIMULATOR_
#define _C_WEATHER_SIMULATOR_

// == Includes ==
#include "core/math/Vector3d.h"
#include "engine/behavior/CGameBehavior.h"
#include "after/types/terrain/TerrainTypes.h"
#include "after/types/terrain/BiomeTypes.h"
#include "after/types/terrain/WeatherTypes.h"
#include "after/types/WorldVector.h"

// Predefines
class CWindMotion;
class CWorldState;
class Perlin;

// Structure for the simulator cell
struct sWeatherSimCell
{
	// Data members
	Vector3d	dir;			// weather Wind direction
	ftype		pressure;		// weather Pressure
	ftype		humidity;		// weather Humidity
	ftype		temperature;	// weather Temperature

	ftype		time_since_last_weather_change; //in seconds

	ftype	ground_position;	// terra Elevation
	unsigned char	terrain;	// terra Terrain
	unsigned char	biome;		// terra Biome

	unsigned char	weather;	// world Weather

#ifdef _ENGINE_DEBUG
	RangeVector match;
#endif

	// Constructor
	sWeatherSimCell ( void )
	{
		dir = Vector3d( 0,0,0 );
		pressure = 0;
		humidity = 0;
		temperature = 1;

		terrain = Terrain::TER_DEFAULT;
		biome	= Terrain::BIO_DEFAULT;

		weather = Terrain::WTH_NONE;
		time_since_last_weather_change = 20.0f;
	}
};

// Structure for the simulator size
struct sSimulatorSize
{
	short x;
	short y;
	short z;
	int	max;
};

// == Classes ==
class CWeatherSimulator : public CGameBehavior
{
	ClassName( "CWeatherSimulator" );
public:
	// Constructor + Destructor
	explicit CWeatherSimulator ( short xSize=64, short ySize=64, short zSize=16 );
	~CWeatherSimulator ( void );

	// Update
	void Update ( void );
	// System Simulate
	void UpdateTick ( void );

	// Static active
	static CWeatherSimulator* Get ( void );
	static void SetActive ( CWeatherSimulator* );

private:
	// == Private Routines ==

	// Performs simulation. This includes pressure which 
	void Simulate ( void );
	// Spawns clouds
	// yay clouds
	void GenerateClouds ( void );
	// Simulates weather
	// oh no rain
	void SimulateWeather ( void );

	// Generates humidity values
	//  Both the current time, biome, and terrain are used for generation
	void GenerateHumidityValues ( void );
	// Generates temperature values
	//  The biome is the largest factor 
	//void GenerateTemperatureValues ( void );
	void SimulateTemperatureValues ( void ); // Blends with nearby cells and also do season+biome

	// Populates the cells with biome information
	void GrabTerrainValues ( void );

	// When terrain's root updates, the simulation root updates as well.
	void UpdateRoot ( void );
	// Sets new root and does necessary data shifts
	void SetRoot ( const RangeVector& );

	// == Private Getting Routines ==
	// Returns the reference to the cell in given table
	inline sWeatherSimCell&	get ( sWeatherSimCell* source, short x, short y, short z )
	{
#ifdef _ENGINE_DEBUG
		int up = 2;
		if ( (x)+(y*size.x)+(z*size.x*size.y) < 0 )
		{
			std::cout << "Bad bounds." << std::endl;
			throw up;
		}
		else if ( (x)+(y*size.x)+(z*size.x*size.y) >= (size.x*size.y*size.z) )
		{
			std::cout << "Really bounds." << std::endl;
			throw up;
		}
#endif
		return source[(x)+(y*size.x)+(z*size.x*size.y)];
	}
	// Turns a cell index into a world position
	inline Vector3d getpos ( short x, short y, short z )
	{
		return Vector3d( (root_position.x + x - size.x/2)*64.0f, (root_position.y + y - size.y/2)*64.0f, (root_position.z + z - size.z/4)*64.0f );
	}

private:
	sWeatherSimCell*	data;
	sWeatherSimCell*	data_temp;

	sSimulatorSize		size;

	// Keeping track and pushing old information outta the stack.
	RangeVector		root_position;
	Vector3d			vCameraPos;

	// Keeping track of time to update the simulation
	bool				bNeedsUpdate;
	ftype				fUpdateTimer;

	// Noise function for some slow-changing variation
	Perlin*				noise;

	// Static instance
	static	CWeatherSimulator*	pActiveSim;

	// Friend the wind motion to give optimized sampling ability
	friend CWindMotion;
};

#endif