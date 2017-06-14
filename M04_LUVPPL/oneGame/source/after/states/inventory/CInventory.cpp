#include "CInventory.h"

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

using namespace std;

//Constructor
CInventory::CInventory ( CActor* owner ) : 
	 sWidth(4), sHeight(6), Owner( owner )
{
	pInventory = new CWeaponItem* [sWidth * sHeight];

	for ( int i = 0; i < sWidth*sHeight; i += 1 )
		pInventory[i] = NULL;
}

CInventory::CInventory (CActor* owner, short w, short h) : 
	sWidth (w), sHeight (h), Owner (owner)
{
	pInventory = new CWeaponItem* [sWidth * sHeight];
	for (int i = 0; i < sWidth*sHeight; i++)
		pInventory[i] = NULL;
}

//Destructor
CInventory::~CInventory (void)
{
	
	if (pInventory)
		delete [] pInventory;
	pInventory = NULL;

	sWidth = 0;
	sHeight = 0;
}

// Update function
void CInventory::Update ( void )
{
	CheckCurrentItems();
}

CActor* CInventory::GetOwner (void)
{
	return Owner;
}

//Return the sHeight of the pInventory
short CInventory::GetHeight (void)
{
	return sHeight;
}

//Return the sWidth of the pInventory
short CInventory::GetWidth (void)
{
	return sWidth;
}

//Delete an item
void CInventory::Trash (CWeaponItem* junk)
{	
	//Set it's owner and hold state to NULL and none to avoid weird stuff
	junk->SetHoldState(Item::HoldState::None);
	junk->SetOwner(NULL);

	//Clear up the space allocated for the item and then set the pointer to null
	if (junk)
		delete junk;
	junk = NULL;
}

//Drop an item to the ground
void CInventory::Drop (CWeaponItem* junk)
{
	//Set the owner to the world and hold state to none
	junk->transform.position = Owner->GetHoldTransform().position;
	junk->SetOwner(NULL);
	junk->SetHoldState(Item::HoldState::None);
}

//This function adds the item to the pInventory
//Returns true if the item is added to the inventory
//Returns false if there's no space to add an item
//Overflowing stacks added to a full inventory will result in lost items, I believe
bool CInventory::AddItem (CWeaponItem* treasure)
{
	//Find a spot for the new item
	int location = CanAddItem(treasure);

	//If there's an open spot
	if ( location != -1 )
	{
		//Take ownership of the item
		treasure->SetOwner( Owner );
		treasure->SetHoldState( Item::HoldState::Hidden );
		
		//If it's going into an empty slot
		if (pInventory[location] == NULL)
		{
			//Put the item in the pInventory
			pInventory[location] = treasure;
		}
		//If it's going onto a stack, put it there
		else
		{
			CWeaponItem* temp = AddStacks(treasure, location);

			if (temp != NULL)
				AddItem(temp);
		}
		return true;
	}
	else
		return false;
}

//Check to see if an item can be placed into the pInventory
int CInventory::CanAddItem (CWeaponItem* cheezburger)
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
							//bInHotbar = false;
							return location;
						}
					}
				}
			}
			//If it's the first empty slot found
			else if (open == -1)
			{
				//Mark it down
				open = location;
			}
		}
	}
	//If it isn't a terra blok
	else
	{
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
						//bInHotbar = false;
						return location;
					}
				}
			}
			//If it's the first open spot found
			else if (open == -1)
			{
				//Mark it down
				//bInHotbar = false;
				open = location;
			}
		}
	}
	//Only get here if there were no open stacks already available
	//Return the first open spot found or -1 if no open spots or unfilled stacks were found.
	return open;
}

//Return the pointer to all of the items
CWeaponItem** CInventory::GetItems (void)
{
	return pInventory;
}

CWeaponItem* CInventory::GetItem (short slot)
{
	if (slot >= 0 && slot < sWidth * sHeight)
		return pInventory[slot];
	else
		return NULL;
}

//Search for an item in the pInventory
CWeaponItem* CInventory::FindItem (short id)
{
	for (int i = 0; i < sWidth; i++)
		for (int j = 0; j < sHeight; j++)
			if (pInventory[i + j * sWidth] != NULL)
				if ((pInventory[i + j * sWidth])->GetID() == id)
					return pInventory[i + j * sWidth];

	return NULL;
}

//Switch the spots of two items
void CInventory::SwitchSpots (short source, short dest)
{	
	CWeaponItem* temp;

	temp = pInventory[source];
	pInventory[source] = pInventory[dest];
	pInventory[dest] = temp;
}

void CInventory::ReceiveItem (CWeaponItem* item, short slot)
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

CWeaponItem* CInventory::AddStacks (CWeaponItem* item, short slot)
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

	return item;
}

void CInventory::SaveInventory (const arstring<256> & location)
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
}

bool CInventory::LoadInventory (const arstring<256> & location)
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
		for (short i = 0; i < sHeight * sWidth; i++)
		{
			int32_t id = fSource.ReadInt32();
			cout <<i << ": " << id << endl;
			if ( id == -1 || id == -2 )
			{
				cout << "Error: Item ID is negative" << endl;
			}
			else if ( WeaponItem::WeaponItemFactory.witem_list.count( id ) == 0)
			{
				pInventory[i] = NULL;
			}
			else
			{
				pInventory[i] = CWeaponItem::Instantiate(id);
				if ( pInventory[i] )
				{
					bWriter >> (*pInventory[i]);
					pInventory[i]->SetOwner( Owner );
					pInventory[i]->SetHoldState( Item::HoldState::Hidden );
				}
			}
		}
	}

	return true;
}

bool CInventory::IsEmpty (void)
{
	for (int i = 0; i < sWidth * sHeight; i++)
	{
		if ((pInventory[i]) != NULL)
			return false;
	}
	return true;
}

// Checks the pInventory for any items that aren't actually owned by our owning actor.
// If the item is not owned, the reference to it is removed and the pointer in the
//  pInventory is set back to null
void CInventory::CheckCurrentItems ( void )
{
	// Loop through all the items
	for ( int i = 0; i < sWidth*sHeight; i++ )
	{
		if ( pInventory[i] != NULL )
		{
			//If the player does not own the item or the stack is empty
			if ( pInventory[i]->GetOwner() == NULL  || pInventory[i]->GetStackSize() <= 0)
			{
				// Remove reference
				pInventory[i] = NULL;
			}
		}
	}
}