
#ifndef _DISCIPLINES_H_
#define _DISCIPLINES_H_

#include "core/types/types.h"

enum DisciplineAreas : uint8_t
{
	DscAreaNone,

	// General disciplines
	DscAreaStrength,
	DscAreaAgility,
	DscAreaIntelligence
};

enum Discipline : uint8_t
{
	DscNone						=0,

	// Special disciplines
	DscStartpoint				=1,
	DscRacialA					=2,
	DscRacialB					=3,

	// Specific disciplines

	// Intelligence disciplines
	DscMagicWestern				=4,
		// 4 Magic
		// 5 Alchemy (change)
		// 6 Enchanting (essencebind, mindgames)
	DscMagicEastern				=7,
		// 7 Magic
		// 8 Alchemy (balance)
		// 9 Enchanting (soulbind)
	DscMagicNature				=10,
		// 10 Magic
		// 11 Alchemy (poison)
		// 12 Enchanting (sproink, mindgames)

	// Agility disciplines
	DscMobility					=11,
	DscCombatRange				=12,

	// Strength disciplines
	DscMining					=13,
	DscCombatShield				=14,
	DscCombatMeleeTwoHanded		=15,
	DscCombatMeleeStrSingle		=16,
	DscCombatMeleeStrMulti		=17,

	// Intelligence+Agility
	DscThievery					=18,
	DscTinkering				=19,
	DscTrading					=20,

	// Agility+Strength
	DscCombatMeleeAgiSingle		=21,
	DscCombatMeleeAgiMulti		=22,
	DscCombatBrawler			=23,

	// Strength+Intelligence
	DscCrafting					=24,
		// 24 Crafting
		// 25 Cooking
		// 26 Building
	DscForging					=27,
	DscFarming					=28,


	DSC_MAX_COUNT				=32
};

enum SubDisciplines : uint8_t
{
	// DscMagicNature
	SubDscNone					=0,

	//SubDscForaging				=1,

	// DscMagicNature, DscMagicWestern, DscMagicEastern
	SubDscMagic					=0,
	SubDscAlchemy				=1,
	SubDscEnchanting			=2, // so 

	// DscCrafting
	SubDscCrafting		=0,
	SubDscCooking		=1,
	SubDscBuilding		=2
};

#endif//_DISCIPLINES_H_