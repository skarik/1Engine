
#ifndef _C_NPC_SPAWNER_H_
#define _C_NPC_SPAWNER_H_

#include "engine/behavior/CGameBehavior.h"
#include "after/types/terrain/TerrainTypes.h"
#include "after/types/terrain/BiomeTypes.h"
#include "after/states/CWorldState.h"

class CPlayer;
class CVoxelTerrain;

namespace NPC
{
	class CZonedCharacterController;

	class CNpcSpawner : public CGameBehavior
	{
		ClassName( "CNpcSpawner" );
	public:

		explicit CNpcSpawner ( CVoxelTerrain*, CZonedCharacterController* );
		~CNpcSpawner ( void );

		void Update ( void );

		static Vector3d GetSpawnPosition ( const Vector3d n_sourcePlayerPosition, bool& out_success );
	private:

		CVoxelTerrain*	pTerra;
		CZonedCharacterController* pController;
		//CPlayer*		currentPlayer;

		ftype	fSpawnUpdateTimer;
		int		iSpawnUpdateFramecount;

		Terrain::EBiomeType		iCurrentBiome;
		Terrain::ETerrainType	iCurrentTerrain;

		void SpawnNpcs ( const Vector3d n_sourcePlayerPosition );

		spawnerState_t*		spawnerState;
		void	InitSpawnerState ( void );

		Vector3d GetAirSpawnPosition ( void );
	
	};

	extern CNpcSpawner* Spawner;

}

#endif