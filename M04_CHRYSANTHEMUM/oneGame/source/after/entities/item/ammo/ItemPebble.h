
#ifndef _ITEM_PEBBLE_H_
#define _ITEM_PEBBLE_H_

#include "CAmmoBase.h"

#include "renderer/logic/model/CModel.h"

class ItemPebble : public CAmmoBase
{
	WITEM_DATA_DEFINE
	{ WITEMDATA_DEFAULTS;
		wpdata.iItemId = 12;
		wpdata.sInfo = "Pebble";
		wpdata.bCanStack = true;
		wpdata.iMaxStack = 55;
		wpdata.fWeight = 0.1f;
		return wpdata;
	}
public:
	ItemPebble ( void ) : CAmmoBase( ItemData() )
	{
		pModel = new CModel( string(".res/models/world/pebble.FBX") );
	}

	bool	Use ( int x )
	{
		return false;
	}
};

#endif