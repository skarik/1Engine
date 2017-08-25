
#include "after/entities/item/CWeaponItem.h"
#include "CWeaponItemFactory.h"

WeaponItem::CWeaponItemFactory WeaponItem::WeaponItemFactory;

// Include
// Everything
// God damn it
#include "after/lua/CLuaWeaponItem.h"

#include "after/entities/item/beta/Flare.h"
#include "after/entities/item/beta/WeaponCrossbow.h"

#include "after/entities/item/ammo/ItemPebble.h"
#include "after/entities/item/ammo/AmmoCrossbowBolt.h"
#include "after/entities/item/ammo/AmmoArrow.h"

#include "after/entities/item/junk/ItemComponentBottle.h"
#include "after/entities/item/junk/ItemJunkSkull.h"

#include "after/entities/item/material/ItemTreeTrunk.h"
#include "after/entities/item/material/ItemTreeBranch.h"
#include "after/entities/item/material/ItemTreeTwig.h"
#include "after/entities/item/material/ItemTreeResin.h"

#include "after/entities/item/props/flora/ItemFloraSnowLily.h"
#include "after/entities/item/props/flora/ItemFloraLifedrop.h"

#include "after/entities/item/props/ItemCampfire.h"
#include "after/entities/item/props/ItemCrateWooden.h"
#include "after/entities/item/props/ItemElectricLamp.h"
#include "after/entities/item/props/ItemElectricSource.h"
#include "after/entities/item/props/ItemLantern.h"
#include "after/entities/item/props/ItemTorch.h"

#include "after/entities/item/system/ItemTerraBlok.h"

#include "after/entities/item/weapon/bow/CRWBowShitty.h"

#include "after/entities/item/weapon/melee/CRWDagger.h"
#include "after/entities/item/weapon/melee/CRWHoe.h"
#include "after/entities/item/weapon/melee/CRWPickaxeSuShitty.h"
#include "after/entities/item/weapon/melee/CRWShovelSuShitty.h"
#include "after/entities/item/weapon/melee/CRWSpear.h"
#include "after/entities/item/weapon/melee/CRWSword.h"
#include "after/entities/item/weapon/melee/CRWSwordShortShitty.h"

#include "after/entities/item/weapon/tool/CRWDrillBasic.h"

#include "after/entities/item/weapon/tossable/CRIDynamite.h"

#include "after/entities/item/wearable/armor/ArmorLeatherBoots.h"
#include "after/entities/item/wearable/armor/ArmorLeatherPants.h"

#include "after/entities/item/wearable/vanity/WearElvenpunk02_goggles.h"
#include "after/entities/item/wearable/vanity/WearElvenpunk02_skirt.h"
#include "after/entities/item/wearable/vanity/WearElvenpunk02_set.h"

void WeaponItem::CWeaponItemFactory::RegisterTypes ( void )
{
	RegisterWItemEx( CLuaWeaponItem, 3 );

	RegisterWItemEx( Flare, 1 );
	RegisterWItemEx( ItemTerraBlok, 2 );

	RegisterWItemEx( ItemTorch, 4 );

	RegisterWItemEx( ItemTreeResin, 6 );

	RegisterWItemEx( AmmoCrossbowBolt, 10 );
	RegisterWItemEx( AmmoArrow, 11 );
	RegisterWItemEx( ItemPebble, 12 );

	RegisterWItemEx( ItemTreeTrunk, 50 );
	RegisterWItemEx( ItemTreeBranch, 51 );
	RegisterWItemEx( ItemTreeTwig, 52 );

	RegisterWItemEx( ItemLantern, 90 );
	RegisterWItemEx( ItemCampfire, 91 );

	RegisterWItemEx( ItemElectricLamp, 100 );
	RegisterWItemEx( ItemElectricSource, 101 );

	RegisterWItemEx( CRIDynamite, 150 );

	RegisterWItemEx( WeaponCrossbow, 200 );
	RegisterWItemEx( CRWBowShitty, 201 );

	//RegisterWItemEx( CRWStoneAxeShitty, 220 );

	RegisterWItemEx( CRWDrillBasic, 250 );

	RegisterWItemEx( CRWSwordShortShitty, 256 );
	RegisterWItemEx( CRWSword, 257 );
	RegisterWItemEx( CRWDagger, 258 );
	RegisterWItemEx( CRWSpear, 259 );

	RegisterWItemEx( CRWPickaxeSuShitty, 300 );
	RegisterWItemEx( CRWShovelSuShitty, 301 );
	RegisterWItemEx( CRWHoe, 302 );

	RegisterWItemEx( ItemFloraSnowLily, 500 );
	RegisterWItemEx( ItemFloraLifedrop, 501 );

	RegisterWItemEx( ArmorLeatherBoots, 1000 );
	RegisterWItemEx( ArmorLeatherPants, 1001 );

	RegisterWItemEx( WearElvenpunk02_goggles, 1100 );
	RegisterWItemEx( WearElvenpunk02_skirt, 1101 );
	RegisterWItemEx( WearElvenpunk02_shirt, 1102 );
	RegisterWItemEx( WearElvenpunk02_cloak, 1103 );
	RegisterWItemEx( WearElvenpunk02_boots, 1104 );

	RegisterWItemEx( ItemComponentBottle, 2000 );

	RegisterWItemEx( ItemCrateWooden, 3000 );

	RegisterWItemEx( ItemJunkSkull, 4000 );
}

string CWeaponItem::ItemName ( const short itemid )
{
	return WeaponItem::WeaponItemFactory.witem_name[itemid];
}
string CWeaponItem::ItemName ( const type_info & itemid )
{
	return WeaponItem::WeaponItemFactory.witem_name[WeaponItem::WeaponItemFactory.witem_hash[itemid.name()]];
}


CWeaponItem* CWeaponItem::Instantiate ( const short itemid )
{
	if ( WeaponItem::WeaponItemFactory.witem_inst.count(itemid) ) {
		CWeaponItem* item = WeaponItem::WeaponItemFactory.witem_inst[itemid]();
		return item;
	}
	std::cout << "COULD NOT FIND ID TO INSTANTIATE" << std::endl;
	return NULL;
}
CWeaponItem* CWeaponItem::Instantiate ( const string & itemid )
{
	short id = WeaponItem::WeaponItemFactory.witem_swap[itemid];
	CWeaponItem* result = Instantiate( id );
	result->weaponItemData.iItemId = id;
	return result;
}
CWeaponItem* CWeaponItem::Instantiate ( const type_info & itemid )
{
	return Instantiate( WeaponItem::WeaponItemFactory.witem_hash[itemid.name()] );
}
template <class WI>
CWeaponItem* CWeaponItem::Instantiate ( void )
{
	return Instantiate( typeid(WI).name() );
}

CWeaponItem* CWeaponItem::Dupe ( void )
{
	//CWeaponItem* copy = Instantiate( typeid(this) );
	CWeaponItem* copy = Instantiate( typeid(*this) );
	(*copy) = this;
	return copy;
}