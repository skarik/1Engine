
#ifndef _ITEM_FLORA_LIFEDROP_H_
#define _ITEM_FLORA_LIFEDROP_H_

#include "after/entities/item/CWeaponItem.h"

class ItemFloraLifedrop : public CWeaponItem
{
	ClassName( "ItemFloraLifedrop" );
protected:
	WITEM_DATA_DEFINE
	{ WITEMDATA_DEFAULTS;

		wpdata.iItemId = 501;

		wpdata.sInfo = "Lifedrop Leaf";
		wpdata.bCanStack = true;
		wpdata.iMaxStack = 25;
		wpdata.fWeight = 0.2f;

		return wpdata;
	}
public:
	// == Constructor ==
	ItemFloraLifedrop ( void );
	// == Destructor ==
	~ItemFloraLifedrop ( void );
	// Update function
	//void Update ( void );
	// Use function
	bool Use( int x ) { return false; }
};

#endif//_ITEM_FLORA_LIFEDROP_H_