
#ifndef _ARMOR_LEATHER_BOOTS_H_
#define _ARMOR_LEATHER_BOOTS_H_

#include "../CWearableItem.h"

class ArmorLeatherBoots : public CWearableItem
{
protected:
	WITEM_DATA_DEFINE
	{ WITEMDATA_DEFAULTS;
		wpdata.iItemId = 1000;

		wpdata.sInfo = "Leather Boots";
		wpdata.bCanStack = false;
		wpdata.fWeight = 4.0f;

		wpdata.eItemRarity	= WeaponItem::RarityCommon;
		wpdata.eItemType	= WeaponItem::TypeArmorBoots;
		wpdata.eTopType		= WeaponItem::ItemArmor;

		return wpdata;
	}

public:
	explicit ArmorLeatherBoots ( void ) : CWearableItem(ItemData())
	{
		sPlayerModel = "leatherset01/leatherset01_boots";
		sWorldModel = "models/items/tree_resin.FBX";
		bHideBody = true;
	}
	~ArmorLeatherBoots (void)
	{
		;
	}
};


#endif//_ARMOR_LEATHER_BOOTS_H_
