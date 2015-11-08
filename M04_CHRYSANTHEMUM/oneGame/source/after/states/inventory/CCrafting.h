#ifndef _CCRAFTING_H_
#define _CCRAFTING_H_

//#include "engine/behavior/CGameBehavior.h"
#include "after/entities/item/ItemCrafting.h"
#include "after/entities/item/CRecipeLibrary.h"
#include "CInventory.h"

#include <vector>

class CWeaponItem;
class CActor;

class CCrafting : public CInventory
{
public:
	explicit CCrafting( short w, short h, CRecipeLibrary* lib, CActor* owner );
	~CCrafting(void);

	//Worry about this later
	void Update (void);

	//Find an item in the pInventory
	CWeaponItem* FindItem (short id);

	//Switch the spots of two items
	void SwitchSpots (short source, short dest);
	
	//Get a single item from the pInventory
	CWeaponItem* GetItem (short slot);

	//Place an item into the pInventory
	void ReceiveItem (CWeaponItem* item, short slot);

	//Prepare a list for searching through the recipe library
	tItemRecipe& GatherIngredients (void);
	//Return the crafting result
	tItemRecipe::item_entry GetResult (int inResultIndex=0);
	//Generate list of all capable matches
	const std::vector<tItemRecipe*>& GetAllMatches ( void );
	// Set choice recipie (from multiple count)
	void SetRecipeChoice (int choice);
	// Return if a recipe needs a forge of some sort
	short NearbyForge (void);
	// Return if the forger needs a certain skill level
	short PlayerLevel (void);
	//Add two stacks of items together
	CWeaponItem* AddStacks (CWeaponItem* item, short slot);

	void MakeItem (void);

	bool HasLevelAndForge (void);

	CWeaponItem* operator () ( const unsigned char x, const unsigned char y )
	{
		if (pInventory[x+y*sWidth] != NULL)
			return pInventory[x+y*sWidth];
		else
			return NULL;
	}

	// Send in the forge input to apply forge data
	const std::vector<tForgePart>& ForgeFindMatches ( const tForgeInput& forgeInput );
	const std::vector<ItemType>& ForgeGetTypeMatches ( void );
	void ForgeApplyMaterials ( CWeaponItem* );
	void ForgeDecrementMaterials ( void );

private:
	CRecipeLibrary* library;
	CWeaponItem* product;
	tItemRecipe::item_entry iResult;
	tItemRecipe		m_craftingBoard;

	bool singleplayer;

	// Function to check all items and make sure they're actually valid
	void CheckCurrentItems ( void );
};
#endif