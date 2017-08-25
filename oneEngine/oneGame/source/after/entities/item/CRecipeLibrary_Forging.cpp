
#include "CRecipeLibrary.h"

//#include <iostream>
#include "core/system/io/FileUtils.h"
#include "core/system/io/CBinaryFile.h"
#include "core/debug/CDebugConsole.h"
#include "core-ext/system/io/mccosf.h"
#include "core-ext/system/io/Resources.h"

#include "after/entities/item/CWeaponItem.h"

void CRecipeLibrary::LoadTestForgelist ( void )
{
	// paninis
	tForgeRecipe recipe;
	recipe.componentsMain = "unremarkable";
	recipe.componentsSecondary = "unremarkable";
	recipe.type = WeaponItem::TypeWeaponSword;

	recipe.req_additive = ItemAdditives::Wood;
	recipe.req_additives[0] = ItemAdditives::Wood;
	recipe.req_additives[1] = ItemAdditives::Wood;

	forgelist.push_back( recipe );
}


// Find all matching recipies
void CRecipeLibrary::FindAllForgeMatches ( const tForgeInput& table )
{
	ClearForgeMatches();
	// Loop through the forge library
	for ( auto forgeEntry = forgelist.begin(); forgeEntry != forgelist.end(); ++forgeEntry )
	{
		// Check for match
		bool matches = true;
		// Assume starting a match. Just skip if a mismatch.
		if ( forgeEntry->req_item != WeaponItem::TypeDefault && table.base && table.base->GetItemType() != forgeEntry->req_item )
			continue;
		if ( forgeEntry->req_additive != ItemAdditives::None && table.base && ItemAdditives::ItemToAdditive( table.base ) != forgeEntry->req_additive )
			continue;

		// Loop through additives
		//bool hasMatch [4] = {false,false,false,false};
		bool itemUsed [4] = {false,false,false,false};
		for ( uint i = 0; i < 4; ++i ) {
			/*if ( forgeEntry->req_items[i] != WeaponItem::TypeDefault && table.additives[i] && table.additives[i]->GetItemType() != forgeEntry->req_items[i] )
				matches = false;
			if ( forgeEntry->req_additives[i] != ItemAdditives::None && table.additives[i] && ItemAdditives::ItemToAdditive( table.additives[i] ) != forgeEntry->req_additives[i] )
				matches = false;*/
			int usedJ = -1;
			bool hasMatch = false;
			if ( forgeEntry->req_items[i] == WeaponItem::TypeDefault && forgeEntry->req_additives[i] == ItemAdditives::None )
			{
				hasMatch = true;
			}
			else {
				for ( uint j = 0; j < 4; ++j )
				{
					if ( ( itemUsed[j] == false )
						&&( forgeEntry->req_items[i] == WeaponItem::TypeDefault || table.additives[j] && table.additives[j]->GetItemType() == forgeEntry->req_items[i] )
						&&( forgeEntry->req_additives[i] == ItemAdditives::None || table.additives[j] && ItemAdditives::ItemToAdditive( table.additives[j] ) == forgeEntry->req_additives[i] ) )
					{
						hasMatch = true;
						usedJ = j;
						itemUsed[j] = true;
						break; // Use first match.
					}
				}
			}
			if ( !hasMatch ) {
				matches = false;
			}
			else {
				// add match the list, saving the J that was used
				//push back j
				//list must be returned back to the craft inventory, subtracting items
				forge_additiveUsageList.push_back(usedJ);
			}
		}
		if ( !matches )
			continue;
		// Matches this much, so add this type to the valid types list
		forgetypes.push_back( forgeEntry->type );
		// Doesn't match completely, so can't add its parts to the part list
		if ( forgeEntry->type != table.targetType )
			continue;
		// If there's a forge match, grab all the matching parts.
		MatchToPartlist( forgeEntry->componentsMain, 0, table.targetType );
		MatchToPartlist( forgeEntry->componentsSecondary, 1, table.targetType );
		MatchToPartlist( forgeEntry->components3, 2, table.targetType );
		MatchToPartlist( forgeEntry->components4, 3, table.targetType );
		// **Must parse each part. Best to do it piece by piece with a helper function,
		// **as finding the parts is not trivial.
	}
}
void CRecipeLibrary::FindAllReforgeMatches ( const tForgeInput& table )
{
	ClearForgeMatches();
}
// Clear the matching part list
void CRecipeLibrary::ClearForgeMatches ( void )
{
	forgeparts.clear();
	forgetypes.clear();
	forge_additiveUsageList.clear();
}

// Returns a reference to the list of all parts
const std::vector<tForgePart>& CRecipeLibrary::GetAllForgeMatches ( void )
{
	return forgeparts;
}
// Returns a reference to the list of all types
const std::vector<ItemType>& CRecipeLibrary::GetAllForgeTypeMatches ( void )
{
	return forgetypes;
}
// Returns a reference to the list of items to eat
const std::vector<int>& CRecipeLibrary::GetAllForgeAdditives ( void )
{
	return forge_additiveUsageList;
}

// Parse a string for matching parts. Add matching parts to part list.
void CRecipeLibrary::MatchToPartlist ( const char* matchName, uchar partType, WeaponItem::WeaponItemType weaponType )
{
	// With the input matchName, generate a list of names to match
	if ( strlen(matchName) == 0 ) {
		return; // No matches here heheheh
	}
	std::vector<arstring<128>> tMatchnames;
	arstring<256> localMatchname;
	strcpy( localMatchname, matchName );
	char* pCurrentCheck = strtok( localMatchname, ";" );
	while ( pCurrentCheck != NULL ) {
		//if ( strlen( pCurrentCheck ) > 0 ) {
			// Valid name
			tMatchnames.push_back( arstring<128>(pCurrentCheck) );
		/*}
		else {
			// Empty name
			
		}*/
		pCurrentCheck = strtok( NULL, ";" );
	}

	for ( uint i = 0; i < tMatchnames.size(); ++i ) {
		if ( tMatchnames[i].length() == 0 ) {
			tForgePart newPart;
			newPart.componentIdentifier = "";
			newPart.partType = partType;
		}
	}
	int matchecount = 0;

	int tifilecount = 0;
	while ( true )
	{
		arstring<256> tPartlistFilename;
		sprintf( tPartlistFilename, "items/sword%d.txt", tifilecount );
		tPartlistFilename = Core::Resources::PathTo( tPartlistFilename ).c_str();
		if ( !IO::FileExists( tPartlistFilename ) ) {
			break;
		}
		tifilecount += 1;
	
		// Open up the target partlist.
		CBinaryFile tPartlistFile;
		tPartlistFile.Open( tPartlistFilename, CBinaryFile::IO_READ );

		COSF_Loader partfile ( tPartlistFile.GetFILE() );
		mccOSF_entry_info_t currentEntry;
		do
		{
			// Read until the first read object
			partfile.GetNext( currentEntry );
			if ( currentEntry.type != MCCOSF_ENTRY_OBJECT ) {
				continue;
			}
			
			// Hit the object, check the key data for a valid key
			uchar typeValue = 255;
			if ( strcmp(currentEntry.name,"blade") == 0 )		typeValue = 0;
			if ( strcmp(currentEntry.name,"part0") == 0 )		typeValue = 0;
			if ( strcmp(currentEntry.name,"hilt") == 0 )		typeValue = 1;
			if ( strcmp(currentEntry.name,"handle") == 0 )		typeValue = 1;
			if ( strcmp(currentEntry.name,"part1") == 0 )		typeValue = 1;
			if ( strcmp(currentEntry.name,"part2") == 0 )		typeValue = 2;
			if ( strcmp(currentEntry.name,"part3") == 0 )		typeValue = 3;
			// Invalid object, skip it
			if ( typeValue != partType ) {
				continue;
			}
			
			// Compare its name to the given list
			bool matches = false;
			for ( uint i = 0; i < tMatchnames.size(); ++i )
			{
				uint tLength = tMatchnames[i].length();
				if ( tLength > 0 ) {
					// Only looking for if the beginning matches. No other matches accepted.
					if ( strncmp( tMatchnames[i], currentEntry.value, tLength ) == 0 )
					{
						matches = true;
					}
				}
				// Found a match, stop looking
				if ( matches ) {
					break;
				}
			}
			// No match, skip it.
			if ( !matches ) {
				continue;
			}

			// Still here, meaning that this is a part 
			tForgePart newPart;
			newPart.componentIdentifier = currentEntry.value;
			newPart.partType = partType;

			forgeparts.push_back( newPart );

			matchecount += 1;
		}
		while ( currentEntry.type != MCCOSF_ENTRY_EOF );
		tPartlistFile.Close();
	}

	std::cout << "Matched " << matchecount << " in type " << partType << std::endl;
}