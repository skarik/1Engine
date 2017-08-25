
#ifndef _C_TERRA_GEN_DEBUG_H_
#define _C_TERRA_GEN_DEBUG_H_

// Includes
#include "CTerrainGenerator.h"
#include "TerrainInstanceTypes.h"

class CRegionManager;
class CTownManager;
class CDungeonBase;

#if __cplusplus > 199711L
	#include <unordered_map>
	using std::unordered_map;
#else
	#include <unordered_map>
	using std::tr1::unordered_map;
#endif


class CTerraGenDebug : public CTerrainGenerator
{
public:
	explicit CTerraGenDebug( CVoxelTerrain*, long int );
	~CTerraGenDebug ( void );

	virtual char	TerraGen_pub_GetType	( Vector3d const& );
	virtual char	TerraGen_pub_GetBiome	( Vector3d const& );
	virtual ftype	TerraGen_pub_GetElevation ( Vector3d const& );

	// == Threaded Query for Subsystems ==
	virtual ftype	Trd_Noise_HF ( const ftype, const ftype, const ftype=0 ) { return 0; };
	virtual ftype	Trd_Noise_Sample ( const ftype, const ftype, const ftype=0 ) { return 0; };

protected:
	// Simulation
	virtual void Simulate ( void ) {;};

	// Generation
	virtual char	TerraGen_priv_GetType	( Vector3d const& );
	virtual char	TerraGen_priv_GetBiome	( Vector3d const& );
	ftype	TerraGen_1p_GetElevation ( Vector3d const& );

	virtual void GenerateTerrainSector ( CBoob *, RangeVector const& );
	virtual void GenerateTerrainSectorSecondPass ( CBoob *, RangeVector const& ) {;};
	virtual void GenerateTerrainSectorThirdPass ( CBoob *, RangeVector const& ) {;};

	void TerraGenLandscapePass ( CBoob *, RangeVector const& );
	ushort TerraGen_Default ( CBoob *, Vector3d const& );

protected:
	// =====
	// Noise objects
	BaseNoise* noise;
	BaseNoise* noise_biome;
	BaseNoise* noise_terra;

	BaseNoise* noise_pub;
	BaseNoise* noise_pub_biome;
	BaseNoise* noise_pub_terra;

	// ssve
	char	iCurrentTerraType;

};

#endif