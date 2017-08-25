
//
//
// CAmmoBase
// 
// Base class for ammo type items. Specific check on isEqual to deal with buffs.
// Also has specific structures to store ammo types.
//

#include "CAmmoBase.h"

CAmmoBase::CAmmoBase ( const WItemData & wdat )
	: CWeaponItem( wdat )
{
	;
}

CAmmoBase::~CAmmoBase ( void )
{
	;
}


bool CAmmoBase::IsEqual ( CWeaponItem* compare )
{
	return CWeaponItem::IsEqual( compare );
}
