
#ifndef _C_RANDOM_WEAPON_SWORD_H_
#define _C_RANDOM_WEAPON_SWORD_H_

#include "after/entities/item/weapon/CBaseRandomMelee.h"

class CRWSword : public CBaseRandomMelee
{
	WITEM_DATA_DEFINE
	{ WITEMDATA_DEFAULTS;
		wpdata.iItemId = 257;

		wpdata.sInfo = "Sword";
		wpdata.bCanStack = false;
		wpdata.fWeight = 5.0f;

		wpdata.eItemRarity	= WeaponItem::RarityNone;
		wpdata.eItemType	= WeaponItem::TypeWeaponSword;
		wpdata.eTopType		= WeaponItem::ItemBlade;

		wpdata.bCanDegrade	= true;
		wpdata.iMaxDurability= 1000;

		return wpdata;
	}
public:
	explicit CRWSword ( void );
	//explicit CRWSword ( tMeleeWeaponProperties& inProps );
	~CRWSword ( void );

	// Pass in component names, then call Generate.
	void Generate ( void ); 

protected:

};

#endif//_C_RANDOM_WEAPON_SWORD_H_