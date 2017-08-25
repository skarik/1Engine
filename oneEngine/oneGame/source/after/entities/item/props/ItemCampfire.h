
#ifndef _ITEM_CAMPFIRE_H_
#define _ITEM_CAMPFIRE_H_

#include "after/entities/item/CWeaponItem.h"
//#include "after/entities/props/CTerrainProp.h"
//#include "CLight.h"

class ItemCampfire : public CWeaponItem
{
	WITEM_DATA_DEFINE
	{ WITEMDATA_DEFAULTS;
		wpdata.iItemId = 91;
		wpdata.sInfo = "Campfire Kit";
		wpdata.bCanStack = false;
		wpdata.fWeight = 1.0f;
		return wpdata;
	}
public:
	ItemCampfire ( void );
	~ItemCampfire ( void );

	// Use function
	bool Use( int x );

	// Lookat calls
	void	OnInteractLookAt	( CActor* interactingActor );
	void	OnInteractLookAway	( CActor* interactingActor );
};

#endif//_ITEM_CAMPFIRE_H_