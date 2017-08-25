
#ifndef _C_RANDOM_WEAPON_BOW_SHITTY_H_
#define _C_RANDOM_WEAPON_BOW_SHITTY_H_

#include "after/entities/item/weapon/CBaseRandomBow.h"

class CRWBowShitty : public CBaseRandomBow
{
	WITEM_DATA_DEFINE
	{ WITEMDATA_DEFAULTS;
		wpdata.iItemId = 201;

		wpdata.sInfo = "Basic Bow";
		wpdata.bCanStack = false;
		wpdata.fWeight = 7.0f;

		wpdata.eItemRarity	= WeaponItem::RarityNone;
		wpdata.eItemType	= WeaponItem::TypeWeapon;
		wpdata.eTopType		= WeaponItem::ItemBow;

		return wpdata;
	}
public:
	explicit CRWBowShitty ( void );
	explicit CRWBowShitty ( tBowWeaponProperties& inProps );
	~CRWBowShitty ( void );

	void Generate ( void );
};

#endif//_C_RANDOM_WEAPON_BOW_SHITTY_H_