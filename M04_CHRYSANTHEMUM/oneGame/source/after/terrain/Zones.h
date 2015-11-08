

#ifndef _ZONE_CHECKER_H_
#define _ZONE_CHECKER_H_

// Includes
#include "core/math/Vector3d.h"
#include "core/math/vect3d_template.h"
#include "after/types/terrain/BlockData.h"
#include "after/types/WorldVector.h"
//#include "CVoxelTerrain.h"
class CVoxelTerrain;

// Class Definition
class CZoneChecker
{
public:
	//	PositionToRV ( VECTOR3D position to convert )
	// Returns:
	//		RangeVector - the input vector converted to index coordinates.
	//		For the MCC, this takes into account the active terrain, and will offset based on its root.
	//		Note that this function will still work even if there is no active terrain.
	static RangeVector	PositionToRV		( const Vector3d & );
	static RangeVector	PositionToRV		( const Vector3d_d & );

public:
	//	IsActiveArea ( VECTOR3D position )
	// Returns:
	//		bool - indicative if the given position is in an 'active' zone.
	//		For the MCC, this checks mainly the current active terrain.
	//		Active terrain areas are where objects should be allowed to be active.
	//		The active terrain area is where the physics engine is gaurenteed to be active, and is where
	//		CCharacter instances should be limited to.
	static bool			IsActiveArea		( const Vector3d & );
	//	IsSuperArea ( VECTOR3D position )
	// Returns:
	//		bool - indicative if the given position is in an 'super' zone.
	//		For the MCC, this checks mainly the current active terrain.
	//		Super terrain areas are very very very much last place that objects should be allowed to be active.
	//		CCharacter instances should be limited to this area at the least.
	//		Totally good for flying enemies though.
	static bool			IsSuperArea			( const Vector3d & );
	//	IsCollidableArea ( VECTOR3D position )
	// Returns:
	//		bool - indicating that the given terrain area has data loaded and has a non-negative collision reference.
	//		This is rather specific to AFTER. It will always return true if there is no terrain to check.
	static bool			IsCollidableArea	( const Vector3d & );

public:
	//	GetCurrentRoot ( )
	// Returns:
	//		double Vector3d - current XYZ position of the world that the root represents right now.
	//		This is specific to AFTER.
	static Vector3d_d	GetCurrentRoot ( void );

public:
	//	GetTerrainBiomeAt ( VECTOR3D position )
	// Returns:
	//		char - corresponding to an enumeration in EBiomeType defined in CBoob.h
	//		For the MCC, this checks the current active terrain and recreates the terrain generation algorithm.
	static char			GetTerrainBiomeAt	( const Vector3d & );
	//	GetTerrainTypeAt ( VECTOR3D position )
	// Returns:
	//		char - corresponding to an enumeration in ETerrainType defined in CBoob.h
	//		For the MCC, this checks the current active terrain and recreates the terrain generation algorithm.
	static char			GetTerrainTypeAt	( const Vector3d & );
	//	GetTerrainElevationAt ( VECTOR3D position )
	// Returns:
	//		ftype - giving the elevation in sector coordinates. Multiply by 64 to get the elevation in feet.
	//		For the MCC, this checks the current active terrain and recreates the terrain generation algorithm.
	//static ftype		GetTerrainElevationAt	( const Vector3d & );

	//	IsActiveTerrain ( TERRAIN* terrain pointer )
	// Returns:
	//		bool - Returns true if supplied pointer is voxel terrain
	static bool			IsActiveTerrain		( const CVoxelTerrain* );
	//	GetActiveTerrain ( void )
	// Returns:
	//		pointer - Returns pointer to active terrain or NULL.
	static CVoxelTerrain* GetActiveTerrain	( void );

	// Unused
	static ftype		GetLesbianDensity	( const Vector3d & );

public:
	//static Terrain::terra_t	GetBlockAtPosition ( const Vector3d & );
	static Terrain::terra_b	GetBlockAtPosition ( const Vector3d & );

public:
	//
	// Returns:
	//		int - counted difference between the terrain types
	//		A value of 4 is typically the "stop" zone. Anything higher is a full stop.
	//
	static int			TerraCompare		( const char, const char );
	//
	// Returns:
	//		int - counted difference between the terrain types
	//		A value of 4 is typically the "stop" zone. Anything higher is a full stop.
	//
	static int			BiomeCompare		( const char, const char );
};

// Global Instance
extern CZoneChecker Zones;


#endif