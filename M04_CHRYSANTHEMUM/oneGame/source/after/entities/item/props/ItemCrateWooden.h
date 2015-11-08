
#ifndef _ITEM_CRATE_WOODEN_H_
#define _ITEM_CRATE_WOODEN_H_

#include "after/entities/item/CWeaponItem.h"

class ItemCrateWooden : public CWeaponItem
{
	WITEM_DATA_DEFINE
	{ WITEMDATA_DEFAULTS;
		wpdata.iItemId = 3000;
		wpdata.sInfo = "Wooden Crate";
		wpdata.bCanStack = false;
		wpdata.fWeight = 1.0f;
		return wpdata;
	}
public:
	ItemCrateWooden ( void );
	~ItemCrateWooden ( void );

	// Use function
	bool Use( int x );

	// Lookat calls
	//void	OnInteractLookAt	( CActor* interactingActor );
	//void	OnInteractLookAway	( CActor* interactingActor );
};

#endif//_ITEM_CRATE_WOODEN_H_