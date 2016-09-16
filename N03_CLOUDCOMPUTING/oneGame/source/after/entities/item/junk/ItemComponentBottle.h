
#ifndef _ITEM_COMPONENT_BOTTLE_H_
#define _ITEM_COMPONENT_BOTTLE_H_

#include "CItemJunkBase.h"
#include "renderer/logic/model/CModel.h"

class ItemComponentBottle : public CItemJunkBase
{

protected:
	WITEM_DATA_DEFINE
	{ WITEMDATA_DEFAULTS;
		wpdata.iItemId = 2000;

		wpdata.sInfo = "Bottle";
		wpdata.bCanStack = true;
		wpdata.iMaxStack = 20;
		wpdata.fWeight = 1.0f;

		return wpdata;
	}
public:
	ItemComponentBottle ( void ) : CItemJunkBase( tJunkStats(true,false,false), ItemData() )
	{
		pModel = new CModel( "models/items/bottle.FBX" );
	}
		 
};

#endif//_ITEM_COMPONENT_BOTTLE_H_