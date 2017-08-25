
#ifndef _ITEM_FLORA_SNOW_LILY_H_
#define _ITEM_FLORA_SNOW_LILY_H_

#include "after/entities/item/CWeaponItem.h"

class ItemFloraSnowLily : public CWeaponItem
{
	ClassName( "ItemFloraSnowLily" );
protected:
	WITEM_DATA_DEFINE
	{ WITEMDATA_DEFAULTS;

		wpdata.iItemId = 500;

		wpdata.sInfo = "Taliforina Snowdrop Lily";
		wpdata.bCanStack = true;
		wpdata.iMaxStack = 25;
		wpdata.fWeight = 0.3f;

		return wpdata;
	}
public:
	// == Constructor ==
	ItemFloraSnowLily ( void );
	// == Destructor ==
	~ItemFloraSnowLily ( void );
	// Update function
	//void Update ( void );
	// Use function
	bool Use( int x );
};

#endif//_ITEM_FLORA_SNOW_LILY_H_