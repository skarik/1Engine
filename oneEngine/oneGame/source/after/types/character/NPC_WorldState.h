// NPC_WorldState.h
// Contains definitons for the sWorldState, sPreferences, and sOpinions structs for the NPC system.
// These structs are used to store behavior of NPCs.

// THIS SHOULD GO UNDER "TYPES"


#ifndef _NPC_WORLDSTATE_H_
#define _NPC_WORLDSTATE_H_

//#include "CGameObject.h"
//#include "NPC_AIState.h"
//#include "CharacterStats.h"
#include "core/types/types.h"
#include "core/containers/arstring.h"
#include "after/states/NPC_CombatState.h"
#include "after/types/character/Attributes.h"
#include "after/types/character/NPC_AIFocus.h"

class CRacialStats;

namespace NPC
{
	enum eCharacterFaction : uint8_t
	{
		FactionNone = 0,
		FactionBandit,
		FactionBanditRunner,

		FactionInvalid = 255
	};
	struct sWorldState
	{
		sWorldState ( void ) : worldPosition(Vector3d::zero), travelDirection(Vector3d::zero), mFocus(AIFOCUS_Wanderer), partyHost(0), mFaction(FactionNone) {;};

		Vector3d	worldPosition;
		Vector3d	travelDirection;
		eBasicAIFocus	mFocus;
		uint64_t	partyHost;
		eCharacterFaction	mFaction;
		arstring<64>	mFocusName;
	};
	struct sPreferences
	{
		bool		requiresGlasses;
		bool		judgemental;
		bool		friendly;
		bool		_buf0;
		eSexualPreferences		sexualPref;
		eCompanionLoveInterest	lovePref;
		// have clothing preferences as well

		/*
		Characters should also save their turn rate
		(different people have different turn rates,
			give same fauna same turn rates,
				save the turn rates in the character model,
					races have different turn rates (cat people are a little bit faster))
		*/
	};
	struct sOpinions
	{
		ftype		loyalty;
		ftype		animosity;
		ftype		opinion;
	};

	struct characterFile_t
	{
		// Default Constructor sets contents to NULL
		characterFile_t ( void ) : rstats(NULL), worldstate(NULL), prefs(NULL), opinions(NULL) {;};
		
		CRacialStats*	rstats;
		sWorldState*	worldstate;
		sPreferences*	prefs;
		sOpinions*		opinions;
	};
}

#endif//_NPC_WORLDSTATE_H_