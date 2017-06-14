
// class CMCCPlanet
//  Class pertaining to planet-wide settings (such as the planet seed)
// This is specifically meant to make grabbing off-world data easily.
// All instances of this class point to the currently active planet on default.
// Planet options are automatically saved when an instance is destroyed.

#ifndef _C_MCC_PLANET_H_
#define _C_MCC_PLANET_H_

#include "core/types/types.h"
#include "core/containers/arstring.h"
#include "core/math/Vector2d.h"

namespace World
{
	struct terrainSampleQuery_t
	{
		uchar	terra;
		uchar	biome;
		ftype	elevation;
	};
	struct regionSampleQuery_t
	{
		uint32_t	region;
	};
};

class CMCCPlanet
{

public:
	explicit	CMCCPlanet ( const char* targetPlanet = "" );
				~CMCCPlanet ( void );

	// Sets the seed for the realm.
	// If this is the active realm, will set the default seed
	void		SetSeed ( const int32_t );
	void		SetSeed ( const char* );	// Set seed via string hash w/ special cases
	// Returns the seed for the realm.
	// If this is the active realm, will set the default seed
	int32_t		GetSeed ( void );

	//  GetDataAt ( VECTOR2D query position, TERRAIN_SAMPLE_QUERY output data )
	// Samples the world for data. Modifies out_data with the results.
	// Returns true on successful query. Otherwise, returns false and out_data is not modified.
	bool		GetDataAt ( const Vector2d& n_position, World::terrainSampleQuery_t* out_data );

	// Saves the realm settings
	void		Save ( void );
	// Loads realm settings from file without saving
	void		Load ( void );

private:
	void		CheckActivePlanet ( void );
	void		UploadSettings ( void );

	bool					b_activePlanet;
	arstring<256>			m_targetPlanet;
	static arstring<256>	m_currentPlanet;

	struct planetSettings_t
	{
		int32_t seed;
		char seed_name [256];
	};
	planetSettings_t			settings;

};


#endif//_C_MCC_PLANET_H_