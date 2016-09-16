
#ifndef _AMMO_CROSSBOW_BOLT_H_
#define _AMMO_CROSSBOW_BOLT_H_

#include "CAmmoBase.h"

class AmmoCrossbowBolt : public CAmmoBase
{
	WITEM_DATA_DEFINE
	{ WITEMDATA_DEFAULTS;
		wpdata.iItemId = 10;
		wpdata.sInfo = "Crossbow Bolt";
		wpdata.bCanStack = true;
		wpdata.iMaxStack = 55;
		wpdata.fWeight = 2.0f;
		return wpdata;
	}
public:
	AmmoCrossbowBolt ( void );

	bool Use ( int );
};

#endif