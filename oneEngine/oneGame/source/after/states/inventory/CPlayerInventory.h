#ifndef _C_PLAYER_INVENTORY_H_
#define _C_PLAYER_INVENTORY_H_

//#include "engine/behavior/CGameBehavior.h"

#include "engine-common/entities/CActor.h"

#include "after/entities/item/CWeaponItem.h"
#include "after/entities/item/wearable/CWearableItem.h"
#include "after/entities/item/skill/CSkill.h"

#include "CInventory.h"

class CPlayerInventory : public CInventory
{

	ClassName ("PlayerInventory");

public:
	//explicit CPlayerInventory( CActor* );
	explicit CPlayerInventory( CActor*, short w, short h, short hands, bool adddebugitems=false );
	~CPlayerInventory(void);

	//Worry about this later
	void Update (void);
	
	//Return the height of the pInventory
	short GetHeight (void);
	//Return the width of the pInventory
	short GetWidth (void);

	//Add an item to the pInventory
	bool AddItem (CWeaponItem* treasure) override;

	//Search through the player pInventory
	//Returns an index for the pInventory array for either an open spot or a current stack of said item
	//Currently assumes that if an item can stack, it will stack (Does not check stack sizes).
	int CanAddItem (CWeaponItem* cheezburger);

	//Return the pointer to the currently equipped item
	CWeaponItem*	GetCurrentEquipped (short hand=0) override;
	short			GetCurrentEquippedCount ( void ) override;
	//Changes the currently equipped item
	void	ChangeEquipped (int scrolldir);
	// Returns the belt slot of what the mouse-wheel cursor is currently on.
	short	GetCurrent (void);
	// Returns the belt slot of what hand has what item. -1 if not equipped.
	short	GetCurrentEquippedIndex (short hand=0);
	// Returns size of the equipped array.
	short	GetHandCount (void);
	// Returns last hand to equip an item
	short	GetLastHandEquipped (void);
	//Checks the currently equipped item
	void	CheckEquipped (void);
	//Change the equipped item to the slot given
	void	ChangeEquippedTo (int slot, bool allowUnequip=true) override;

	//Return the array of Hotkeyed items
	CWeaponItem** GetCurrentHotbar (void);
	//Get the hotbar and return it
	CWeaponItem** GetHotbar (void);

	//Get a single item from the pInventory
	CWeaponItem* GetItem (short slot);
	//Find an item in the pInventory
	CWeaponItem* FindItem (short id);

	//Switch the spots of two items
	void SwitchSpots (short source, short dest);
	//Place an item into the pInventory
	void ReceiveItem (CWeaponItem* item, short slot);
	//Switches a spot in hotbar/belt with the inventory
	void SwitchSpotHotbar (const short inventorySpot, const short hotbarSpot);

	//Rechecked the equipped item, in the case of moving items around in the pInventory
	void RecheckEquipped (short slot);

	//Add two stacks of items to each other
	CWeaponItem* AddStacks (CWeaponItem* item, short slot);
	//Add two stacks of items to each other (for calling from other classes)
	CWeaponItem* AddStacks (CWeaponItem* item, short slot, bool hotornot);

	// Armor piece management
	CWearableItem* EquipArmorPiece (CWearableItem* piece, short slot);
	CWearableItem* UnequipArmorPiece (short slot);
	CWearableItem* GetEquippedArmorPiece (const short slot);

	//Get the armor list so the armor can be rendered in the GUI
	CWearableItem** GetArmorList (void);

	// Save the inventory to the given file
	void SaveInventory (const arstring<256> & location);
	// Load the inventory to the given file
	bool LoadInventory ( const arstring<256> & location);

	//Easy dereference because I'm getting lazy. Doesn't actually work yet
	//So much for being lazy.
	CWeaponItem* operator () ( const unsigned char x, const unsigned char y )
	{
		if (pInventory[x+y*sWidth] != NULL)
			return pInventory[x+y*sWidth];
		else
			return NULL;
	}
		
	// Gear type enumeration
	enum eGear
	{
		GEAR_HEAD		= 0,
		GEAR_NECKLACE	= 1,
		GEAR_SHOULDER	= 2,
		GEAR_CHEST		= 3,
		GEAR_RIGHTHAND	= 4,
		GEAR_LEFTHAND	= 5,
		GEAR_LEGS		= 6,
		GEAR_RIGHTFOOT	= 7,
		GEAR_LEFTFOOT	= 8,

		GEAR_MAX
	};

private:
	void ClearAndDeleteContents ( void );

private:
	CWeaponItem**	wEquipped;
	CWeaponItem**	wHotbar;
	CWeaponItem*	wEmpty;
	CWearableItem**	wArmor;
		
	bool			bSingleplayer;
	bool			bInHotbar;

	short			sBeltWidth; //Default: 6
	short			sCurrent; 
	short			sLastEquipped;
	short			sBeltHeight;
	short			sHands;
	short			sHandsUsed;
	short			sLastHandEquipped;

	// Function to check all items and make sure they're actually valid
	void CheckCurrentItems ( void ) override;
};
#endif