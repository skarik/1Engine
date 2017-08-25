// Now, it's CALLED PlayerStats, but it actually can be used for NPC's as well!
// Isn't that something?!?

#ifndef _C_PLAYER_STATS_H_
#define _C_PLAYER_STATS_H_

#include "core/containers/arstring.h"
#include "core/math/vect3d_template.h"
#include "after/states/CRacialStats.h"

#include <map>

class CBaseSerializer;
typedef CBaseSerializer Serializer;

class CCharacter;

namespace Terrain
{
	class CWorldGenerator;
};

class CPlayerStats
{
public:
	explicit		CPlayerStats ( void );

	// == Initialization ==
					// Sets initial stats
	void			SetDefaults ( void );
					// Randomizes stats
	void			Randomize ( void );

	// == Stat IO ==
					// Loads player stats from file, or calls SetDefaults if cannot find stats
	void			LoadFromFile ( void );
					// Saves player stats to file
	void			SaveToFile ( void );
					// Unified saving and loading
	void			serialize ( Serializer &, const uint );

	// == Step, Update, and Gameplay ==
					// Updates gameplay stats
	void			UpdateStats ( void );
					// Performs a player level up
	void			LevelUp ( void );
					// Generates a spawnpoint for the character, given a generator.
					// Different results will occur for different races.
	Vector3d_d		MakeSpawnpoint ( Terrain::CWorldGenerator* );
public:
	CCharacter*	pOwner;
	
	// ====== GAMEPLAY STATS ======
	// Game stats
	Vector3d	vPlayerInitSpawnPoint;
	Vector3d	vPlayerCalcSpawnPoint;
	std::map<string,Vector3d_d>	mPlayerSpawnpointMap;

	// Point stats stored here
	CRacialStats*	race_stats;
	CharacterStats*	stats;

	// Skillpoints
	int16_t		skillpoints_race;
	int16_t		skillpoints_skill;
	int16_t		skillpoints_discipline [32];
	Real		skillpoints_experience [32];

	// Used stats
	float	fTimeSinceCombat;	// used for regen buffs (TODO: IS THIS EVEN USED?)

	// Record stats
	bool	bHasPlayed; // If has played on this world previously.
	float	fDeathCount;
	float	fFeetFallen;
	float	fFeetTravelled;

	// ====== GAME MECHANICS STATS ======
	// Companion info
	eSexualPreferences		companion_sex_prefs;
	eCompanionLoveInterest	companion_love_prefs;
	eCharacterGender		companion_gender;		// overwritten once game starts
	eCharacterRace			companion_race;			// overwritten once game starts

private:
	// Helper Routines
	void			GameStats_GenerateSpawnpoint ( void );
	void			GameStats_CheckSpawnpoint ( void );	
};

#endif