#ifndef _CRECIPELIBRARY_H_
#define _CRECIPELIBRARY_H_

#include "ItemCrafting.h"
#include "ItemForging.h"

class CRecipeLibrary
{
public:
	explicit CRecipeLibrary (void);
	~CRecipeLibrary (void);

	// Populates this->matchingRecipes with all matches to the recipe given.
	void	FindAllMatches (const tItemRecipe & ingredients);
	// Returns a reference to the vector of all matches
	const std::vector<tItemRecipe*>& GetAllMatches ( void );
	
	// Return current target recipe
	const tItemRecipe&	GetRecipe ( void );
	// Of the current matches in this->matchingRecipies, set the current match
	void	SetRecipeChoice ( const short mn_recipe=0 );

	// Returns a list of all recipies
	std::vector<tItemRecipe>* GetRecipeLibrary ( short& size );

private:
	// Loads the given external library set
	void LoadLibrarySet ( const char* filename );
	// Loads the default library
	void InitiateTestLibrary (void);

private:
	// Item Recipe Library
	std::vector<tItemRecipe> library;
	// Count of values in the library
	short size;
	short result;
	short recipe;
	// List of matching recipies
	std::vector<tItemRecipe*>	matchingRecipes;
	short currentRecipe;

	tItemRecipe noRecipe;
	tItemRecipe::item_entry noMatch;

public:
	// Find all matching recipies
	void FindAllForgeMatches ( const tForgeInput& );
	void FindAllReforgeMatches ( const tForgeInput& );
	// Clear the matching part list
	void ClearForgeMatches ( void );
	// Returns a reference to the list of all parts
	const std::vector<tForgePart>& GetAllForgeMatches ( void );
	// Returns a reference to the list of all types
	const std::vector<ItemType>& GetAllForgeTypeMatches ( void );
	// Returns a reference to the list of items to eat
	const std::vector<int>& GetAllForgeAdditives ( void );

private:
	// Loads the default forgelist
	void LoadTestForgelist ( void );

	// Parse a string for matching parts. Add matching parts to part list.
	void MatchToPartlist ( const char*, uchar partType, WeaponItem::WeaponItemType );

private:
	// Forge List
	std::vector<tForgeRecipe> forgelist;
	// Forge part list (minimal. Does not contain part information)
	std::vector<tForgePart> forgeparts;
	// List of valid types
	std::vector<ItemType> forgetypes;
	// List of parts that need to be used
	std::vector<int> forge_additiveUsageList;
};

#endif