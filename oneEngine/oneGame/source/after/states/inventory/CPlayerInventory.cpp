#include "CPlayerInventory.h"

#include "core/system/io/CBinaryFile.h"
#include "core-ext/system/io/serializer/OSerialBinary.h"
#include "core-ext/system/io/serializer/ISerialBinary.h"
#include "core/system/io/FileUtils.h"

#include "after/types/terrain/BlockType.h"

#include "after/entities/item/beta/Flare.h"
#include "after/entities/item/system/ItemTerraBlok.h"
#include "after/entities/item/system/ItemBlockPuncher.h"
#include "after/entities/item/system/ItemMeleeCombat.h"
#include "after/entities/item/beta/WeaponCrossbow.h"
#include "after/entities/item/props/ItemTorch.h"
#include "after/entities/item/props/ItemLantern.h"
#include "after/entities/item/material/ItemTreeTrunk.h"
#include "after/entities/item/material/ItemTreeResin.h"
#include "after/entities/item/ammo/AmmoArrow.h"
#include "after/entities/item/ammo/ItemPebble.h"

#include "after/entities/item/skill/skills/mobility/SkillBlink.h"
#include "after/entities/item/skill/skills/offensive/SkillMagikinesis.h"

#include "after/entities/item/wearable/armor/ArmorLeatherBoots.h"
#include "after/entities/item/wearable/armor/ArmorLeatherPants.h"
#include "after/entities/item/wearable/vanity/WearElvenpunk02_goggles.h"
#include "after/entities/item/wearable/vanity/WearElvenpunk02_skirt.h"
#include "after/entities/item/wearable/vanity/WearElvenpunk02_set.h"

#include "after/entities/item/weapon/CWeaponItemGenerator.h"
#include "engine-common/types/ItemProperties.h"

using std::cout;
using std::endl;

//Constructor
//CPlayerInventory::CPlayerInventory ( CActor* owner ) : CInventory( owner ),
CPlayerInventory::CPlayerInventory ( CActor* owner, short w, short h, short hands, bool adddebugitems ) : CInventory( owner, w, h ),
	bSingleplayer(true), /*sWidth(4), sHeight(6),*/ sBeltWidth(6), sHands (hands), sBeltHeight(1)
{
	//pInventory = new CWeaponItem* [sWidth * sHeight];
	wEquipped = new CWeaponItem* [sHands];
	wArmor = new CWearableItem* [9];
	wHotbar = new CWeaponItem* [sBeltHeight * sBeltWidth];	
	// Initialize the pInventory to empty
	for (short i = 0; i < sHands; i++) {
		wEquipped[i] = NULL;
	}
	wEmpty = NULL;
	for ( int i = 0; i < sWidth*sHeight; i += 1 ) {
		pInventory[i] = NULL;
	}
	for (short i = 0; i < GEAR_MAX; i++) {
		wArmor[i] = NULL;
	}
	for (short i = 0; i < sBeltHeight * sBeltWidth; i++)
	{
		wHotbar[i] = NULL;
	}

	// Create blok pauncher
	wEmpty = new ItemMeleeCombat();
	wEmpty->SetOwner(Owner);

	// Init inventory info
	bInHotbar = false;
	sHandsUsed = 0;

	sCurrent = 0;
	sLastEquipped = 0;

	if ( adddebugitems ) 
	{
		// Populate pInventory
		pInventory[0] = new WeaponCrossbow();
		pInventory[0]->SetOwner( Owner );
		pInventory[0]->SetHoldState( Item::HoldState::Hidden );

		pInventory[1] = new ItemTorch();
		pInventory[1]->SetOwner( Owner );
		pInventory[1]->SetHoldState( Item::HoldState::Hidden );

		pInventory[2] = new ItemTerraBlok( NULL, Terrain::EB_WOOD );
		pInventory[2]->SetOwner( Owner );
		pInventory[2]->SetHoldState( Item::HoldState::Hidden );

		pInventory[3] = ItemGenerator.MakeShittySword( Vector3d() );
		pInventory[3]->SetOwner( Owner );
		pInventory[3]->SetHoldState( Item::HoldState::Hidden );

		pInventory[4] = ItemGenerator.MakePickaxeSuShitty( Vector3d() );
		pInventory[4]->SetOwner( Owner );
		pInventory[4]->SetHoldState( Item::HoldState::Hidden );

		pInventory[5] = new ItemTreeTrunk();
		pInventory[5]->SetOwner(Owner);
		pInventory[5]->SetHoldState(Item::HoldState::Hidden);
		pInventory[5]->SetStackSize(30);

		pInventory[6] = new ItemTreeResin();
		pInventory[6]->SetOwner(Owner);
		pInventory[6]->SetHoldState(Item::HoldState::Hidden);
		pInventory[6]->SetStackSize(30);

		pInventory[7] = new AmmoArrow();
		pInventory[7]->SetOwner(Owner);
		pInventory[7]->SetHoldState(Item::HoldState::Hidden);
		pInventory[7]->SetStackSize(30);

		pInventory[8] = ItemGenerator.MakeTestBow( Vector3d() );
		pInventory[8]->SetOwner( Owner );
		pInventory[8]->SetHoldState( Item::HoldState::Hidden );

		pInventory[9] = new ItemLantern();
		pInventory[9]->SetOwner( Owner );
		pInventory[9]->SetHoldState( Item::HoldState::Hidden );

		pInventory[10] = new ArmorLeatherBoots();
		pInventory[10]->SetOwner( Owner );
		pInventory[10]->SetHoldState( Item::HoldState::Hidden );

		pInventory[11] = new ArmorLeatherPants();
		pInventory[11]->SetOwner( Owner );
		pInventory[11]->SetHoldState( Item::HoldState::Hidden );

		pInventory[13] = new ItemPebble();
		pInventory[13]->SetOwner(Owner);
		pInventory[13]->SetHoldState(Item::HoldState::Hidden);
		pInventory[13]->SetStackSize(30);


		pInventory[16] = new WearElvenpunk02_goggles();
		pInventory[16]->SetOwner(Owner);
		pInventory[16]->SetHoldState(Item::HoldState::Hidden);
		pInventory[17] = new WearElvenpunk02_skirt();
		pInventory[17]->SetOwner(Owner);
		pInventory[17]->SetHoldState(Item::HoldState::Hidden);
		pInventory[18] = new WearElvenpunk02_shirt();
		pInventory[18]->SetOwner(Owner);
		pInventory[18]->SetHoldState(Item::HoldState::Hidden);
		pInventory[19] = new WearElvenpunk02_boots();
		pInventory[19]->SetOwner(Owner);
		pInventory[19]->SetHoldState(Item::HoldState::Hidden);
		pInventory[15] = new WearElvenpunk02_cloak();
		pInventory[15]->SetOwner(Owner);
		pInventory[15]->SetHoldState(Item::HoldState::Hidden);
	}
	else 
	{
		wHotbar[0] = ItemGenerator.MakePickaxeSuShitty( Vector3d() );
		wHotbar[0]->SetOwner( Owner );
		wHotbar[0]->SetHoldState( Item::HoldState::OnBelt );

		wHotbar[1] = ItemGenerator.MakeShittySword( Vector3d() );
		wHotbar[1]->SetOwner( Owner );
		wHotbar[1]->SetHoldState( Item::HoldState::OnBelt );

		wHotbar[5] = ItemGenerator.DropJunk( Vector3d() );
		wHotbar[5]->SetOwner( Owner );
		wHotbar[5]->SetHoldState( Item::HoldState::OnBelt );

		wHotbar[2] = ItemGenerator.MakeHoe( Vector3d() );
		wHotbar[2]->SetOwner( Owner );
		wHotbar[2]->SetHoldState( Item::HoldState::OnBelt );
	}
}

//Destructor
CPlayerInventory::~CPlayerInventory (void)
{
	ClearAndDeleteContents();

	if (wEquipped)
		delete [] wEquipped;
	wEquipped = NULL;
	if (wEmpty)
		delete wEmpty;
	wEmpty = NULL;
	if (wArmor)
		delete [] wArmor;
	wArmor = NULL;
	if (wHotbar)
		delete []wHotbar;
	wHotbar = NULL;

	sWidth = 0;
	sHeight = 0;
	bSingleplayer = 0;
}

// Update function
void CPlayerInventory::Update ( void )
{
	CheckCurrentItems();
	CheckEquipped();
}

short CPlayerInventory::GetHeight (void)
{
	return sHeight;
}

short CPlayerInventory::GetWidth (void)
{
	return sWidth;
}

//This function adds the item to the pInventory
bool CPlayerInventory::AddItem (CWeaponItem* treasure)
{
	//Find a spot for the new item
	int location = CanAddItem(treasure);

	//If there's an open spot
	if ( location != -1 )
	{
		//Take ownership of the item
		treasure->SetOwner( Owner );
		treasure->SetHoldState( Item::HoldState::Hidden );
	
		if (bInHotbar == true)
		{
			if (wHotbar[location] == NULL)
			{
				wHotbar[location] = treasure;
				RecheckEquipped(location);
			}
			else
			{
				CWeaponItem* temp = AddStacks(treasure, location);
				if (temp != NULL) {
					AddItem(temp);
				}
			}
		}
		else
		{
			//If it's going into an wEmpty slot
			if (pInventory[location] == NULL)
			{
				//Put the item in the pInventory
				pInventory[location] = treasure;
			}
			//If it's going onto a stack, put it there
			else
			{
				CWeaponItem* temp = AddStacks(treasure, location);
				if (temp != NULL) {
					AddItem(temp);
				}
			}
		}
		return true;
	}
	else {
		return false;
	}
}

//Check to see if an item can be placed into the pInventory
int CPlayerInventory::CanAddItem (CWeaponItem* cheezburger)
{
	int location = 0, open = -1;
	
	// First check the pointer, make sure it's not already in the pInventory
	for (location = 0; location < (sWidth * sHeight); location++)
	{
		// If the item is already held, return -1 (don't pick it up twice!)
		if ( pInventory[location] == cheezburger )
		{
			return -1;
		}
	}
	// Check for if it's a terra blok.
	if ( cheezburger->GetTypeName() == "ItemTerraBlok" )
	{
		//If it is, do some magic to access it's TerraBlok data
		ItemTerraBlok* newBlokItem = (ItemTerraBlok*)(cheezburger);

		//Look through the wHotbar first
		for (location = 0; location < 6; location++)
		{
			if (wHotbar[location] != NULL)
			{
				if (wHotbar[location]->GetTypeName() == "ItemTerraBlok")
				{
					//Get it's info
					ItemTerraBlok* tempBlokItem = (ItemTerraBlok*)(wHotbar[location]);

					//Then check to see if it's the same type of blok
					if (tempBlokItem->GetType() == newBlokItem->GetType())				
					{
						//Then check to make sure that the stack isn't full before returning the location
						if (tempBlokItem->GetStackSize() < tempBlokItem->GetMaxStack())
						{
							bInHotbar = true;
							return location;
						}
					}
				}
			}
		}

		//Look through the pInventory
		for (location = 0; location < (sWidth * sHeight); location++)
		{
			//If there's an item
			if (pInventory[location] != NULL)
			{
				//And that item is a terra blok
				if (pInventory[location]->GetTypeName() == "ItemTerraBlok")
				{
					//Get it's info
					ItemTerraBlok* tempBlokItem = (ItemTerraBlok*)(pInventory[location]);

					//Then check to see if it's the same type of blok
					if (tempBlokItem->GetType() == newBlokItem->GetType())				
					{
						//Then check to make sure that the stack isn't full before returning the location
						if (tempBlokItem->GetStackSize() < tempBlokItem->GetMaxStack())
						{
							bInHotbar = false;
							return location;
						}
					}
				}
			}
			//If it's the first wEmpty slot found
			else if (open == -1)
			{
				//Mark it down
				open = location;
				bInHotbar = false;
			}
		}
	}
	//If it isn't a terra blok
	else
	{
		//Search through the wHotbar
		for (location = 0; location < sBeltWidth; location++)
		{
			//If there's an item
			if (wHotbar[location] != NULL)
			{

				//Check to see if it is the same item and can stack: Now uses just the IDs, not the full WItemData struct
				if ( cheezburger->GetID() == wHotbar[location]->GetID() && cheezburger->GetCanStack() )				
				{
					//Then check to make sure that the stack isn't already full before returning location
					if (wHotbar[location]->GetStackSize() < wHotbar[location]->GetMaxStack())
					{
						bInHotbar = true;
						return location;
					}
				}
			}
		}

		//Search through the pInventory
		for (location = 0; location < (sWidth * sHeight); location++)
		{
			//If there's an item
			if (pInventory[location] != NULL)
			{
				//Check to see if it is the same item and can stack: Now uses just the IDs, not the full WItemData struct
				if ( cheezburger->GetID() == pInventory[location]->GetID() && cheezburger->GetCanStack() )				
				{
					//Then check to make sure that the stack isn't already full before returning location
					if (pInventory[location]->GetStackSize() < pInventory[location]->GetMaxStack())
					{
						bInHotbar = false;
						return location;
					}
				}
			}
			//If it's the first open spot found
			else if (open == -1)
			{
				//Mark it down
				bInHotbar = false;
				open = location;
			}
		}
	}
	//Only get here if there were no open stacks already available
	//Return the first open spot found or -1 if no open spots or unfilled stacks were found.
	return open;
}

//Return the currently wEquipped item
CWeaponItem* CPlayerInventory::GetCurrentEquipped (short slot)
{
	return wEquipped[slot];
}
short CPlayerInventory::GetCurrentEquippedCount ( void )
{
	return sHands;
}

//Change the currently wEquipped item
void CPlayerInventory::ChangeEquipped (int scrolldir)
{
	// Change equipped with mousewheel, but do not let it mess up the equipping states.
	//If the wheeled was scrolled downward, increment current to equip the item in the next slot
	if ( scrolldir < 0 )
	{
		if ( sCurrent + 1 == sBeltWidth ) {
			ChangeEquippedTo( 0,false );
		}
		else {
			ChangeEquippedTo( sCurrent + 1,false );
		}
	}
	//If the wheel was scrolled upward, decrement current to equip the item in the previous slot
	else if ( scrolldir > 0 )
	{
		if ( sCurrent == 0 ) {
			ChangeEquippedTo( sBeltWidth-1,false );
		}
		else {
			ChangeEquippedTo( sCurrent - 1,false );
		}
	}
}

short CPlayerInventory::GetCurrent (void)
{
	//return sLastEquipped;
	return sCurrent;
}

short CPlayerInventory::GetCurrentEquippedIndex (short hand)
{
	if ( wEquipped[hand] != NULL && wEquipped[hand] != wEmpty ) {
		for (short i = 0; i < sBeltWidth*sBeltHeight; ++i)
		{
			if ( wHotbar[i] == wEquipped[hand] ) {
				return i;
			}
		}
	}
	return -1;
}


short CPlayerInventory::GetHandCount (void) {
	return sHands;
}

short CPlayerInventory::GetLastHandEquipped (void) {
	return sLastHandEquipped;
}

//Checks the stack size of the wEquipped item
void CPlayerInventory::CheckEquipped (void)
{
	for ( int i = 0; i < sHands; ++i ) {
		if ( wEquipped[i] == NULL ) {
			wEquipped[i] = wEmpty;
		}

		//If the stack has been exhausted
		if (wEquipped[i]->GetStackSize() <= 0)
		{
			//Delete the item, set the slot to NULL to mark it as wEmpty, and equip the BlokPuncher in wEmpty
			Trash(wHotbar[sCurrent]);
			wHotbar[sCurrent] = NULL;
			wEquipped[i] = wEmpty;
		}
	}
}

//Change the wEquipped item based on a number passed in
void CPlayerInventory::ChangeEquippedTo (int slot,bool allowUnequip)
{
	// Set current to this slot
	sCurrent = slot;

	// Get target hand
	short targetHand = slot/3;
	sLastHandEquipped = targetHand;
	// Check if can equip
	bool canEquip = true;
	if (wHotbar[slot] != NULL)
	{
		if (wHotbar[slot]->GetHands() > sHands) {
			canEquip = false;
		}
	}
	// Check if item is already equipped somewhere
	bool already = false;
	bool broken = false;
	if (wHotbar[slot] != NULL)
	{
		//Check to make sure the item isn't equipped already
		for (short i = 0; i < sHands; i++)
		{
			if (wEquipped[i] == wHotbar[slot])
			{
				already = true;
				if ( i != targetHand ) {
					broken = true;
				}
				break;
			}
		}
	}
	// Fix broken
	if ( broken ) {
		cout << "Yeah, no idea." << endl;
		cout << "If Josh has no idea, then we're all doomed" << endl;
	}
	// If slot trying to equip is empty, empty out the hand
	if ( wHotbar[slot] == NULL )
	{
		if ( wEquipped[targetHand] != NULL && wEquipped[targetHand] != wEmpty ) {
			Owner->OnUnequip(wEquipped[targetHand]);
			wEquipped[targetHand]->SetHoldState(Item::HoldState::Hidden);
		}
		wEquipped[targetHand] = wEmpty;
	}
	// If equipped, and allowing unequip, then unequip
	else if ( already )
	{
		if ( allowUnequip )
		{
			Owner->OnUnequip(wEquipped[targetHand]);
			wEquipped[targetHand]->SetHoldState(Item::HoldState::Hidden);
			wEquipped[targetHand] = wEmpty;
		}
	}
	// Equip if not equipped already
	else if ( canEquip )
	{
		// Loop through equipped to check amount of hands used
		short handsUsed = 0;
		for (short i = 0; i < sHands; ++i)
		{
			if (wEquipped[i] != NULL && wEquipped[i] != wEmpty) {
				handsUsed += wEquipped[i]->GetHands();
			}
		}
		// Unequip previous item
		if ( wEquipped[targetHand] != NULL && wEquipped[targetHand] != wEmpty ) {
			wEquipped[targetHand]->SetHoldState(Item::HoldState::Hidden);
			Owner->OnUnequip(wEquipped[targetHand]);
		}
		// Get the next amount of hands used if this one is equipped
		wEquipped[targetHand] = wHotbar[slot];
		short nextHandsUsed = 0;
		for (short i = 0; i < sHands; ++i)
		{
			if (wEquipped[i] != NULL && wEquipped[i] != wEmpty) {
				//cout << wEquipped[i]->GetHands() << ";";
				nextHandsUsed += wEquipped[i]->GetHands();
			}
		}

		// If this item pushes over the limit
		if ( nextHandsUsed > sHands )
		{
			// Then start unequipping items.
			for (short i = 0; i < sHands; ++i)
			{
				// Skip target hand
				if (i != targetHand ) {
					if (wEquipped[i] != NULL && wEquipped[i] != wEmpty) {
						if ( wEquipped[i]->GetHands() > 0 ) {
							// Subtract used hands count
							nextHandsUsed -= wEquipped[i]->GetHands();
							// Unequip item
							Owner->OnUnequip( wEquipped[i] );
							wEquipped[i]->SetHoldState(Item::HoldState::Hidden);
							wEquipped[i] = wEmpty; 
							if ( nextHandsUsed <= sHands ) {
								break;
							}
						} // End check no-handed object
					} // End check invalid object
				} // End skip target hand
			}
			// Done unequipping items
		}
		// Actually equip item
		Owner->OnEquip(wEquipped[targetHand]);
		wEquipped[targetHand]->SetHoldState(Item::HoldState::Holding);
		wEquipped[targetHand]->SetHand( targetHand );
	}
}


//Return the wHotbar items
CWeaponItem** CPlayerInventory::GetCurrentHotbar (void)
{
	return wHotbar;
}

//Brilliant. Two functions with different names that do the exact same thing.
CWeaponItem** CPlayerInventory::GetHotbar (void)
{
	return wHotbar;
}

CWeaponItem* CPlayerInventory::GetItem (short slot)
{
	if (slot >= 0 && slot < sWidth * sHeight)
		return pInventory[slot];
	else
		return NULL;
}

//Search for an item in the pInventory. Searches through Hotbar first then through inventory.
CWeaponItem* CPlayerInventory::FindItem (short id)
{
	for (int i = 0; i < sBeltWidth; i++)
		if (wHotbar[i] != NULL)
			if ((wHotbar[i])->GetID() == id)
				return wHotbar[i];

	for (int i = 0; i < sWidth; i++)
		for (int j = 0; j < sHeight; j++)
			if (pInventory[i + j * sWidth] != NULL)
				if ((pInventory[i + j * sWidth])->GetID() == id)
					return pInventory[i + j * sWidth];

	return NULL;
}

//Switch the spots of two items
void CPlayerInventory::SwitchSpots (short source, short dest)
{	
	CWeaponItem* temp;

	temp = pInventory[source];
	pInventory[source] = pInventory[dest];
	pInventory[dest] = temp;
}

void CPlayerInventory::ReceiveItem (CWeaponItem* item, short slot)
{
	if (slot > sHeight * sWidth || slot < 0)
	{
		cout << "OUT OF BOUNDS IDIOT!" << slot << endl;
	}
	else
	{
		pInventory[slot] = item;
	}
}

void CPlayerInventory::SwitchSpotHotbar (const short inventorySpot, const short hotbarSpot)
{
	CWeaponItem* temp;
	temp = pInventory[inventorySpot];
	pInventory[inventorySpot] = wHotbar[hotbarSpot];
	wHotbar[hotbarSpot] = temp;

	RecheckEquipped( hotbarSpot );
}

void CPlayerInventory::RecheckEquipped (short slot)
{
	if (slot == sCurrent) {
		ChangeEquippedTo (sCurrent);
	}
} 

CWeaponItem* CPlayerInventory::AddStacks (CWeaponItem* item, short slot)
{
	if (bInHotbar == true)
	{
		if (item->GetID() == wHotbar[slot]->GetID())
		{
			short total = item->GetStackSize() + wHotbar[slot]->GetStackSize();
			short max = item->GetMaxStack();
		
			if (total > max)
			{
				wHotbar[slot]->SetStackSize(max);
				item->SetStackSize(total - max);
				return item;
			}
			else
			{
				wHotbar[slot]->SetStackSize(total);
				delete item;
				return NULL;
			}
		}
	}
	else
	{
		if (item->GetID() == pInventory[slot]->GetID())
		{
			short total = item->GetStackSize() + pInventory[slot]->GetStackSize();
			short max = item->GetMaxStack();
		
			if (total > max)
			{
				pInventory[slot]->SetStackSize(max);
				item->SetStackSize(total - max);
				return item;
			}
			else
			{
				pInventory[slot]->SetStackSize(total);
				delete item;
				return NULL;
			}
		}
	}

	return item;
}

//Add stacks of items to each other. For calling from other classes
CWeaponItem* CPlayerInventory::AddStacks (CWeaponItem* item, short slot, bool hotornot)
{
	if (hotornot == true)
	{
		if (item->GetID() == (wHotbar[slot])->GetID())
		{
			short total = item->GetStackSize() + (wHotbar[slot])->GetStackSize();
			short max = item->GetMaxStack();
		
			if (total > max)
			{
				(wHotbar[slot])->SetStackSize(max);
				item->SetStackSize(total - max);
				return item;
			}
			else
			{
				(wHotbar[slot])->SetStackSize(total);
				delete item;
				return NULL;
			}
		}
	}
	else
	{
		if (item->GetID() == pInventory[slot]->GetID())
		{
			short total = item->GetStackSize() + pInventory[slot]->GetStackSize();
			short max = item->GetMaxStack();
		
			if (total > max)
			{
				pInventory[slot]->SetStackSize(max);
				item->SetStackSize(total - max);
				return item;
			}
			else
			{
				pInventory[slot]->SetStackSize(total);
				delete item;
				return NULL;
			}
		}
	}

	return item;
}

CWearableItem* CPlayerInventory::EquipArmorPiece (CWearableItem* piece, short slot)
{
	CWearableItem* temp = NULL;
	switch (slot)
	{
	case GEAR_HEAD:
		if (piece->GetItemType() == WeaponItem::TypeArmorHead)
		{
			temp = wArmor[GEAR_HEAD];
			wArmor[GEAR_HEAD] = piece;
			if ( temp ) {
				temp->OnUnequip( Owner );
			}
			if ( piece ) {
				piece->OnEquip( Owner );
			}
			piece = NULL;
		}
		else
			temp = piece;
		break;
	case GEAR_NECKLACE:
		if (piece->GetItemType() == WeaponItem::TypeNecklace)
		{
			temp = wArmor[GEAR_NECKLACE];
			wArmor[GEAR_NECKLACE] = piece;
			if ( temp ) {
				temp->OnUnequip( Owner );
			}
			if ( piece ) {
				piece->OnEquip( Owner );
			}
			piece = NULL;
		}
		else
			temp = piece;
		break;	
	case GEAR_SHOULDER:
		if (piece->GetItemType() == WeaponItem::TypeArmorShoulder)
		{
			temp = wArmor[GEAR_SHOULDER];
			wArmor[GEAR_SHOULDER] = piece;
			if ( temp ) {
				temp->OnUnequip( Owner );
			}
			if ( piece ) {
				piece->OnEquip( Owner );
			}
			piece = NULL;
		}
		else
			temp = piece;
		break;	
	case GEAR_CHEST:
		if (piece->GetItemType() == WeaponItem::TypeArmorChest)
		{
			temp = wArmor[GEAR_CHEST];
			wArmor[GEAR_CHEST] = piece;
			if ( temp ) {
				temp->OnUnequip( Owner );
			}
			if ( piece ) {
				piece->OnEquip( Owner );
			}
			piece = NULL;
		}
		else
			temp = piece;
		break;	
	case GEAR_RIGHTHAND:
		if (piece->GetItemType() == WeaponItem::TypeArmorGloves)
		{
			temp = wArmor[GEAR_RIGHTHAND];
			wArmor[GEAR_RIGHTHAND] = piece;
			if ( temp ) {
				temp->OnUnequip( Owner );
			}
			if ( piece ) {
				piece->OnEquip( Owner );
			}
			piece = NULL;
		}
		else
			temp = piece;
		break;	
	case GEAR_LEFTHAND:
		if (piece->GetItemType() == WeaponItem::TypeArmorGloves)
		{
			temp = wArmor[GEAR_LEFTHAND];
			wArmor[GEAR_LEFTHAND] = piece;
			if ( temp ) {
				temp->OnUnequip( Owner );
			}
			if ( piece ) {
				piece->OnEquip( Owner );
			}
			piece = NULL;
		}
		else
			temp = piece;
		break;	
	case GEAR_LEGS:
		if (piece->GetItemType() == WeaponItem::TypeArmorGreaves)
		{
			temp = wArmor[GEAR_LEGS];
			wArmor[GEAR_LEGS] = piece;
			if ( temp ) {
				temp->OnUnequip( Owner );
			}
			if ( piece ) {
				piece->OnEquip( Owner );
			}
			piece = NULL;
		}
		else
			temp = piece;
		break;	
	case GEAR_RIGHTFOOT:
		if (piece->GetItemType() == WeaponItem::TypeArmorBoots)
		{
			temp = wArmor[GEAR_RIGHTFOOT];
			wArmor[GEAR_RIGHTFOOT] = piece;
			if ( temp ) {
				temp->OnUnequip( Owner );
			}
			if ( piece ) {
				piece->OnEquip( Owner );
			}
			piece = NULL;
		}
		else
			temp = piece;
		break;	
	case GEAR_LEFTFOOT:
		if (piece->GetItemType() == WeaponItem::TypeArmorBoots)
		{
			temp = wArmor[GEAR_LEFTFOOT];
			wArmor[GEAR_LEFTFOOT] = piece;
			if ( temp ) {
				temp->OnUnequip( Owner );
			}
			if ( piece ) {
				piece->OnEquip( Owner );
			}
			piece = NULL;
		}
		else
			temp = piece;
		break;
	default:
		cout << "Invalid wArmor slot. What?" << endl;
		break;
	}
	return temp;
}

CWearableItem* CPlayerInventory::UnequipArmorPiece (short slot)
{
	CWearableItem* temp = NULL;

	if (( slot < GEAR_MAX )&&( slot >= 0 ))
	{
		temp = wArmor[slot];
		wArmor[slot] = NULL;
		if ( temp ) {
			temp->OnUnequip( Owner );
		}
		return temp;
	}
	else
	{
		cout << "Nothing is there? I don't know." << endl;
		return NULL;
	}
}

CWearableItem* CPlayerInventory::GetEquippedArmorPiece ( const short slot )
{
	if (( slot < GEAR_MAX )&&( slot >= 0 )) {
		return wArmor[slot];
	}
	else {
		cout << "Nothing is there? I don't know." << endl;
		return NULL;
	}
}

CWearableItem** CPlayerInventory::GetArmorList (void)
{
	return wArmor;
}

void CPlayerInventory::SaveInventory (const arstring<256> & location)
{
	CBinaryFile fDest;
	arstring<256> cheese ("inventory");

	fDest.Open ( location, CBinaryFile::IO_WRITE);
	OSerialBinary bWriter (&fDest);
	//Write as uint32_t, CBinaryFile::WriteUint32
	//No item, 0
	//Item, 0 then << serialize
	cout << location << endl;
	if (!fDest.IsOpen())
	{
		cout << "File opening failed: " << location << " may not exist or something like that" << endl;
	}
	else
	{
		for (short i = 0; i < sHeight * sWidth; i++)
		{
			if (pInventory[i] == NULL)
			{
				fDest.WriteInt32(0);
			}
			else
			{
				fDest.WriteInt32(pInventory[i]->GetID());
				bWriter << (*pInventory[i]);
			}
		}

		for (short i = 0; i < sBeltWidth; i++)
		{
			if (wHotbar[i] == NULL)
			{
				fDest.WriteInt32(0);
			}
			else
			{
				fDest.WriteInt32(wHotbar[i]->GetID());
				bWriter << (*wHotbar[i]);
			}
		}

		for (short i = 0; i < GEAR_MAX; i++)
		{
			if (wArmor[i] == NULL)
			{
				fDest.WriteInt32(0);
			}
			else
			{
				fDest.WriteInt32(wArmor[i]->GetID());
				bWriter << (*wArmor[i]);
			}

		}
	}
}

bool CPlayerInventory::LoadInventory (const arstring<256> & location)
{
	//ISerialBinary instead of OSerialBinary
	CBinaryFile fSource;
	fSource.Open ( location, CBinaryFile::IO_READ);
	ISerialBinary bWriter (&fSource);
	//Write as uint32_t, CBinaryFile::WriteUint32
	//No item, 0
	//Item, 0 then << serialize
	
	if (!IO::FileExists (location))
		return false;

	if (!fSource.IsOpen())
	{
		cout << "File opening failed: " << location << " may not exist or something like that";
		return false;
	}
	else
	{
		ClearAndDeleteContents();

		for (short i = 0; i < sHeight * sWidth; i++)
		{
			int32_t id = fSource.ReadInt32();
			cout <<i << ": " << id << endl;
			if ( id == -1 || id == -2 )
			{
				cout << "Error: Item ID is negative" << endl;
			}
			else if ( WeaponItem::WeaponItemFactory.witem_list.count( id ) == 0 )
			{
				pInventory[i] = NULL;
			}
			else
			{
				pInventory[i] = CWeaponItem::Instantiate(id);
				if ( pInventory[i] ) {
					bWriter >> (*pInventory[i]);
					pInventory[i]->SetOwner( Owner );
					pInventory[i]->SetHoldState( Item::HoldState::Hidden );
				}
			}
		}
		for (short i = 0; i < sBeltWidth; i++)
		{
			int32_t id = fSource.ReadInt32();
			cout <<i << ": " << id << endl;
			if ( id == -1 || id == -2 )
			{
				cout << "Error: Item ID is invalid" << endl;
			}
			else if ( WeaponItem::WeaponItemFactory.witem_list.count( id ) == 0 )
			{
				wHotbar[i] = NULL;
			}
			else
			{
				wHotbar[i] = CWeaponItem::Instantiate(id);
				if ( wHotbar[i] ) {
					bWriter >> (*wHotbar[i]);
					wHotbar[i]->SetOwner( Owner );
					wHotbar[i]->SetHoldState( Item::HoldState::Hidden );
				}
			}
		}
		for (short i = 0; i < GEAR_MAX; i++)
		{
			int32_t id = fSource.ReadInt32();
			cout <<i << ": " << id << endl;
			if ( id == -1 || id == -2 )
			{
				cout << "Error: Item ID is invalid" << endl;
			}
			else if ( WeaponItem::WeaponItemFactory.witem_list.count( id ) == 0 )
			{
				wArmor[i] = NULL;
			}
			else
			{
				wArmor[i] = (CWearableItem*)(CWeaponItem::Instantiate(id));
				if ( wArmor[i] ) {
					bWriter >> (*wArmor[i]);
					wArmor[i]->SetOwner( Owner );
					wArmor[i]->SetHoldState( Item::HoldState::Hidden );
					Owner->OnEquip( wArmor[i] );
				}
			}
		}
	}

	return true;
}

void CPlayerInventory::ClearAndDeleteContents ( void )
{
	for (short i = 0; i < sHeight * sWidth; i++)
	{
		for (short k = 0; k < GEAR_MAX; k++)
		{
			if ( pInventory[i] == wArmor[k] ) {
				wArmor[k] = NULL;
			}
		}
		if ( pInventory[i] ) {
			pInventory[i]->SetOwner(NULL);
			delete pInventory[i];
			pInventory[i] = NULL;
		}
	}
	for (short i = 0; i < sBeltWidth; i++)
	{
		if ( wHotbar[i] ) {
			wHotbar[i]->SetOwner(NULL);
			delete wHotbar[i];
			wHotbar[i] = NULL;
		}
	}
	for (short i = 0; i < GEAR_MAX; i++)
	{
		if ( wArmor[i] ) {
			wArmor[i]->SetOwner(NULL);
			delete wArmor[i];
			wArmor[i] = NULL;
		}
	}
}

// Checks the pInventory for any items that aren't actually owned by our owning actor.
// If the item is not owned, the reference to it is removed and the pointer in the
//  pInventory is set back to null
void CPlayerInventory::CheckCurrentItems ( void )
{
	// Loop through all the items
	for ( int i = 0; i < sWidth*sHeight; i++ )
	{
		if ( pInventory[i] != NULL )
		{
			//If the player does not own the item or the stack is wEmpty
			if ( pInventory[i]->GetOwner() == NULL ) {
				// Remove reference
				pInventory[i] = NULL;
			}
			else if ( pInventory[i]->GetStackSize() <= 0 ) {
				Trash( pInventory[i] );
				pInventory[i] = NULL;
			}
		}
	}
	// Loop through all the wHotbar items
	for ( int i = 0; i < sBeltWidth; i++ )
	{
		//cout << i << ": " << wHotbar[i] << endl;
		if ( (wHotbar[i]) != NULL )
		{
			bool removeRef = false;
			if ( wHotbar[i]->GetOwner() == NULL) {
				// Remove reference
				removeRef = true;
			}
			else if ( wHotbar[i]->GetStackSize() <= 0 ) {
				Trash( wHotbar[i] );
				removeRef = true;
			}
			// Unequip item
			if ( removeRef ) {
				//if ( i == sCurrent ) {
				if ( wEquipped[i/3] == wHotbar[i] ) {
					wEquipped[i/3] = wEmpty;
				}
				wHotbar[i] = NULL;
			}
		}
	}

	// Also check the wEquipped item
	for ( int i = 0; i < sHands; i++ )
	{
		if ( wEquipped[i] && wEquipped[i]->GetOwner() == NULL ) {
			wEquipped[i] = NULL;
		}
	}
}
