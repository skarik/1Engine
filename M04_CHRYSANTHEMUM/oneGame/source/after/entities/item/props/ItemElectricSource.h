
#ifndef _ITEM_ELECTRIC_SOURCE_
#define _ITEM_ELECTRIC_SOURCE_

#include "after/entities/item/CWeaponItem.h"
#include "renderer/light/CLight.h"

class ItemElectricSource : public CWeaponItem
{
	WITEM_DATA_DEFINE
	{ WITEMDATA_DEFAULTS;
		wpdata.iItemId = 101;
		wpdata.sInfo = "Electric Source (test)";
		wpdata.bCanStack = 1;
		wpdata.iMaxStack = 25;
		wpdata.fWeight = 1.0f;
		return wpdata;
	}
public:
	ItemElectricSource ( void );
	~ItemElectricSource ( void );

	// Create common
	void InitializeCommon ( void );

	// Update function
	void Update ( void );

	// Use function
	bool Use( int x );

private:
};

#endif