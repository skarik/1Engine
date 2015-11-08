
#ifndef _ENGINE_COMMON_ITEM_PROPERTIES_H_
#define _ENGINE_COMMON_ITEM_PROPERTIES_H_

namespace Item
{
	// HoldType enumeration
	enum HoldType
	{
		Default,
		CircleGrip,
		Sword,
		TwoHandedSword,
		DefaultRanged,
		ChannelMelee,
		LightSourceMelee,
		EmptyHanded,
		TwoHandedAxe,
		SpellHanded,
		//SpellBallHanded,
		Bow
	};
	// HoldState enumeration
	enum HoldState
	{
		Holding,
		OnBelt,
		Hidden,
		Unequipping,
		Equipping,
		None,
		Hover,
		SystemHidden,
		UserSet
	};
	// Use type enumeration
	enum UseType
	{
		UPrimary		= 0,
		UOne			= 0,
		USecondary		= 1,
		UTwo			= 1,
		UTertiary		= 2,
		UOptional		= 2,
		UDefend			= 3,
		UFour			= 3,

		UPrimarySprint		= 10,
		USecondarySprint	= 11,
		UOptionalSprint		= 12,
		UDefendSprint		= 13,

		UModifier		= 24,
		UModifierPrepare= 25
	};
	// Stance enumeration
	enum EquipStance
	{
		StanceOffense,
		StanceDefense
	};

	// HIT-TYPE used to indentify raycast types
	enum HitType {
		HIT_NONE = 0,
		HIT_TERRAIN,
		HIT_ACTOR,
		HIT_TREE,
		HIT_COMPONENT,
		HIT_CHARACTER,

		HIT_UNKNOWN,

		MAX_HIT_TYPE_COUNT
	};

};

#endif//_ENGINE_COMMON_ITEM_PROPERTIES_H_