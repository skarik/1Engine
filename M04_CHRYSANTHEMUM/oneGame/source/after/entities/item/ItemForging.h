
#ifndef _ITEM_FORGING_H_
#define _ITEM_FORGING_H_

//#include "CBaseRandomItem.h"

#include "core/types/types.h"
#include "core/containers/arstring.h"
#include "after/entities/item/weapon/ItemAdditives.h"

class CWeaponItem;

struct tForgeRecipe
{
	// result item type
	ItemType	type;
	// required forge level to create (tech level)
	int forgeLevel;
	// base additive required for the recipie, None if not needing specific additive
	ItemAdditives::eItemAdditive	req_additive;
	// base item type required for the recipe, TypeDefault if not needing specific item
	ItemType	req_item;
	// additives required for this recipie to have parts unlocked, None if not needing specific additive
	ItemAdditives::eItemAdditive	req_additives [4];
	// items required for the recipies in the additives area, TypeDefault if not needing specific item
	ItemType	req_items [4];

	// if has been unlocked
	bool unlocked;

	// valid components to choose for this recipe.
	// values are a ; delimited list of names
	// to denote no component is an option, add empty value (";;")
	// Incomplete names are matched to a selection of parts
	arstring<256> componentsMain;
	arstring<256> componentsSecondary;
	arstring<256> components3;
	arstring<256> components4;

	// constructor
	tForgeRecipe ( void )
		: unlocked(true),
		type(WeaponItem::TypeDefault), forgeLevel(0),
		req_additive(ItemAdditives::None), req_item(WeaponItem::TypeDefault),
		componentsMain(";"), componentsSecondary(";"), components3(";"), components4(";")
	{
		req_additives[0] = ItemAdditives::None;
		req_additives[1] = ItemAdditives::None;
		req_additives[2] = ItemAdditives::None;
		req_additives[3] = ItemAdditives::None;
		req_items[0] = WeaponItem::TypeDefault;
		req_items[1] = WeaponItem::TypeDefault;
		req_items[2] = WeaponItem::TypeDefault;
		req_items[3] = WeaponItem::TypeDefault;
	}
};

struct tForgePart
{
	// String ID for the component
	arstring<128> componentIdentifier;
	// Not a clue.
	int		componentid;
	// 0 for main, 1 for secondary, 2 for 3, and 3 for 4 HAHAHAHA
	uchar	partType;
	// If part is unlocked
	bool	unlocked;
};

struct tForgeInput
{
	WeaponItem::WeaponItemType targetType;
	CWeaponItem* base;
	CWeaponItem* additives[4];
	CWeaponItem* enchantments[2];
};


#endif//_ITEM_FORGING_H_