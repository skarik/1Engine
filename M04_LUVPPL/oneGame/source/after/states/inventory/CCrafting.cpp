// Main header
#include "CCrafting.h"

#include "engine-common/entities/CActor.h"

// Prototyped classes
#include "after/entities/item/CWeaponItem.h"

// All item classes here
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

CCrafting::CCrafting (short w, short h, CRecipeLibrary* lib, CActor* owner ) : CInventory(owner,w,h),
	singleplayer(true), library(lib)
{
	iResult.id = 0;
	iResult.stack = 0;
	iResult.subid = 0;
}

CCrafting::~CCrafting (void)
{
	singleplayer = 0;
	sWidth = 0;
	sHeight = 0;
}

void CCrafting::Update (void)
{
	GatherIngredients();
	if ( m_craftingBoard.width > 0 && m_craftingBoard.height > 0 ) {
		library->FindAllMatches(m_craftingBoard);
		iResult = library->GetRecipe().result;
	}
	else {
		if ( iResult.id != 0 ) {
			m_craftingBoard.setSize(0,0);
			library->FindAllMatches(m_craftingBoard);
		}
		iResult.id = 0;
		iResult.stack = 0;
		iResult.subid = 0;
	}
	CheckCurrentItems();
}

CWeaponItem* CCrafting::FindItem (short id)
{
	for (int i = 0; i < sWidth; i++)
		for (int j = 0; j < sHeight; j++)
		{
			if (pInventory[i + j * sWidth] != NULL)
			{
				if ((pInventory[i + j * sWidth])->GetID() == id)
				{
					return pInventory[i + j * sWidth];
				}
			}
		}

	return NULL;
}

//Switch the spots of two items
void CCrafting::SwitchSpots (short source, short dest)
{	
	CWeaponItem* temp;

	temp = pInventory[source];
	pInventory[source] = pInventory[dest];
	pInventory[dest] = temp;
}

CWeaponItem* CCrafting::GetItem (short slot)
{
	return pInventory[slot];
}

void CCrafting::ReceiveItem (CWeaponItem* item, short slot)
{
	pInventory[slot] = item;
}

tItemRecipe& CCrafting::GatherIngredients (void)
{
	//tItemRecipe* list;
	tItemRecipe::item_entry zero;
	zero.id = 0;
	zero.stack = 0;
	tItemRecipe::item_entry temp;

	tItemRecipe& list = m_craftingBoard;

	// find bounding box
	int minx = sWidth, miny = sHeight;
	int maxx = 0, maxy = 0;
	for ( int y = 0; y < sHeight; ++y )
	{
		for ( int x = 0; x < sWidth; ++x )
		{
			if ( pInventory[x + y * sWidth] != NULL )
			{
				minx = std::min<int>( minx,x );
				miny = std::min<int>( miny,y );
				maxx = std::max<int>( maxx,x );
				maxy = std::max<int>( maxy,y );
			}
		}
	}

	if ( maxx >= minx && maxy >= miny )
	{
		// set proper size
		list.setSize( maxx-minx+1, maxy-miny+1 );

		for ( int y = miny; y <= maxy; ++y )
		{
			for ( int x = minx; x <= maxx; ++x )
			{
				if (pInventory[x + y * sWidth] != NULL)
				{	
					temp.id = pInventory[x + y * sWidth]->GetID();
					temp.stack = pInventory[x + y * sWidth]->GetStackSize();
					if (temp.id == 2)
					{
						temp.subid = dynamic_cast<ItemTerraBlok*>(pInventory[x + y * sWidth])->GetType();
					}
					list(x-minx,y-miny) = temp;
				}
				else
				{
					list(x-minx,y-miny) = zero;
				}
			}
		}
	}
	else {
		list.setSize(0,0);
	}
	return m_craftingBoard;
}

tItemRecipe::item_entry CCrafting::GetResult (int inResultIndex)
{
	return iResult;
}
const std::vector<tItemRecipe*>& CCrafting::GetAllMatches ( void )
{
	return library->GetAllMatches();
}

void CCrafting::SetRecipeChoice (int choice)
{
	library->SetRecipeChoice(choice);
}

CWeaponItem* CCrafting::AddStacks (CWeaponItem* item, short slot)
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

void CCrafting::MakeItem (void)
{
	const tItemRecipe& temp = library->GetRecipe();
	
	// find bounding box
	int minx = sWidth, miny = sHeight;
	int maxx = 0, maxy = 0;
	for ( int y = 0; y < sHeight; ++y )
	{
		for ( int x = 0; x < sWidth; ++x )
		{
			if ( pInventory[x + y * sWidth] != NULL )
			{
				minx = std::min<int>( minx,x );
				miny = std::min<int>( miny,y );
				maxx = std::max<int>( maxx,x );
				maxy = std::max<int>( maxy,y );
			}
		}
	}

	if ( maxx >= minx && maxy >= miny )
	{
		for ( int y = miny; y <= maxy; ++y )
		{
			for ( int x = minx; x <= maxx; ++x )
			{
				if (pInventory[x + y * sWidth] != NULL) {	
					pInventory[x + y * sWidth]->AddToStack(-1 * temp(x-minx,y-miny).stack);
				}
			}
		}
	}
}

void CCrafting::CheckCurrentItems (void)
{
	for (int i = 0; i < sWidth; i++)
	{
		for (int j = 0; j < sHeight; j++)
		{
			if (pInventory[i + j * sWidth] != NULL)
				if (pInventory[i + j * sWidth]->GetStackSize() < 1)
				{
					delete pInventory[i + j * sWidth];
					pInventory[i + j * sWidth] = NULL;
				}
		}
	}
}

#include "engine/state/CGameState.h"
short CCrafting::NearbyForge (void)
{
	// Start with no forge
	short forgeLevel = 0;
	// Loop through all objects
	std::vector<CGameBehavior*>* allObjects = CGameState::Active()->FindObjectsWithLayer( Layers::Actor );
	for ( auto it = allObjects->begin(); it != allObjects->end(); ++it )
	{
		short nextValue = 0;
		// Get object's forge level
		if ( (*it)->GetTypeName() == "PropCampfire" ) {
			nextValue = 1;
		}
		// Check if object close enough
		if ( nextValue > forgeLevel )
		{ //8 feet (4 blocks
			if ( (Owner->transform.position - ((CActor*)*it)->transform.position).sqrMagnitude() < sqr(8) ) {
				forgeLevel = nextValue;
			}
		}
	}
	// Free temporary list
	delete allObjects;
	// Return found forge level
	return forgeLevel;
}

short CCrafting::PlayerLevel (void)
{
	return 0;
}

bool CCrafting::HasLevelAndForge (void)
{
	if (NearbyForge() < library->GetRecipe().forge)
		return false;
	if (PlayerLevel() < library->GetRecipe().level)
		return false;
	return true;
}

// Send in the forge input to apply forge data
const std::vector<tForgePart>& CCrafting::ForgeFindMatches ( const tForgeInput& forgeInput )
{
	// Check the item in the main slot
	ItemType sourcetype = WeaponItem::TypeDefault;
	ItemAdditives::eItemAdditive sourceadditive = ItemAdditives::None;
	if ( forgeInput.base ) {
		sourcetype = forgeInput.base->GetItemData()->eItemType;
		sourceadditive = ItemAdditives::ItemToAdditive( forgeInput.base );
	}
	else {
		// Send a clear list message to the main thing
		library->ClearForgeMatches();
		return library->GetAllForgeMatches();
	}

	if ( sourceadditive != ItemAdditives::Composite )
	{
		// Go to the forge library and locate matches.
		library->FindAllForgeMatches( forgeInput );
	}
	else {
		library->FindAllReforgeMatches( forgeInput );
	}
	// After this function call, the library's "forgeparts" will be populated with valid values.
	return library->GetAllForgeMatches();
}

const std::vector<ItemType>& CCrafting::ForgeGetTypeMatches ( void )
{
	return library->GetAllForgeTypeMatches();
}

void CCrafting::ForgeDecrementMaterials ( void )
{
	// Get the list from the inventory
	const std::vector<int>& additiveUsageList = library->GetAllForgeAdditives();

	for ( auto additive = additiveUsageList.begin(); additive != additiveUsageList.end(); ++additive )
	{
		if ( *additive != -1 ) {
			CWeaponItem* item = pInventory[1+*additive];
			if ( item ) {
				// Decrement item stack
				if ( item->GetCanStack() ) {
					item->AddToStack(-1);
					// Delete if no more items in stack
					if ( item->GetStackSize() == 0 ) {
						delete item;
						pInventory[1+*additive] = NULL;
					}
				}
				else { // Delete if it doesn't stack
					delete item;
					pInventory[1+*additive] = NULL;
				}
			}
		}
	}

	// Remove the main ingredients. Also remove the enchantments.
	if ( pInventory[0] ) {
		// Decrement item stack
		if ( pInventory[0]->GetCanStack() ) {
			pInventory[0]->AddToStack(-1);
			// Delete if no more items in stack
			if ( pInventory[0]->GetStackSize() == 0 ) {
				delete pInventory[0];
				pInventory[0] = NULL;
			}
		}
		else { // Delete if it doesn't stack
			delete pInventory[0];
			pInventory[0] = NULL;
		}
	}
	// Remove the two enchantments
	for ( int i = 5; i <= 6; ++i ) {
		// Decrement item stack
		if ( pInventory[i] ) {
			if ( pInventory[i]->GetCanStack() ) {
				pInventory[i]->AddToStack(-1);
				// Delete if no more items in stack
				if ( pInventory[i]->GetStackSize() == 0 ) {
					delete pInventory[i];
					pInventory[i] = NULL;
				}
			}
			else { // Delete if it doesn't stack
				delete pInventory[i];
				pInventory[i] = NULL;
			}
		}
	}
}

void CCrafting::ForgeApplyMaterials ( CWeaponItem* resultItem )
{
	// Will do later
}
