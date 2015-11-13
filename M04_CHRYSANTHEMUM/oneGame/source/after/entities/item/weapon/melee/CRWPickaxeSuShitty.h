
#ifndef _C_RANDOM_WEAPON_PICKAXE_SUPER_SHITTY_H_
#define _C_RANDOM_WEAPON_PICKAXE_SUPER_SHITTY_H_

#include "after/entities/item/weapon/CBaseRandomMelee.h"

class CRWPickaxeSuShitty : public CBaseRandomMelee
{
	WITEM_DATA_DEFINE
	{ WITEMDATA_DEFAULTS;
		wpdata.iItemId = 300;

		wpdata.sInfo = "Cruddy Pickaxe";
		wpdata.bCanStack = false;
		wpdata.fWeight = 7.0f;
		wpdata.iHands = 2;

		wpdata.eItemRarity	= WeaponItem::RarityNone;
		wpdata.eItemType	= WeaponItem::TypeWeaponPickaxe;
		wpdata.eTopType		= WeaponItem::ItemAxe;

		wpdata.bCanDegrade	= true;
		wpdata.iMaxDurability= 1000;

		return wpdata;
	}
public:
	explicit CRWPickaxeSuShitty ( void );
	explicit CRWPickaxeSuShitty ( tMeleeWeaponProperties& inProps );
	~CRWPickaxeSuShitty ( void );

	void Generate ( void );

	void SetBlockHardness ( void ) override;
protected:
	int iGripType;
	int iBladeType;
};

#endif//_C_RANDOM_WEAPON_PICKAXE_SUPER_SHITTY_H_