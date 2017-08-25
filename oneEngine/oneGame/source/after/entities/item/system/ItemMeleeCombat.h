// class ItemMeleeCombat
// 
// Item class that controls the baseline melee system
// 


#ifndef _ITEM_MELEE_COMBAT_H_
#define _ITEM_MELEE_COMBAT_H_

#include "after/entities/item/CWeaponItem.h"


class ItemMeleeCombat : public CWeaponItem
{
	ClassName( "ItemMeleeCombat" );
public:
	ItemMeleeCombat();
	~ItemMeleeCombat();

	// Override the CreatePhysics function.
	// This object does not have a physics object, nor a model. To prevent engine error spam, we override it.
	void CreatePhysics ( void ) override;

	// Override update
	void Update ( void ) override;

	// Use function
	bool Use ( int x )		override;
	// Activating the item has just started. Weapons take the firing type as the argument.
	void StartUse ( int )	override;
	// Activating the item has ended. Weapons take the firing type as the argument.
	void EndUse ( int )		override;
	// Attack function
	void Attack ( XTransform& )	override;

	// These are the equip calls, which are called when the item is 'equipped.' The means the item was put on the belt or equipped otherwise.
	//void	OnPlaceinBelt ( CActor* interactingActor ) {}
	//void	OnEquip ( CActor* interactingActor ) {}

	// Need it to work on equip
	void	OnEquip ( CActor* interactingActor ) override;
	// Need it to work on unequip
	void	OnUnequip ( CActor* interactingActor ) override;

	// Clang Checking ??!??
	void ClangCheck ( void );

	enum eWeaponState_t
	{
		ATTACKSTATE_IDLE,
		ATTACKSTATE_ATKGROUND,
		ATTACKSTATE_ATKDUCK,
		ATTACKSTATE_ATKSPRINT,
		ATTACKSTATE_ATKAIR,
		ATTACKSTATE_ATKSLIDE
	};

protected:
	Real	chargeTime_Heavy;	// Constant value for time for heavy attack
	Real	chargeTime;

	bool	charged;			// Is an attack queued?

	eWeaponState_t	attackstate;
};


#endif//_ITEM_MELEE_COMBAT_H_