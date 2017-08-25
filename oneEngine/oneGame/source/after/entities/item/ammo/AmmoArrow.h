
#ifndef _AMMO_ARROW_H_
#define _AMMO_ARROW_H_

#include "CAmmoBase.h"

class AmmoArrow : public CAmmoBase
{
	WITEM_DATA_DEFINE
	{ WITEMDATA_DEFAULTS;
		wpdata.iItemId = 11;
		wpdata.sInfo = "Arrow";
		wpdata.bCanStack = true;
		wpdata.iMaxStack = 55;
		wpdata.fWeight = 2.0f;
		return wpdata;
	}
public:
	AmmoArrow ( void );

	bool Use ( int );
};

#endif