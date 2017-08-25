
#ifndef _WEAPON_CROSSBOW_
#define _WEAPON_CROSSBOW_

#include "after/entities/item/CWeaponItem.h"

class WeaponCrossbow : public CWeaponItem
{
	ClassName( "WeaponCrossbow" );

	WITEM_DATA_DEFINE
	{ WITEMDATA_DEFAULTS;
		wpdata.iItemId = 200;
		wpdata.sInfo = "Crossbow";
		wpdata.bCanStack = false;
		wpdata.iMaxStack = 1;
		wpdata.fWeight = 13.0f;
		wpdata.iHands = 2;
		return wpdata;
	}
public:
	WeaponCrossbow ( void );

	bool Use ( int );
};


#endif