
#ifndef _C_TERRAIN_ACCESSOR_H_
#define _C_TERRAIN_ACCESSOR_H_

#include "physical/physics/raycast/RaycastHit.h"
#include "engine/behavior/CGameObject.h"
#include "after/types/terrain/BlockTracker.h"

class CVoxelTerrain;
class CTerrainProp;
class CFoliage;

class CTerrainAccessor
{
public:
	CTerrainAccessor ( void );
	~CTerrainAccessor ( void );

	//=========================================//
	// Block Grabbers
	//=========================================//

	Terrain::terra_b GetBlockAtPosition ( Vector3d const& );
	Terrain::terra_b GetBlockAtPosition ( RaycastHit const& );
	void GetBlockAtPosition ( Vector3d const&, BlockTrackInfo & );
	void GetBlockAtPosition ( RaycastHit const&, BlockTrackInfo & );

	//=========================================//
	// Block Setters
	//=========================================//

	bool SetBlockAtPosition ( Vector3d const&, const ushort );
	bool SetBlockAtPosition ( Vector3d const&, const Terrain::terra_b );
	bool SetBlockAtPosition ( BlockTrackInfo const&, const ushort );
	bool SetBlockAtPosition ( BlockTrackInfo const&, const Terrain::terra_b );

	//=========================================//
	// Query
	//=========================================//
	
	bool BlockHasComponent ( const BlockTrackInfo& );
	bool BlockHasFoliage ( const BlockTrackInfo& );
	bool BlockHasGrass ( const BlockTrackInfo& );

	//=========================================//
	// Editors
	//=========================================//

	void CompressBlock ( const BlockTrackInfo& );
	void DestroyBlock ( const BlockTrackInfo& );
	void TillBlock ( const BlockTrackInfo& );

	void DestroyGrass ( const BlockTrackInfo& );

	void AddComponent ( const BlockTrackInfo&, CTerrainProp* );
	void AddFoliage ( const BlockTrackInfo&, CFoliage* );

	void RemoveComponent ( CTerrainProp* );
	void RemoveFoliage ( CFoliage* );
	
private:
	CVoxelTerrain*	m_terrain;

};
#define TerrainAccess CTerrainAccessor()

#endif//_C_TERRAIN_ACCESSOR_H_