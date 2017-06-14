
#ifndef _C_RANDOM_WEAPON_SWORD_SHORT_SHITTY_H_
#define _C_RANDOM_WEAPON_SWORD_SHORT_SHITTY_H_

#include "after/entities/item/weapon/CBaseRandomMelee.h"

class CRWSwordShortShitty : public CBaseRandomMelee
{
	WITEM_DATA_DEFINE
	{ WITEMDATA_DEFAULTS;
		wpdata.iItemId = 256;

		wpdata.sInfo = "Short Sword";
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
	explicit CRWSwordShortShitty ( void );
	explicit CRWSwordShortShitty ( tMeleeWeaponProperties& inProps, const char* bladeModel, const char* gripModel );
	~CRWSwordShortShitty ( void );

	void Generate ( void );

protected:
	//int iGripType;
	//int iBladeType;
	arstring<128> sBladeModel;
	arstring<128> sGripModel;
};

#endif