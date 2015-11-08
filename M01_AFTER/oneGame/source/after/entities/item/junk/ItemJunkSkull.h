
#ifndef _ITEM_JUNK_SKULL_H_
#define _ITEM_JUNK_SKULL_H_

#include "CItemJunkBase.h"
#include "renderer/logic/model/CModel.h"

class ItemJunkSkull : public CItemJunkBase
{

protected:
	WITEM_DATA_DEFINE
	{ WITEMDATA_DEFAULTS;
		wpdata.iItemId = 4000;

		wpdata.sInfo = "Skull";
		wpdata.bCanStack = false;
		wpdata.fWeight = 4.0f;

		return wpdata;
	}
public:
	ItemJunkSkull ( void ) : CItemJunkBase( tJunkStats(true,true,false), ItemData() )
	{
		pModel = new CModel( ".res/models/props/skull.FBX" );
	}
		 
};

#endif//_ITEM_JUNK_SKULL_H_