
#ifndef _C_RANDOM_WEAPON_SHOVEL_SUPER_SHITTY_H_
#define _C_RANDOM_WEAPON_SHOVEL_SUPER_SHITTY_H_

#include "after/entities/item/weapon/CBaseRandomMelee.h"

class CRWShovelSuShitty : public CBaseRandomMelee
{
	WITEM_DATA_DEFINE
	{ WITEMDATA_DEFAULTS;
		wpdata.iItemId = 301;

		wpdata.sInfo = "Cruddy Shovel";
		wpdata.bCanStack = false;
		wpdata.fWeight = 6.0f;
		wpdata.iHands = 2;

		wpdata.eItemRarity	= WeaponItem::RarityNone;
		wpdata.eItemType	= WeaponItem::TypeWeaponShovel;
		wpdata.eTopType		= WeaponItem::ItemDigger;

		wpdata.bCanDegrade	= true;
		wpdata.iMaxDurability= 1000;

		return wpdata;
	}
public:
	explicit CRWShovelSuShitty ( void );
	explicit CRWShovelSuShitty ( tMeleeWeaponProperties& inProps );
	~CRWShovelSuShitty ( void );

	void Generate ( void );

	void SetBlockHardness ( void ) override;
protected:
	int iGripType;
	int iBladeType;
};

#endif//_C_RANDOM_WEAPON_SHOVEL_SUPER_SHITTY_H_