
#ifndef _C_RANDOM_WEAPON_HOE_
#define _C_RANDOM_WEAPON_HOE_

#include "after/entities/item/weapon/CBaseRandomMelee.h"

class CRWHoe : public CBaseRandomMelee
{
	WITEM_DATA_DEFINE
	{ WITEMDATA_DEFAULTS;
		wpdata.iItemId = 302;

		wpdata.sInfo = "Hoe";
		wpdata.bCanStack = false;
		wpdata.fWeight = 5.0f;

		wpdata.eItemRarity	= WeaponItem::RarityNone;
		wpdata.eItemType	= WeaponItem::TypeWeaponPickaxe;
		wpdata.eTopType		= WeaponItem::ItemAxe;

		wpdata.bCanDegrade	= true;
		wpdata.iMaxDurability= 1000;

		return wpdata;
	}
public:
	explicit CRWHoe ( void );
	~CRWHoe ( void );

	// Pass in component names, then call Generate.
	// (For now, ignore, and just load pickaxe model)
	void Generate ( void ); 

	// Hoe has a special use. Left click will till dirt instead of dig it.
	// It is not used for digging otherwise. 
	//bool Use ( int use ) override;
	void Attack ( XTransform& ) override;
protected:

	void DoBlockFarming ( void );
};

#endif//_C_RANDOM_WEAPON_HOE_