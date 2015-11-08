
#ifndef _C_WORLD_STATE_H_
#define _C_WORLD_STATE_H_

#include "core/types/types.h"
#include "core/types/float.h"
#include "core/math/Math.h"
#include "core/math/Color.h"

namespace NPC
{
	//	struct spawnerState
	// State of the world enemy spawner. Certain special globals need to be kept track of.
	struct spawnerState_t
	{
		int		iWeepingAngelsFree;
		ftype	fWeepingAnglesBaseSpawnChance;

		int		iMiniWeepingAngelsFree;

		spawnerState_t()
		{
			iWeepingAngelsFree = 0;
			fWeepingAnglesBaseSpawnChance = 0;
			iMiniWeepingAngelsFree = 0;
		}
	};
}
namespace World
{
	//	struct 
	struct generationSubsystemState_t
	{
		// next ID to assign to the container
		uint32_t	iContainerCount;

		generationSubsystemState_t()
		{
			iContainerCount = 0;
		}
	};
}

class CWorldState
{

public:
	CWorldState ( void );
	~CWorldState ( void );

public:
	void Tick ( void );
	void Save ( void );
	void Load ( void );
	void SetDefaults ( void );
	
public:
	double		fTimeSpeed;
	double		fCurrentDay;
	double		fCurrentTime;

	uint32_t	bIsDaytime;

	uint32_t	regionCount;
	uint32_t	townCount;

	Color		cBaseAmbient;
	Color		cBaseColor;

	//uint32_t	iFeatureCount [16];
	/*struct feature_info_t
	{
		uint32_t count;
		Vector3d position
	};*/
	NPC::spawnerState_t spawnerState;
	World::generationSubsystemState_t generationSubsystemState;
};

extern CWorldState* ActiveGameWorld;

#endif