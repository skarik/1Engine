//#include <iostream>
#include "core/system/io/FileUtils.h"
#include "core/debug/CDebugConsole.h"
#include "core-ext/system/io/mccosf.h"
#include "after/types/terrain/BlockType.h"
#include "CRecipeLibrary.h"

CRecipeLibrary::CRecipeLibrary (void)
{
	// Load in libraries
	{
		//InitiateTestLibrary();
		bool load = true;
		int filecount = 0;
		while ( load ) {
			arstring<256> libraryFile;
			sprintf( libraryFile, ".res/items/recipelibrary%d.txt", filecount );
			if ( IO::FileExists( libraryFile ) ) {
				LoadLibrarySet( libraryFile );
				filecount += 1;
				// Print recipies loaded
				Debug::Console->PrintMessage( "Loaded recipies from " + string(libraryFile) + "\n" );
			}
			else {
				load = false;
			}
		}
	}
	size = library.size();
	result = 0;
	recipe = 0;

	// the "noMatch" entry
	noMatch.id = 0;
	noMatch.subid = 0;
	noMatch.stack = 0;

	currentRecipe = 0;

	// Load in forglists
	{
		LoadTestForgelist();
	}
}

CRecipeLibrary::~CRecipeLibrary (void)
{
	/*if (library) {
		delete [] library;
	}
	library = NULL;*/
}

std::vector<tItemRecipe>* CRecipeLibrary::GetRecipeLibrary ( short& size )
{
	size = this->size;
	return &(library);
}

void CRecipeLibrary::InitiateTestLibrary (void)
{
	tItemRecipe::item_entry temp;
	temp.id = 0;
	temp.stack = 0;

	// Set up library size
	library.resize(11);

	//Initiate the torch recipe
	library[0].name = "Torch";
	library[0].result.id = 4;
	library[0].result.stack = 3;

	library[0].setSize(1,2);
	temp.id = 6;
	temp.stack = 1;
	library[0](0,0) = temp;
	temp.id = 51;
	temp.stack = 1;
	library[0](0,1) = temp;

	//Initiate the branch recipe
	library[1].name = "Tree Branch";
	library[1].result.id = 51;
	library[1].result.stack = 2;
	library[1].height = 1;
	library[1].width = 1;
	temp.id = 50;
	temp.stack = 1;
	library[1].slot.push_back(temp);

	//Initiate the twig recipe
	library[2].name = "Twig";
	library[2].result.id = 52;
	library[2].result.stack = 4;
	library[2].height = 1;
	library[2].width = 1;
	temp.id = 51;
	temp.stack = 1;
	library[2].slot.push_back(temp);

	//Initiate the DRILLLLLLLLLLLLLLLL!!!!!!!!!!!!!!!!!
	library[3].name = "Hand Drill";
	library[3].result.id = 250;
	library[3].result.stack = 1;
	library[3].height = 1;
	library[3].width = 2;
	temp.id = 4;
	temp.stack = 1;
	library[3].slot.push_back(temp);
	temp.id = 4;
	temp.stack = 1;
	library[3].slot.push_back(temp);

	//Initiate the Crossbow Bolt
	library[4].name = "Crossbow Bolt";
	library[4].result.id = 10;
	library[4].result.stack = 5;
	library[4].height = 1;
	library[4].width = 2;
	temp.id = 4;
	temp.stack = 1;
	library[4].slot.push_back(temp);
	temp.id = 4;
	temp.stack = 1;
	library[4].slot.push_back(temp);

	//Initiate the Wood Block
	library[5].name = "Wood Block";
	library[5].result.id = 2;
	library[5].result.subid = 6;
	library[5].result.stack = 1;
	library[5].height = 2;
	library[5].width = 2;
	temp.id = 52;
	temp.stack = 1;
	for (short i = 0; i < 4; i++)
		library[5].slot.push_back(temp);

	//Initiate the Gravel Block
	library[6].name = "Gravel Block";
	library[6].result.id = 2;
	library[6].result.subid = Terrain::EB_GRAVEL;
	library[6].result.stack = 3;
	library[6].height = 1;
	library[6].width = 1;
	temp.id = 2;
	temp.subid = Terrain::EB_STONE;
	temp.stack = 1;
	library[6].slot.push_back(temp);

	//Initiate the Stone Brick Block
	library[7].name = "Stone Brick";
	library[7].result.id = 2;
	library[7].result.subid = Terrain::EB_STONEBRICK;
	library[7].result.stack = 1;
	library[7].height = 1;
	library[7].width = 1;
	temp.id = 2;
	temp.subid = Terrain::EB_STONE;
	temp.stack = 2;
	library[7].slot.push_back(temp);

	//Initiate the twig recipe
	library[8].name = "Wooden Arrow";
	library[8].result.id = 11;
	library[8].result.stack = 4;
	library[8].height = 2;
	library[8].width = 1;
	temp.id = 52;
	temp.stack = 1;
	for (short i = 0; i < 2; i++)
		library[8].slot.push_back(temp);

	//Initiate the Pebble
	library[9].height = 1;
	library[9].width = 1;
	temp.id = 2;
	temp.subid = Terrain::EB_STONE;
	temp.stack = 1;
	library[9].slot.push_back(temp);

	library[9].name = "Pebble";
	library[9].result.id = 12;
	library[9].result.stack = 30;

	//Initiate the Pebble
	library[10].height = 1;
	library[10].width = 1;
	temp.id = 2;
	temp.subid = Terrain::EB_GRAVEL;
	temp.stack = 1;
	library[10].slot.push_back(temp);

	library[10].name = "Pebble";
	library[10].result.id = 12;
	library[10].result.stack = 10;
}

const tItemRecipe& CRecipeLibrary::GetRecipe ( void )
{
	if ( matchingRecipes.empty() ) {
		return noRecipe;
	}
	if ( currentRecipe < 0 ) {
		return *(matchingRecipes[0]);
	}
	else if ( currentRecipe < matchingRecipes.size() ) {
		return *(matchingRecipes[currentRecipe]);
	}
	else {
		return *(matchingRecipes[matchingRecipes.size()-1]);
	}
}
void CRecipeLibrary::SetRecipeChoice ( const short mn_recipe )
{
	if ( !matchingRecipes.empty() ) {
		if ( mn_recipe < 0 ) {
			currentRecipe = 0;
		}
		else if ( mn_recipe < matchingRecipes.size() ) {
			currentRecipe = mn_recipe;
		}
		else {
			currentRecipe = matchingRecipes.size()-1;
		}
	}
}


void CRecipeLibrary::FindAllMatches (const tItemRecipe & inTable)
{
	result = -1;
	const tItemRecipe& ingredients = inTable;
	matchingRecipes.clear();

	// Loop through all the 
	for (int i = 0; i < size; i++)
	{
		// Don't have result if mismatch on size
		if ( ingredients.width != library[i].width || ingredients.height != library[i].height ) {
			continue;
		}

		result = -1;
		for (int y = 0; y < library[i].height; y++)
		{
			for (int x = 0; x < library[i].width; x++)
			{
				if ( library[i](x,y).id != ingredients(x,y).id ) {
					x += library[i].width; y += library[i].height;	// Break out of loop if items don't match
					result = -1;
				}
				else if ( (ingredients(x,y).id == 2 && library[i](x,y).id == 2) && (library[i](x,y).subid != ingredients(x,y).subid) )
				{
					//cout << "This could be a problem" << endl;
					x += library[i].width; y += library[i].height;	// Break out of loop if subtype doesn't match (specific case for Terra bloks)
					result = -1;
				}
				else if ( library[i](x,y).stack > ingredients(x,y).stack ) {
					x += library[i].width; y += library[i].height;	// Break out of loop if there aren't enough of the item
					result = -1;
				}
				else
				{
					result = i;
				}
			}
		}
		// Found result, so break
		if (result != -1) {
			matchingRecipes.push_back( &(library[result]) );
		}
	}
}

const std::vector<tItemRecipe*>& CRecipeLibrary::GetAllMatches ( void )
{
	return matchingRecipes;
}

void CRecipeLibrary::LoadLibrarySet ( const char* filename )
{
	FILE* location;
	//location = fopen (".res/system/Recipe Library.txt", "rb");
	location = fopen (filename, "rb");

	COSF_Loader librarian (location);

	mccOSF_entry_info_t sCurrentLine;

	sCurrentLine.level = 0;
	sCurrentLine.nextchar = 0;
	sCurrentLine.type = mccOSF_entrytype_enum::MCCOSF_ENTRY_NORMAL;

	tItemRecipe blank;
	tItemRecipe::item_entry empty;

	empty.id = 0;
	empty.subid = 0;
	empty.stack = 0;
	empty.userdata = "";

	blank.width = 0;
	blank.name = "";
	blank.height = 0;
	blank.forge = 0;
	blank.level = 0;
	blank.unlocked = true;
	blank.result = empty;

	librarian.GetNext (sCurrentLine);
	auto spot = library.begin();
	do
	{
		bool found = false;

		if (sCurrentLine.type == mccOSF_entrytype_enum::MCCOSF_ENTRY_OBJECT)
		{
			if ((strcmp(sCurrentLine.name, "Name") == 0)||(strcmp(sCurrentLine.name, "name") == 0))
			{
				//for (short i = 0; i < library.size(); i++)
				spot = library.begin();
				for (; spot != library.end(); ++spot)
				{
					if (strcmp(spot->name.c_str(), sCurrentLine.value) >= 0)
					{
						spot = library.insert (spot, blank);
						spot->name = sCurrentLine.value;
						found = true;
						break;
					}
				}
				if (found == false)
				{
					//spot = library.end();
					spot = library.insert (library.end(), blank);
					spot->name = sCurrentLine.value;
				}
			}
			librarian.GoInto (sCurrentLine);
		}
		else if (sCurrentLine.type == mccOSF_entrytype_enum::MCCOSF_ENTRY_NORMAL)
		{
			do 
			{
				if (strcmp (sCurrentLine.name, "Width:") == 0)
				{
					spot->width = atoi (sCurrentLine.value);
				}
				else if (strcmp (sCurrentLine.name, "Height:") == 0)
				{
					spot->height = atoi (sCurrentLine.value);
				}
				else if (strcmp (sCurrentLine.name, "Forge:") == 0)
				{
					spot->forge = atoi (sCurrentLine.value);
				}
				else if (strcmp (sCurrentLine.name, "Level:") == 0)
				{
					spot->level = atoi (sCurrentLine.value);
				}
				else if (strcmp (sCurrentLine.name, "ID:") == 0)
				{
					spot->result.id = atoi (sCurrentLine.value);
				}
				else if (strcmp (sCurrentLine.name, "SubID:") == 0)
				{
					spot->result.subid = atoi (sCurrentLine.value);
				}
				else if (strcmp (sCurrentLine.name, "Stack:") == 0)
				{
					spot->result.stack = atoi (sCurrentLine.value);
				}
				else if (strcmp (sCurrentLine.name, "Ingredients") == 0)
				{
					librarian.GoInto (sCurrentLine);
				}
				else
				{
					empty.id = atoi (strtok (sCurrentLine.value, " "));
					empty.subid = atoi (strtok (NULL, " "));
					empty.stack = atoi (strtok (NULL, " "));
					spot->slot.push_back (empty);
				}
				librarian.GetNext (sCurrentLine);
			} while (sCurrentLine.type != mccOSF_entrytype_enum::MCCOSF_ENTRY_END);
		}
		librarian.GetNext (sCurrentLine);
	} while (sCurrentLine.type != mccOSF_entrytype_enum::MCCOSF_ENTRY_EOF);
}