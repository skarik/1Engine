
#ifndef _WEAR_ELVENPUNK_02_SKIRT_H_
#define _WEAR_ELVENPUNK_02_SKIRT_H_

#include "../CWearableItem.h"

class WearElvenpunk02_skirt : public CWearableItem
{
protected:
	WITEM_DATA_DEFINE
	{ WITEMDATA_DEFAULTS;
		wpdata.iItemId = 1101;

		wpdata.sInfo = "Skirt";
		wpdata.bCanStack = false;
		wpdata.fWeight = 5.0f;

		wpdata.eItemRarity	= WeaponItem::RarityCommon;
		wpdata.eItemType	= WeaponItem::TypeArmorGreaves;
		wpdata.eTopType		= WeaponItem::ItemArmor;

		return wpdata;
	}

public:
	explicit WearElvenpunk02_skirt ( void ) : CWearableItem(ItemData())
	{
		sPlayerModel = "elvenpunk02/skirt";
		sWorldModel = ".res/models/items/tree_resin.FBX";
		bHideBody = false;
	}
	~WearElvenpunk02_skirt (void)
	{
		;
	}
};


#endif//_WEAR_ELVENPUNK_02_SKIRT_H_
