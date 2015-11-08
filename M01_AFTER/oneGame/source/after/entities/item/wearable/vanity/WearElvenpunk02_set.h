
#ifndef _WEAR_ELVENPUNK_02_SET_H_
#define _WEAR_ELVENPUNK_02_SET_H_

#include "../CWearableItem.h"

class WearElvenpunk02_shirt : public CWearableItem
{
protected:
	WITEM_DATA_DEFINE
	{ WITEMDATA_DEFAULTS;
		wpdata.iItemId = 1102;

		wpdata.sInfo = "Shirt";
		wpdata.bCanStack = false;
		wpdata.fWeight = 5.0f;

		wpdata.eItemRarity	= WeaponItem::RarityCommon;
		wpdata.eItemType	= WeaponItem::TypeArmorChest;
		wpdata.eTopType		= WeaponItem::ItemArmor;

		return wpdata;
	}

public:
	explicit WearElvenpunk02_shirt ( void ) : CWearableItem(ItemData())
	{
		sPlayerModel = "elvenpunk02/shirt";
		sWorldModel = ".res/models/items/tree_resin.FBX";
		bHideBody = false;
	}
	~WearElvenpunk02_shirt (void)
	{
		;
	}
};

class WearElvenpunk02_cloak : public CWearableItem
{
protected:
	WITEM_DATA_DEFINE
	{ WITEMDATA_DEFAULTS;
		wpdata.iItemId = 1103;

		wpdata.sInfo = "Cloak";
		wpdata.bCanStack = false;
		wpdata.fWeight = 5.0f;

		wpdata.eItemRarity	= WeaponItem::RarityCommon;
		wpdata.eItemType	= WeaponItem::TypeArmorShoulder;
		wpdata.eTopType		= WeaponItem::ItemArmor;

		return wpdata;
	}

public:
	explicit WearElvenpunk02_cloak ( void ) : CWearableItem(ItemData())
	{
		sPlayerModel = "elvenpunk02/cloak";
		sWorldModel = ".res/models/items/tree_resin.FBX";
		bHideBody = false;
		bHideHair = true;
	}
	~WearElvenpunk02_cloak (void)
	{
		;
	}
};


class WearElvenpunk02_boots : public CWearableItem
{
protected:
	WITEM_DATA_DEFINE
	{ WITEMDATA_DEFAULTS;
		wpdata.iItemId = 1104;

		wpdata.sInfo = "Boots";
		wpdata.bCanStack = false;
		wpdata.fWeight = 5.0f;

		wpdata.eItemRarity	= WeaponItem::RarityCommon;
		wpdata.eItemType	= WeaponItem::TypeArmorBoots;
		wpdata.eTopType		= WeaponItem::ItemArmor;

		return wpdata;
	}

public:
	explicit WearElvenpunk02_boots ( void ) : CWearableItem(ItemData())
	{
		sPlayerModel = "elvenpunk02/boots";
		sWorldModel = ".res/models/items/tree_resin.FBX";
		bHideBody = true;
	}
	~WearElvenpunk02_boots (void)
	{
		;
	}
};


#endif//_WEAR_ELVENPUNK_02_SET_H_
