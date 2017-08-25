
#ifndef _C_RANDOM_WEAPON_SPEAR_H_
#define _C_RANDOM_WEAPON_SPEAR_H_

#include "after/entities/item/weapon/CBaseRandomMelee.h"

class CRWSpear : public CBaseRandomMelee
{
	WITEM_DATA_DEFINE
	{ WITEMDATA_DEFAULTS;
		wpdata.iItemId = 259;

		wpdata.sInfo = "Spear";
		wpdata.bCanStack = false;
		wpdata.fWeight = 7.0f;

		wpdata.eItemRarity	= WeaponItem::RarityNone;
		wpdata.eItemType	= WeaponItem::TypeWeaponPike;
		wpdata.eTopType		= WeaponItem::ItemPolearm;

		wpdata.bCanDegrade	= true;
		wpdata.iMaxDurability= 1000;

		return wpdata;
	}
public:
	explicit CRWSpear ( void );
	//explicit CRWSword ( tMeleeWeaponProperties& inProps );
	~CRWSpear ( void );

	// Pass in component names, then call Generate.
	void Generate ( void ); 

protected:

};

#endif//_C_RANDOM_WEAPON_SPEAR_H_