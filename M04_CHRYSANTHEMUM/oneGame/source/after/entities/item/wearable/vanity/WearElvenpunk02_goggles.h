
#ifndef _WEAR_ELVENPUNK_02_GOGGLES_H_
#define _WEAR_ELVENPUNK_02_GOGGLES_H_

#include "../CWearableItem.h"

class WearElvenpunk02_goggles : public CWearableItem
{
protected:
	WITEM_DATA_DEFINE
	{ WITEMDATA_DEFAULTS;
		wpdata.iItemId = 1100;

		wpdata.sInfo = "Leather Pants";
		wpdata.bCanStack = false;
		wpdata.fWeight = 5.0f;

		wpdata.eItemRarity	= WeaponItem::RarityCommon;
		wpdata.eItemType	= WeaponItem::TypeArmorHead;
		wpdata.eTopType		= WeaponItem::ItemArmor;

		return wpdata;
	}

public:
	explicit WearElvenpunk02_goggles ( void ) : CWearableItem(ItemData())
	{
		sPlayerModel = "elvenpunk02/goggles";
		sWorldModel = ".res/models/items/tree_resin.FBX";
		bHideBody = false;
	}
	~WearElvenpunk02_goggles (void)
	{
		;
	}
};


#endif//_WEAR_ELVENPUNK_02_GOGGLES_H_
