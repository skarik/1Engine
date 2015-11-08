#ifndef _CINVENTORY_H_
#define _CINVENTORY_H_

#include "engine/behavior/CGameBehavior.h"

#include "engine-common/entities/CActor.h"

#include "after/entities/item/CWeaponItem.h"
#include "after/entities/item/wearable/CWearableItem.h"
#include "after/entities/item/skill/CSkill.h"

//#include "core/settings/CGameSettings.h"

class CInventory /*: public CGameBehavior*/
{
public:
	explicit CInventory( CActor* );
	explicit CInventory( CActor*, short w, short h);
	~CInventory(void);

	// Update function. Checks for invalid objects and other per-frame checks.
	void Update (void);
	
	// Return the actore that owns this pInventory
	CActor* GetOwner (void);
	//Return the height of the pInventory
	virtual short GetHeight (void);
	//Return the width of the pInventory
	virtual short GetWidth (void);

	//Throws out an item from the pInventory. Permanently deletes it
	void Trash (CWeaponItem* junk);
	//Drop an item onto the ground
	void Drop (CWeaponItem* junk);
	//Add an item to the pInventory
	virtual bool AddItem (CWeaponItem* treasure);

	//Search through the player pInventory
	//Returns an index for the pInventory array for either an open spot or a current stack of said item
	//Currently assumes that if an item can stack, it will stack (Does not check stack sizes).
	int CanAddItem (CWeaponItem* cheezburger);

	// Set the size of the equipped array
	virtual void			SetEquippedMaxCount ( short inHands ) {}
	//Return the pointer to the currently equipped item
	virtual CWeaponItem*	GetCurrentEquipped (short hand=0) { return NULL; }
	virtual short			GetCurrentEquippedCount ( void ) { return 0; }

	//Change the equipped item to the slot given
	virtual void	ChangeEquippedTo (int slot, bool allowUnequip=true) {}

	//Return the pointer to the items
	CWeaponItem** GetItems (void);
	//Get a single item from the pInventory
	virtual CWeaponItem* GetItem (short slot);
	//Find an item in the pInventory
	virtual CWeaponItem* FindItem (short id);

	//Switch the spots of two items
	void SwitchSpots (short source, short dest);
	//Place an item into the pInventory
	void ReceiveItem (CWeaponItem* item, short slot);

	//Add two stacks of items to each other
	virtual CWeaponItem* AddStacks (CWeaponItem* item, short slot);

	//Easy dereference because I'm getting lazy. Doesn't actually work yet
	CWeaponItem* operator () ( const unsigned char x, const unsigned char y )
	{
		if (pInventory[x+y*sWidth] != NULL)
			return pInventory[x+y*sWidth];
		else
			return NULL;
	}

	//Save the Inventory to the disk
	virtual void SaveInventory ( const arstring<256> & location);
	//Load the Inventory from the disk
	virtual bool LoadInventory ( const arstring<256> & location);

	virtual bool IsEmpty (void);

protected:
	CWeaponItem**	pInventory;

	CActor* Owner;
	
	short sWidth;  //Default: 6
	short sHeight; //Default: 4

	// Function to check all items and make sure they're actually valid
	virtual void CheckCurrentItems ( void );
	
};
#endif