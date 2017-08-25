
#ifndef _C_RANDOM_WEAPON_DAGGER_H_
#define _C_RANDOM_WEAPON_DAGGER_H_

#include "after/entities/item/weapon/CBaseRandomMelee.h"

class CRWDagger : public CBaseRandomMelee
{
	WITEM_DATA_DEFINE
	{ WITEMDATA_DEFAULTS;
		wpdata.iItemId = 258;

		wpdata.sInfo = "Dagger";
		wpdata.bCanStack = false;
		wpdata.fWeight = 3.0f;

		wpdata.eItemRarity	= WeaponItem::RarityNone;
		wpdata.eItemType	= WeaponItem::TypeWeaponKnife;
		wpdata.eTopType		= WeaponItem::ItemBlade;

		wpdata.bCanDegrade	= true;
		wpdata.iMaxDurability= 1000;

		return wpdata;
	}
public:
	explicit CRWDagger ( void );
	~CRWDagger ( void );

	// Pass in component names, then call Generate.
	void Generate ( void ); 

protected:

};

#endif//_C_RANDOM_WEAPON_DAGGER_H_