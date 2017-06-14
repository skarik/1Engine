
#ifndef _C_TOWN_GENERATION_INTERFACE_H_
#define _C_TOWN_GENERATION_INTERFACE_H_

#include "CTownManager.h"
#include "TerrainTypes.h"

class CTownGenerationInterface
{
public:
	typedef	CTownManager::sTownEntry sTownEntry;
	typedef	CTownManager::sTownUsageEntry sTownUsageEntry;
	typedef	CTownManager::sTownInfo sTownInfo;
	typedef	CTownManager::sPatternProperties sPatternProperties;
	typedef	CTownManager::sPatternRoad sPatternRoad;
	typedef	CTownManager::sPatternLandmark sPatternLandmark;
	typedef	CTownManager::sPatternInstance sPatternInstance;
	typedef	CTownManager::sPatternBuilding sPatternBuilding;
	//typedef	enum CTownManager::eBuildingType eBuildingType;
	typedef	CTownManager::sPattern sPattern;
	//typedef	enum CTownManager::eFloorplanValue eFloorplanValue;
public:
	CVoxelTerrain*		terrain;		// Pointer to associated terrain
	CTerraGenDefault*	generator;		// Pointer to associated mesh generation (used for regenerating towns)
public:
	uchar				areaBiome;
	uchar				areaTerra;
	ftype				areaElevation;
	RangeVector			areaPosition;
	sPattern*			genTown;
	sTownEntry*			genTownEntry;

	CTownManager*		floorplans;

	virtual void		Generate ( void )=0;

protected:
	Vector3d	townCenterPosition;
	int			mseed;
protected:
	// Utility function for random chance based on noise
	bool				Chance ( const ftype chance, const int seed );
	// Gets a random number from -0.5 to 0.5
	ftype				RandomNumber ( void );
};


#endif//_C_TOWN_GENERATION_INTERFACE_H_