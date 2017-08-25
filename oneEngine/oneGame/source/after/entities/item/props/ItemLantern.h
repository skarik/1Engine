
#ifndef _ITEM_LANTERN_H_
#define _ITEM_LANTERN_H_

#include "after/entities/item/CWeaponItem.h"

class CLight;
class CParticleSystem;

class ItemLantern : public CWeaponItem
{
	ClassName( "ItemLantern" );
protected:
	WITEM_DATA_DEFINE
	{ WITEMDATA_DEFAULTS;

		wpdata.iItemId = 90;

		wpdata.sInfo = "Lantern";
		wpdata.bCanStack = false;
		wpdata.iMaxStack = 1;
		wpdata.fWeight = 1.0f;

		wpdata.bCanDegrade = true;
		wpdata.iMaxDurability = 500;

		return wpdata;
	}
public:
	// == Constructor ==
	ItemLantern ( void );

	// == Destructor ==
	~ItemLantern ( void );
	
	// Update function
	void Update ( void );

	// Use function
	bool Use( int x );
	
private:
	CLight* pLight;
	CParticleSystem* pFireSystem;

	ftype degradeTimer;
};

#endif