
#ifndef _ENGINECOMMON_ITEM_BASE_
#define _ENGINECOMMON_ITEM_BASE_

#include "engine/behavior/CGameObject.h"

// Prototype
class CActor;

//===============================================================================================//
// CItemBase
// 
// item base class
//===============================================================================================//
class CItemBase : public CGameObject
{
public:
	// Activate the item. Weapons take the firing type as the argument.
	// Override this function to give your items a behavior.
	// Return false if this item cannot be used this way.
	virtual bool Use ( int ) =0;
	// Activating the item has just started. Weapons take the firing type as the argument.
	virtual void StartUse ( int ) =0;
	// Activating the item has ended. Weapons take the firing type as the argument.
	virtual void EndUse ( int ) =0;
	// Attack with the item. This is, straight up, an attack.
	// Triggered when the owner character animation recieves an 'attack' event.
	virtual void Attack ( XTransform& ) =0;
	// Subdraw the item. This is called when the item is selected again when already out.
	// This is also called when the "draw" button is pressed.
	virtual void SubDraw ( void ) =0;

	// These are the equip calls, which are called when the item is 'equipped.' The means the item was put on the belt or equipped otherwise.
	virtual void	OnEquip ( CActor* interactingActor ) =0;
	virtual void	OnUnequip ( CActor* interactingActor ) =0;

	// Interrupt public call for stopping actions.
	// This is called whenever the weaponItem owner switches weapons, gets stunned, or some other interrupting action happens.
	// This is virtual, and you SHOULD declare your own behaviour for your classes.
	// The default implementation stops holstering.
	virtual void Interrupt ( void ) { ; }
};

#endif//_ENGINECOMMON_ITEM_BASE_