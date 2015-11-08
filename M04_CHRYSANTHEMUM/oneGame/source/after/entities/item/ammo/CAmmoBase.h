
//
//
// CAmmoBase
// 
// Base class for ammo type items. Specific check on isEqual to deal with buffs.
// Also has specific structures to store ammo types.
//

#ifndef _C_AMMO_BASE_H_
#define _C_AMMO_BASE_H_

#include "after/entities/item/CWeaponItem.h"

class CAmmoBase : public CWeaponItem
{
	ClassName( "AbstractAmmoClass" );
public:
	CAmmoBase ( const WItemData & wdat );
	~CAmmoBase ( void );

	// Comparison operator
	bool IsEqual ( CWeaponItem* compare ) override;

};

#endif

