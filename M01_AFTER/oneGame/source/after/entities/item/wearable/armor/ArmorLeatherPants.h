
#ifndef _ARMOR_LEATHER_PANTS_H_
#define _ARMOR_LEATHER_PANTS_H_

#include "../CWearableItem.h"

class ArmorLeatherPants : public CWearableItem
{
protected:
	WITEM_DATA_DEFINE
	{ WITEMDATA_DEFAULTS;
		wpdata.iItemId = 1001;

		wpdata.sInfo = "Leather Pants";
		wpdata.bCanStack = false;
		wpdata.fWeight = 5.0f;

		wpdata.eItemRarity	= WeaponItem::RarityCommon;
		wpdata.eItemType	= WeaponItem::TypeArmorGreaves;
		wpdata.eTopType		= WeaponItem::ItemArmor;

		return wpdata;
	}

public:
	explicit ArmorLeatherPants ( void ) : CWearableItem(ItemData())
	{
		sPlayerModel = "leatherset01/leatherset01_pants";
		sWorldModel = "models/items/tree_resin.FBX";
		bHideBody = true;
	}
	~ArmorLeatherPants (void)
	{
		;
	}
};


#endif//_ARMOR_LEATHER_PANTS_H_
