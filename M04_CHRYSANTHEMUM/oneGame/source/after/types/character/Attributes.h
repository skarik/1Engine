
#ifndef _AFTER_TYPES_CHARACTER_H_
#define _AFTER_TYPES_CHARACTER_H_

#include "core/types/types.h"

enum eCharacterRace : uint8_t
{
	CRACE_DEFAULT = 0,
	CRACE_HUMAN,		// european
	CRACE_ELF,			// dicks
	CRACE_DWARF,		// dirt
	CRACE_KITTEN,		// asian
	CRACE_FLUXXOR,		// islander
	CRACE_MERCHANT,		// desert and sand

	CRACE_SAME		= 254,
	CRACE_RANDOM	= 255
};
enum eCharacterGender : uint8_t
{
	CGEND_FEMALE = 0,
	CGEND_MALE,
	CGEND_NDEF_FEMALE,	// unisex race "female"
	CGEND_NDEF_MALE,	// unisex race "male"

	CGEND_OPPOSITE	= 253,
	CGEND_SAME		= 254,
	CGEND_RANDOM	= 255
};
enum eCharacterSpecial : uint8_t
{
	CSPCL_NONE = 0,
	CSPCL_WITCHEYE,			// bonus magic, minus charisma
	CSPCL_RINCEWIND,		// start with a high-cooldown spell, but magic exp income is reduced
	CSPCL_DARKCURSED,		// takes more magic damage, have more affinity to "dark" offensive spells, "light" spells less effective
	CSPCL_DRAGONBORN		// can yell loudly and seduce dragons
};
enum eTattooType : uint8_t
{
	TATT_CLANMARK	= 0,
	TATT_TATTOO		= 1,
	TATT_SCAR		= 2,
	TATT_FLUXXGLOW	= 3
};
enum eSexualPreferences : uint8_t
{
	SEXPREF_STRAIGHT = 0,
	SEXPREF_GAY,
	SEXPREF_BISEXUAL,	// aka anything with a hole
	SEXPREF_ONLY_MALE,
	SEXPREF_ONLY_FEMALE,
	
	SEXPREF_RANDOM	= 255
};
enum eCompanionLoveInterest : uint8_t
{
	CLOVE_PLAYER = 0,
	CLOVE_NOT_PLAYER,
	CLOVE_NO_PREFERENCE,

	CLOVE_RANDOM	= 255
};
enum eCompanionExperience : uint8_t
{
	CEXPC_MISTRUSTFUL,
	CEXPC_NEW,
	CEXPC_MENTOR,

	CEXPC_RANDOM	= 255
};
enum eCharacterPersonality : uint16_t
{
	CPRSNA_STANDARD,

	CPRSNA_RANDOM	= 65535
};
enum eCharacterMovestyle : uint8_t
{
	CMOVES_DEFAULT_INVALID = 0,
	CMOVES_DEFAULT,		// Normal person
	CMOVES_JOLLY,		// Really hoppy movement
	CMOVES_GLOOMY,		// Bad posture. Emo.
	CMOVES_SASSY,		// Generally sexy/faggoty walking
	CMOVES_LARGE_HAM,	// Posing and confident badass

	CMOVES_MAX,

	CMOVES_RANDOM	= 255
};
enum eCharacterTalkstyle : uint8_t
{
	CTALKS_DEFAULT,

	CTALKS_RANDOM	= 255
};

#endif//_AFTER_TYPES_CHARACTER_H_