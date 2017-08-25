// CRacialStats is a misleading name.
// Really, this should be called CCharacterStats, and CharacterStats should be GeneralStats.

#ifndef _C_RACIAL_STATS_H_
#define _C_RACIAL_STATS_H_

#include <vector>
#include <string>
//#include "CharacterStats.h"
#include "core/math/Vector3d.h"
#include "core/math/Color.h"
#include "core/containers/arstring.h"

#include "after/types/character/Attributes.h"

using std::string;

class CBinaryFile;
class CBaseSerializer;
class CCharacter;
class CharacterStats;

class CRacialStats
{
public:
	explicit		CRacialStats ( void ) {
		//stats		= new CharacterStats();
		stats		= NULL;
		bTableReady = false;
		pOwner		= NULL;
	};
	void			CopyFrom ( const CRacialStats* n_copy_source );

	// == Initialization ==
					// Sets initial stats
	void			SetDefaults ( void );
					// Loads up Race defaults from Lua
	void			SetLuaDefaults ( void );

	// == Vanity Stats ==
					// Rerolls colors, but takes race into account
	void			RerollColors ( void );
					// Limit colors based on the player race
	void			LimitColors ( void );

	// == Stat IO ==
					// Loads player stats from file, or calls SetDefaults if cannot find stats
	void			LoadFromFile ( CBinaryFile& );
					// Saves player stats to file
	void			SaveToFile ( CBinaryFile& );
					// Unified saving and loading
	void			serialize ( CBaseSerializer &, const uint );

	// == Step, Update, and Gameplay ==
					// Updates gameplay stats
	void			UpdateStats ( void );
					// Performs a player level up
	void			LevelUp ( void );
					// Generates a spawnpoint for the player, assuming the terrain exists
	//void			MakeSpawnpoint ( void );
public:
	CCharacter*	pOwner;

	// ====== PLAYERTYPE STATS ======
	// == Player Enum Stats ==
	eCharacterRace		iRace;
	eCharacterGender	iGender;

	int		iHairstyle;
	int		iMovestyle;
	int		iTalkstyle;
	int		iMiscstyle;

	// == Player String Stats ==
	string	sPlayerName;
	string	sLastName;

	// == Player Special Info ==
	eCharacterSpecial	iSpecialType;
	int		iBodyType;
	int		iFaceType;
	int		iMiscType; //ear or horn style usually

	// ====== VANITY STATS ======
	// == Player Colors ==
	Color	cEyeColor;		// default eye color
	Color	cFocusColor;	// focus color for magical randomness + eye glowing

	Color	cHairColor;
	Color	cSkinColor;

	// == Visual Equipment for Quick Load ==
	string	sGearHead;
	string	sGearNecklace;
	string	sGearShoulder;
	string	sGearChest;
	string	sGearRighthand;
	string	sGearLefthand;
	string	sGearLegs;
	string	sGearRightfoot;
	string	sGearLeftfoot;

	// == Tattoo and Body marking info ==
	unsigned short	iTattooCount;

	struct tattoo_t {
		uchar			type;		// type (if it glows, if it's a scar, yadayada)
		arstring<32>	pattern;	// name of pattern
		Color			color;		// color of marking
		bool			mirror;		// should it be mirrored? (symmetrical designs)
		Vector3d		projection_pos;		// projection start position
		Vector3d		projection_dir;		// projection direction
		Vector3d		projection_scale;	// projection scaling
		ftype			projection_angle;	// the angle to that the projection area should curve to
		// Tattoos are rendered once to a side-buffer. The side-buffer is rendered on 
		// the character model with a shader. Rendering a tattoo to the side-buffer
		// involves a model-space to UV-space projection. A semi-high density quad
		// plane is used to render the tattoo to the side-buffer. The curvature of the
		// projection can be configured to give better results on curved areas, such
		// as arms or legs. A limitation is the project needs one of the first rays to
		// hit, or the projection will be ignored. The algorithm needs a reference
		// point to work with.
	};
	std::vector<tattoo_t>	tattooList;
	
	// ====== GAMEPLAY STATS ======
	// Point stats stored here
	CharacterStats*	stats;

	//unsigned int	iStatPoints;
	//unsigned int	iSkillPoints;

	// Used stats
	float	fTimeSinceCombat;	// used for regen buffs

	// ====== PHYSICS STATS ======
	// == Player Hull Sizes ==
	float	fPlayerRadius;
	float	fStandingHeight;
	float	fCrouchingHeight;
	float	fProneHeight;

	// == Movement ==
	// Speed and acceleration
	float	fRunSpeed;
	float	fSprintSpeed;
	float	fCrouchSpeed;
	float	fProneSpeed;
	float	fSwimSpeed;
	float	fGroundAccelBase;
	float	fAirAccelBase;
	float	fWaterAccelBase;
	float	fJumpVelocity;
	// Special moves
	bool	bCanAutojump;
	bool	bCanPowerslide;
	bool	bCanSprint;
	bool	bCanCrouchSprint;

	float	fSlideTime;	// length of power slide

	int		iWallRunStepCount;
	
	// Misc Stats
	unsigned int		iWieldCount;


private:
	// Helper Routines
	//void			GameStats_GenerateSpawnpoint ( void );
	//void			GameStats_CheckSpawnpoint ( void );

	void			Lua_GenerateLinkTable ( void );

	// System Variables
	bool bTableReady;

	//void			PointStats_SetDefaultFromRace ( void );
	
	//void			PhysStats_SetDefaultFromRace ( void );
	//void			PhysStats_SetHullSizes ( void );
	//void			PhysStats_SetDefaultSpeed ( void );
	//void			PhysStats_AddSpeedBonusFromPoints ( void );
};

namespace Races
{
	string GetRaceGeneralDesc ( const eCharacterRace );
	string GetRaceBuffDesc ( const eCharacterRace );
	string GetCharacterDescriptor ( const CRacialStats* );
};

#endif
