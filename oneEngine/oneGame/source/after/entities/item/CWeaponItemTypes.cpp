
#include "CWeaponItemTypes.h"


const char* WeaponItem::GetTypeName ( const WeaponItemType& type )
{
	/*
	TypeDefault = 0,
		TypeBag,

		// == Weapon types ==
		TypeWeapon,
		// Blades
		TypeWeaponKnife,
		TypeWeaponSword,
		TypeWeaponLongSword,
		// Axes
		TypeWeaponPick,
		TypeWeaponPickaxe,
		TypeWeaponAxe,
		TypeWeaponGreatAxe,
		// Polearms
		TypeWeaponGlaive, //pole sword
		TypeWeaponPoleAxe,
		TypeWeaponPike,
		TypeWeaponScythe,
		// Diggers
		TypeWeaponShovel,
		// Bashers
		TypeWeaponHammer,
		TypeWeaponWarHammer,
		TypeWeaponClub,
		TypeWeaponFlail,
		*/
	static char weapontypeList [][32] =
	{
		"",
		"Bag",
		"Weapon",
		"Knife",
		"Sword",
		"Long Sword",
		"Pick",
		"Pickaxe",
		"Axe",
		"Great Axe",
		"Glaive",
		"Poleaxe",
		"Pike",
		"Scythe",
		"Shovel",
		"Hammer",
		"War Hammer",
		"Club",
		"Flail"
	};
	return weapontypeList[(int)type];
}