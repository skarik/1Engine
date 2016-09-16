
#ifndef _GAMETYPE_NOT_DYING_H_
#define _GAMETYPE_NOT_DYING_H_

#include "CGameType.h"
#include "after/states/CWorldState.h"

class CCamera;
class CPlayerStats;
class Daycycle;
class CEnvironmentEffects;
class CTerrainLoadscreen;
namespace NPC {
	class CZonedCharacterController;
	class CNpcSpawner;
}

class gametypeNotDying : public CGameType
{

public:
	gametypeNotDying ( CGameType*, const Vector3d& n_spawnpoint );
	~gametypeNotDying ( void );

	void Update ( void );

private:
	void GsInitialize ( void );
	void GsLoading ( void );
	void GsSpawning ( void );
	void GsSpawnFall ( void );
	void GsGameplay ( void );

private:
	enum eCurrentState
	{
		GSTATE_INITIALIZE,
		GSTATE_LOADING,
		GSTATE_SPAWNING,
		GSTATE_SPAWNFALL,
		GSTATE_GAMEPLAY
	} currentState;

	Vector3d player_spawnpoint;

	/*CWorldState		worldState;

	CTerrainLoadscreen*	pTerrainLoadscreen;
	CCamera*		pDefaultCamera;
	CPlayerStats*	pPlayerStats;	// ownership is given to player on player spawn

	//Daycycle*		pWorldDaycycle;
	CEnvironmentEffects* pWorldEffects;

	NPC::CZonedCharacterController*	pCharacterController;
	NPC::CNpcSpawner*	pNpcSpawner;

	bool			bHasSpawnPoint;
	*/
};

#endif