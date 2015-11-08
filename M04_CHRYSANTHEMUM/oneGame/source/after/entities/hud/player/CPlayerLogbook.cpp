
#include "CPlayerLogbook.h"
#include "core/input/CInput.h"
#include "core-ext/input/CInputControl.h"
#include "core/system/Screen.h"
#include "after/entities/item/CRecipeLibrary.h"
#include "after/entities/item/CWeaponItem.h"

#include "after/entities/item/system/ItemTerraBlok.h"

#include "core/system/io/FileUtils.h"
#include <fstream>
#include "yaml-cpp/yaml.h"

#include "renderer/material/glMaterial.h"
#include "renderer/system/glMainSystem.h"
#include "renderer/system/glDrawing.h"
#include "core/math/Rect.h"
#include "core/utils/StringUtils.h"

//#include <boost/algorithm/string.hpp>
#include <filesystem>

using std::cout;
using std::endl;

//namespace fs = boost::filesystem;
namespace fs = std::tr2::sys;

CPlayerLogbook::CPlayerLogbook ( CRecipeLibrary* p_lib )
	: CGameBehavior (), CRenderableObject (),
	p_recipe_Library( p_lib )
{
	// Set render type
	renderSettings.renderHints = RL_WORLD;
	renderType = Renderer::V2D;

	// Load all the font type stuff
	fntPageTitle	= new CBitmapFont ( "benegraphic.ttf", (int) (26*1.6f), FW_BOLD );
	fntPageText		= new CBitmapFont ( "benegraphic.ttf", (int) (16*1.7f), FW_NORMAL );
	fntLogbookHelp	= new CBitmapFont ( "benegraphic.ttf", (int) (20*1.7f), FW_BOLD );
	texNull			= new CTexture ( "null" );

	matDrawer	= new glMaterial();
	matDrawer->m_diffuse = Color( 1.0f,1,1 );
	matDrawer->setTexture( 0, texNull );
	matDrawer->passinfo.push_back( glPass() );
	matDrawer->passinfo[0].m_lighting_mode	= Renderer::LI_NONE;
	matDrawer->passinfo[0].shader = new glShader( ".res/shaders/v2d/default.glsl" );
	matDrawer->removeReference();
	SetMaterial( matDrawer );

	// Set logbook mode
	iLogbookMode = 0;
	bDragBook = false;
	bIsBusyHolding = false;

	// Load full logbook
	root_logbook.name = "Logbook";
	//LoadFullLogbook();
	lb_current_entry.name = "Logbook";
	lb_current_entry.filename = "";
	UpdateDisplayedEntry(-1);

	// Set default lookat values
	p_holder_CurrentEntry	= NULL;
	p_holder_CurrentGroup	= &root_logbook;
	s_holder_CurrentSubject	= "";
	b_holder_InSubjectView	= false;
	UpdateHolderPosition( false );

	// Load primary recipe library
	InitRecipeLogbook();

	// Start hidden
	visible = false;

	// Set visibility
	//offset.x = 0;
	//anim_position.x = 0
	position.x = 0.8f;
	position.y = 0.1f;
	anim_position = Vector2d( 0,0 );
	offset = Vector2d( 0,0 );

	// Create input control
	mControl = new CInputControl( this );
	// Reset substate
	i_logbookSubstate = 0;
}
CPlayerLogbook::~CPlayerLogbook ( void )
{
	// Free drawing stuff
	delete fntPageTitle;
	delete fntPageText;
	delete fntLogbookHelp;
	//matDrawer->removeReference();
	//delete matDrawer;

	// Free control stuff
	delete mControl;
	mControl = NULL;
}

void CPlayerLogbook::SetVisibility ( bool & visibility, bool forceDirty )
{
	if ( forceDirty ) {
		bVisibilityDirty = true;
	}
	else if ( bVisibilityDirty ) {
		visibility = visible;
		bVisibilityDirty = false;
	}
	if ( visible && !visibility ) {
		mControl->Release();
	}
	visible = visibility;
}
void CPlayerLogbook::SetTab ( const int tabNum )
{
	if ( tabNum >= 0 && tabNum <= 2 ) {
		iLogbookMode = tabNum;
	}
}

void CPlayerLogbook::LoadFullLogbook ( void )
{
	fs::path	currentPath;
	string		currentEntry;
	int			targetDepth;
	int			currentChar;
	string		currentSubentry;
	sLogbookGroup*	currentGroup	= &root_logbook;

	// First, go into the system logbook area
	fs::path	targetDir ( ".res\\logbook\\entries" ); 

	// Loop through every file in there
	fs::directory_iterator end_itr;
	for ( fs::directory_iterator dir_itr ( targetDir ); dir_itr != end_itr; ++dir_itr )
	{
		if ( !fs::is_directory( dir_itr->status() ) )
		{
			// For every file, take the file name
			currentPath = dir_itr->path();

			// With the file name, strip the path and the extension
			//currentname = currentPath.filename().string();
			//currentname = currentname.substr( 0, currentname.size()-4 );
			currentEntry = currentPath.stem();

			//cout << currentEntry << endl;

			// Now we have the logbook entry name, we go from dot to dot.
			// First, we grab the recursion amount by counting the dots
			targetDepth = 0;
			for ( unsigned int i = 0; i < currentEntry.size(); ++i )
			{
				if ( currentEntry[i] == '.' )
					targetDepth += 1;
			}
			// Now, we recurse that many times, looking for a section each recurse
			currentGroup = &root_logbook;
			
			currentChar = 0;
			for ( int step = 0; step < targetDepth; ++step )
			{
				// First, grab the section name (add characters to a string until hit a dot)
				currentSubentry = "";
				while ( currentEntry[currentChar] != '.' )
				{
					currentSubentry += currentEntry[currentChar];
					++currentChar;
				}
				++currentChar;
				//cout << "  -" << currentSubentry << endl;

				// At current depth, look for the next section
				bool found = false;
				for ( std::vector<sLogbookGroup*>::iterator it = currentGroup->subgroups.begin(); it != currentGroup->subgroups.end(); ++it )
				{
					if ( (*it)->name == currentSubentry )
					{
						found = true;
						currentGroup = *it;
						break;
					}
				}
				// If the section doesn't exist, create it
				if ( !found )
				{
					sLogbookGroup* newGroup = new sLogbookGroup;
					newGroup->name = currentSubentry;
					currentGroup->subgroups.push_back( newGroup );
					currentGroup = newGroup;
				}
			}

			// For the last recursion, it's going to be a data add instead
			{
				// First, grab the section name (add characters to a string until hit a dot)
				currentSubentry = "";
				while ( currentChar < (signed)currentEntry.size() )
				{
					currentSubentry += currentEntry[currentChar];
					++currentChar;
				}
				//cout << "   data:" << currentSubentry << " at " << currentPath << endl;

				// Now, add this new entry to the current logbook group
				sLogbookEntry* newEntry = new sLogbookEntry;
				newEntry->name = currentSubentry;
				newEntry->filename = currentPath.string();
				currentGroup->entries.push_back( newEntry );
			}
		}
	}

	cout << "---" << endl;

	// Do final recurse loop to check
	//DebugPrintGroup( &root_logbook, 0 );
}

void CPlayerLogbook::DebugPrintGroup ( const sLogbookGroup * currentGroup, int indent )
{
	// First print out entries
	for ( std::vector<sLogbookEntry*>::const_iterator it = currentGroup->entries.begin(); it != currentGroup->entries.end(); ++it )
	{
		// print out indent
		for ( int i = 0; i < indent; ++i )
			cout << " ";

		// Print out entry
		cout << "data:" << (*it)->name << " at " << (*it)->filename << endl;
	}
	for ( std::vector<sLogbookGroup*>::const_iterator it = currentGroup->subgroups.begin(); it != currentGroup->subgroups.end(); ++it )
	{
		// print out indent
		for ( int i = 0; i < indent; ++i )
			cout << " ";

		// Print out entry
		cout << "+" << (*it)->name << endl;

		// go inside this one
		DebugPrintGroup( (*it), indent+2 );
	}
}

void CPlayerLogbook::Update ( void )
{
	// Move entire logbook based on visibility
	if ( visible ) {
		//anim_position.x += (0.16f - anim_position.x)*Time::TrainerFactor( 30.0f );
	}
	else {
		//anim_position.x = -0.4f;
		bDragBook = false;
	}
	offset = position + anim_position;

	// Don't update unless visible
	if ( !visible )
		return;

	// Calculate mouseover offset
	Vector2d vMousePos = Vector2d( CInput::MouseX()/(ftype)Screen::Info.height, CInput::MouseY()/(ftype)Screen::Info.height );

	// Based on mode, perform logbook controls
	if ( bDragBook )
	{
		// Check mouse choice
		Vector2d vDeltaPoint = Vector2d( Input::DeltaMouseX()/(ftype)Screen::Info.height, Input::DeltaMouseY()/(ftype)Screen::Info.height );
		
		position += vDeltaPoint;

		Vector2d size ( 0.65f, 0.8f );
		Vector2d border;
		Vector2d limit;
		limit.x = Screen::Info.width / (ftype)Screen::Info.height;
		limit.y = 1;
		border.y = 0.05f;
		border.x = 0.05f;

		if ( position.x < border.x - size.x ) {
			position.x = border.x - size.x + 0.001f;
		}
		else if ( position.x > limit.x - border.x ) {
			position.x =  limit.x - border.x - 0.001f;
		}

		if ( position.y < border.y - size.y ) {
			position.y = border.y - size.y + 0.001f;
		}
		else if ( position.y > limit.y - border.y ) {
			position.y =  limit.y - border.y - 0.001f;
		}

		if ( CInput::MouseUp(CInput::MBLeft) || bIsBusyHolding ) {
			bDragBook = false;
		}
	}
	else
	{
		if (( vMousePos.x > 0.65f+offset.x )&&( vMousePos.x < 0.65f+offset.x+0.06f ))
		{
			i_mouseOver_tabSelect = (int)((vMousePos.y-(0.05f+offset.y))/0.10f);
			if ( vMousePos.y > offset.y + 0.05f + i_mouseOver_tabSelect*0.10f + 0.08f )
				i_mouseOver_tabSelect = -1;
		}
		else {
			i_mouseOver_tabSelect = -1;
		}
		// If left clicked, figure out where to go to
		if ( i_mouseOver_tabSelect != -1 )
		{
			if ( CInput::MouseDown(CInput::MBLeft) )
			{
				if (( i_mouseOver_tabSelect >= 0 )&&( i_mouseOver_tabSelect <= 2 ))
				{
					mControl->Release();
					i_logbookSubstate = 0;
					iLogbookMode = i_mouseOver_tabSelect;
				}
			}
		}
		else if ( i_mouseOver_Offset == -1 ) {
			// If not clicked a tab, drag the logbook
			if ( CInput::MouseDown(CInput::MBLeft) )
			{
				if ( (vMousePos.x > offset.x) && (vMousePos.x < offset.x + 0.65f)
					&& (vMousePos.y > offset.y) && (vMousePos.y < offset.y + 0.8f) )
				{
					if ( (vMousePos.x < offset.x + 0.1f) || (vMousePos.x > offset.x + 0.55f)
						|| (vMousePos.y < offset.y + 0.1f) || (vMousePos.y > offset.y + 0.7f) ) {
						if ( !bIsBusyHolding ) {
							bDragBook = true;
						}
					}
				}
			}
		}

		if ( iLogbookMode == 0 ) {
			//UpdateLogbook( vMousePos );
			UpdateLogbookView( vMousePos );
		}
		else if ( iLogbookMode == 1 ) {
			UpdateRecipebook( vMousePos );
		}
		else if ( iLogbookMode == 2 ) {
			// NOTHING
		}
	}
}

void CPlayerLogbook::UpdateLogbook ( Vector2d vMousePos )
{
	i_mouseOver_Offset = -1;
	if (( vMousePos.x > 0.05f+offset.x )&&( vMousePos.x < 0.60f+offset.x ))
	{
		i_mouseOver_Offset = (int)((vMousePos.y-(0.08f+offset.y))/0.04f);
		if ( vMousePos.y > offset.y + 0.08f + i_mouseOver_Offset*0.04f + 0.035f )
			i_mouseOver_Offset = -1;
	}

	// If left clicked, figure out where to go to
	if ( i_mouseOver_Offset != -1 )
	{
		if ( CInput::MouseDown(CInput::MBLeft) )
		{
			// If the offset is in range
			if ( i_mouseOver_Offset < (signed)(p_holder_CurrentGroup->entries.size()+p_holder_CurrentGroup->subgroups.size()) )
			{
				// If the offset is greater than or equal to the size of the entry list, then we clicked on a group
				if ( i_mouseOver_Offset >= (signed)p_holder_CurrentGroup->entries.size() )
				{
					int iTargetGroup = i_mouseOver_Offset - p_holder_CurrentGroup->entries.size();
					// We now have the group index, so now edit the string address
					s_holder_CurrentSubject += '.';
					s_holder_CurrentSubject += p_holder_CurrentGroup->subgroups[iTargetGroup]->name;
					UpdateHolderPosition( false );
				}
				else // Otherwise, we clicked on an entry
				{
					// We now have the entry index, so now edit the string address
					s_holder_CurrentSubject += '.';
					s_holder_CurrentSubject += p_holder_CurrentGroup->entries[i_mouseOver_Offset]->name;
					UpdateHolderPosition( true );
					UpdatePageInfo();
				}
			}
		}
	}
	if ( CInput::MouseDown(CInput::MBRight) ) 
	{
		// Go up a section if right-clicked
		int iLastPosition = 0;
		if ( s_holder_CurrentSubject.size() > 0 )
		{
			for ( int i = 0; i < (signed)s_holder_CurrentSubject.size(); ++i )
			{
				if ( s_holder_CurrentSubject[i] == '.' )
					iLastPosition = i;
			}
			s_holder_CurrentSubject = s_holder_CurrentSubject.substr( 0, iLastPosition );
			UpdateHolderPosition( false );
		}
	}
}

void CPlayerLogbook::UpdateLogbookView ( Vector2d vMousePos )
{
	i_mouseOver_Offset = -1;
	if (( vMousePos.x > 0.05f+offset.x )&&( vMousePos.x < 0.60f+offset.x ))
	{
		i_mouseOver_Offset = (int)((vMousePos.y-(0.08f+offset.y))/0.04f);
		if ( vMousePos.y > offset.y + 0.08f + i_mouseOver_Offset*0.04f + 0.035f )
			i_mouseOver_Offset = -1;
	}

	// If left clicked, figure out where to go to
	if ( i_mouseOver_Offset != -1 )
	{
		if ( CInput::MouseDown(CInput::MBLeft) )
		{
			if ( i_mouseOver_Offset < (signed)s_subentries.size() )
			{
				UpdateDisplayedEntry( i_mouseOver_Offset );
			}
		}
	}
	// If right clicked, remove an entry
	if ( CInput::MouseDown(CInput::MBRight) )
	{
		UpdateDisplayedEntry( -1 );
	}
}

void CPlayerLogbook::UpdateDisplayedEntry ( const int entryIndex )
{
	if ( entryIndex >= 0 )
	{
		/*if ( lb_current_entry.filename.length() > 0 ) {
			lb_current_entry.filename = lb_current_entry.filename + "." + s_subentries[entryIndex];
		}
		else {
			lb_current_entry.filename = "$" + s_subentries[entryIndex];
		}*/
		if ( lb_current_entry.filename.length() > 1 ) {
			lb_current_entry.filename = lb_current_entry.filename + "." + s_subentries[entryIndex];
		}
		else {
			lb_current_entry.filename = "$" + s_subentries[entryIndex];
		}
	}
	else
	{
		if ( lb_current_entry.filename.find_last_of('.') != std::string::npos ) {
			lb_current_entry.filename = lb_current_entry.filename.substr( 0,lb_current_entry.filename.find_last_of('.') );
		}
		else {
			lb_current_entry.filename = "$";
		}
	}
	cout << lb_current_entry.filename << endl;

	// Load Subentries
	s_subentries.clear();
	{
		fs::path	currentPath;
		string		currentEntry;
		string		currentSubentry;
		// Go into the system logbook area
		fs::path	targetDir ( ".res/logbook/entries" ); 

		// Loop through every file in there
		fs::directory_iterator end_itr;
		for ( fs::directory_iterator dir_itr ( targetDir ); dir_itr != end_itr; ++dir_itr )
		{
			if ( !fs::is_directory( dir_itr->status() ) )
			{
				// For every file, take the file name
				currentPath = dir_itr->path();
				// With the file name, strip the path and the extension
				currentEntry = "$" + currentPath.stem();
				// Match entry with current entry.
				if ( currentEntry.find( lb_current_entry.filename ) != string::npos )
				{
					//cout << "match: " << currentEntry << endl;
					//cout << "   vs: " << lb_current_entry.filename << endl;
					// If entry is equal, skip
					if ( currentEntry == lb_current_entry.filename ) {
						continue;
					}
					// If entry matches, add suffix to the s_subentries list if unique.
					if ( lb_current_entry.filename.length() > 1 ) {
						currentSubentry = currentEntry.substr(lb_current_entry.filename.length()+1);
					}
					else {
						currentSubentry = currentEntry.substr(lb_current_entry.filename.length());
					}
					//cout << "     --" << currentSubentry << endl;
					if ( currentSubentry.length() > 1 ) {
						// Cut off last part of suffix
						uint nextpos = currentSubentry.find_first_of('.');
						if ( nextpos != string::npos ) {
							currentSubentry = currentSubentry.substr(0,nextpos);
						}
						//cout << "     --" << currentSubentry << endl;
						// Search and add if unique
						if ( std::find( s_subentries.begin(), s_subentries.end(), currentSubentry ) == s_subentries.end() ) {
							s_subentries.push_back( currentSubentry );
						}
					}
				}
				// End entry name matching
			}
		}
		// End loop through directory
	}

	// Load entry
	{
		char filename [512];
		sprintf( filename, ".res/logbook/entries/%s.yml", lb_current_entry.filename.substr(1).c_str() );
		if ( IO::FileExists( filename ) )
		{
			using std::ifstream;

			// Load file
			ifstream inputFile ( filename );

			// Parse data in YAML document.
			YAML::Parser ymlParser ( inputFile );

			YAML::Node ymlDocument;
			if ( ymlParser.GetNextDocument( ymlDocument ) )
			{
				const YAML::Node*		pCurrentNode;
				
				pCurrentNode = ymlDocument.FindValue( "title" );
				s_entry_title = "";
				if ( pCurrentNode ) {
					*pCurrentNode >> s_entry_title;
				}

				pCurrentNode = ymlDocument.FindValue( "default" );
				s_entry_description = "";
				if ( pCurrentNode ) {
					*pCurrentNode >> s_entry_description;
				}
			}

			// Explcitly close the file
			inputFile.close();
		}
		else 
		{
			if ( lb_current_entry.filename.length() == 1 ) {
				s_entry_title = "Observation Log";
			}
			else {
				s_entry_title = "";
			}
			s_entry_description = "";
		}
	}
}


void CPlayerLogbook::InitRecipeLogbook ( void )
{
	short listSize;
	auto library = p_recipe_Library->GetRecipeLibrary( listSize );
	for ( int i = 0; i < listSize; ++i )
	{
		sLogbookRecipe recipeEntry;
		recipeEntry.recipe = &(*library)[i];
		s_recipe_CurrentList.push_back( recipeEntry );
	}
	// Set positioning
	v_recipe_DrawPos.x = 0.05f;
	v_recipe_DrawPos.y = 0.51f;

	// Set default recipe lookat values
	s_recipe_CurrentSubject = "";
	b_recipe_InSubjectView	= false;
	i_recipe_ScrollSize = 8;
	i_recipe_ScrollOffset = 0;
	f_recipe_ScrollBarPos = 0;
}

void CPlayerLogbook::SearchRecipies ( string resultName, std::vector<sLogbookRecipe> &listToEdit )
{
	if ( resultName.length() > 0 ) {
		// convert search result to lowercase
		resultName = StringUtils::ToLower( resultName );
		// clear current results
		s_recipe_CurrentList.clear();

		short listSize;
		auto library = p_recipe_Library->GetRecipeLibrary( listSize );
		for ( int i = 0; i < listSize; ++i )
		{
			string name = (*library)[i].name;
			name = StringUtils::ToLower( name );

			if ( name.find(resultName) != string::npos ) {
				sLogbookRecipe recipeEntry;
				recipeEntry.recipe = &(*library)[i];
				s_recipe_CurrentList.push_back( recipeEntry );
			}
		}
	}
	else { 
		short listSize;
		auto library = p_recipe_Library->GetRecipeLibrary( listSize );
		for ( int i = 0; i < listSize; ++i )
		{
			sLogbookRecipe recipeEntry;
			recipeEntry.recipe = &(*library)[i];
			s_recipe_CurrentList.push_back( recipeEntry );
		}
	}
}

void CPlayerLogbook::UpdateRecipebook ( Vector2d vMousePos )
{
	// ==== Do all search list editing first ====
	// Typing bar
	if ( true || i_mouseOver_Offset == -1 ) {
		if ( CInput::Mouse(CInput::MBLeft) )
		{
			if ( Rect( 0.20f + offset.x, 0.08f + offset.y, 0.40f, 0.045f ).Contains( vMousePos ) )
			{
				mControl->Capture();
				i_logbookSubstate = 1;
			}
			else
			{
				mControl->Release();
				i_logbookSubstate = 0;
			}
		}
	}
	if ( i_logbookSubstate == 1 ) {
		// Get typing input
		bool performSearch = false;
		//s_recipe_SearchParams += CInput::GetTypeChar();
		if ( s_recipe_SearchParams.size() < 33 )
		{
				unsigned char i = Input::GetTypeChar(); // Get typed character
				if (( i != Keys.Return )&&( i != 0 )) {
					if ( i == '\n' || i == '\t' || i == ' ' ) { // don't start with spaces
						if ( s_recipe_SearchParams.size() != 0 ) {
							s_recipe_SearchParams += i;
						}
					}
					else {
						s_recipe_SearchParams += i;
					}
					performSearch = true;
				}
				else if ( i == Keys.Return ) {
					i_logbookSubstate = 0;
					performSearch = true;
				}
		}
		if ( Input::Keydown( Keys.Backspace ) ) // Delete char if backspace
		{
			if ( s_recipe_SearchParams.size() > 0 ) {
				s_recipe_SearchParams.resize( s_recipe_SearchParams.size()-1 );
				performSearch = true;
			}
		}
		// Recreate the list of recipies based on the search
		if ( performSearch ) {
			SearchRecipies( s_recipe_SearchParams, s_recipe_CurrentList );
		}
	}

	// ==== Do all search displaying next ====
	// Get selection mouseover
	i_mouseOver_Offset = -1;
	if (( vMousePos.x > 0.05f+offset.x )&&( vMousePos.x < 0.50f+offset.x ))
	{
		i_mouseOver_Offset = (int)floorf((vMousePos.y-(0.13f+offset.y))/0.04f);
		if ( vMousePos.y > offset.y + 0.13f + i_mouseOver_Offset*0.04f + 0.035f ) {
			i_mouseOver_Offset = -1;
		}
		if ( i_mouseOver_Offset >= std::min<int>( i_recipe_ScrollSize, (int)s_recipe_CurrentList.size()-i_recipe_ScrollOffset ) ) {
			i_mouseOver_Offset = -1;
		}
		if ( i_mouseOver_Offset < 0 ) {
			i_mouseOver_Offset = -1;
		}
	}
	// If left clicked, figure out where to go to
	if ( i_mouseOver_Offset != -1 )
	{
		if (( CInput::MouseDown(CInput::MBLeft) )/*&&( !b_recipe_InSubjectView )*/)
		{
			// If the offset is in range
			if ( i_mouseOver_Offset+i_recipe_ScrollOffset < (signed)(s_recipe_CurrentList.size()) )
			{
				int iTargetGroup = i_mouseOver_Offset+i_recipe_ScrollOffset;
				s_recipe_CurrentSubject = s_recipe_CurrentList[iTargetGroup].recipe->name;
				s_recipe_CurrentEntry	= s_recipe_CurrentList[iTargetGroup];
				b_recipe_InSubjectView = true;

				// Create the recipe objects
				while ( !p_recipe_CurrentEntryItems.empty() ) {
					if ( p_recipe_CurrentEntryItems.back() ) {
						delete p_recipe_CurrentEntryItems.back();
					}
					p_recipe_CurrentEntryItems.pop_back();
				}
				for ( uint recipeSlot = 0; recipeSlot < s_recipe_CurrentEntry.recipe->slot.size(); ++recipeSlot )
				{
					if ( s_recipe_CurrentEntry.recipe->slot[recipeSlot].id != 0 ) {
						p_recipe_CurrentEntryItems.push_back( CWeaponItem::Instantiate(s_recipe_CurrentEntry.recipe->slot[recipeSlot].id) );
						p_recipe_CurrentEntryItems.back()->SetHoldState( Item::SystemHidden );

						if ( s_recipe_CurrentEntry.recipe->slot[recipeSlot].id == 2 ) {
							((ItemTerraBlok*)p_recipe_CurrentEntryItems.back())->SetType( s_recipe_CurrentEntry.recipe->slot[recipeSlot].subid );
						}
					}
					else {
						p_recipe_CurrentEntryItems.push_back( NULL );
					}
				}
				// And push back the result object
				p_recipe_CurrentEntryItems.push_back( CWeaponItem::Instantiate(s_recipe_CurrentEntry.recipe->result.id) );
				p_recipe_CurrentEntryItems.back()->SetHoldState( Item::SystemHidden );
				if ( s_recipe_CurrentEntry.recipe->result.id == 2 ) {
					((ItemTerraBlok*)p_recipe_CurrentEntryItems.back())->SetType( s_recipe_CurrentEntry.recipe->result.subid );
				}

			}
		}
	}
	if ( CInput::MouseDown(CInput::MBRight) ) 
	{
		if ( (vMousePos.x > offset.x) && (vMousePos.x < offset.x + 0.65f)
			&& (vMousePos.y > offset.y) && (vMousePos.y < offset.y + 0.8f) )
		{
			// Go up a section if right-clicked
			b_recipe_InSubjectView = false;
		}
	}
	// Scroll bar code
	if (( vMousePos.x > 0.565f+offset.x )&&( vMousePos.x < 0.565f+0.035f+offset.x ))
	{
		bIsBusyHolding = false;
		if ( CInput::Mouse(CInput::MBLeft) )
		{
			ftype tempY;
			//tempY = (vMousePos.y - (0.13f + offset.y))/(0.04f*i_recipe_ScrollSize - 0.005f);
			tempY = (vMousePos.y - (0.13f + offset.y + 0.01f))/(0.04f*i_recipe_ScrollSize - 0.005f -0.02f);
			//tempY = (tempY-0.05f)/0.95f;
			if ( tempY > 0 && tempY < 1 ) {
				bIsBusyHolding = true;
				f_recipe_ScrollBarPos = tempY;
			}
		}
	}
	// Limit result regardless of where we are
	i_recipe_ScrollOffset = std::max<int>(0,(int)(f_recipe_ScrollBarPos * (s_recipe_CurrentList.size()-i_recipe_ScrollSize+1)) );


	// Get mouse over pos
	//if ( b_recipe_InSubjectView )
	{
		i_recipe_MouseOverX = (int)((vMousePos.x-(v_recipe_DrawPos.x+offset.x))/0.07f);
		if ( vMousePos.x > v_recipe_DrawPos.x + offset.x + 0.07f*i_recipe_MouseOverX + 0.06f )
			i_recipe_MouseOverX = -1;

		i_recipe_MouseOverY = (int)((vMousePos.y-(v_recipe_DrawPos.y+offset.y))/0.07f);
		if ( vMousePos.y > v_recipe_DrawPos.y + offset.y + 0.07f*i_recipe_MouseOverY + 0.06f )
			i_recipe_MouseOverY = -1;
	}
}



bool CPlayerLogbook::Render ( const char pass )
{
	if ( pass != 0 )
		return false;

	GL_ACCESS GLd_ACCESS

	// Do rendering here
	GL.beginOrtho();
	GLd.DrawSet2DScaleMode( GLd.SCALE_HEIGHT );	// scale with height
		matDrawer->setTexture( 0, texNull );
		matDrawer->bindPass(0);

		//ftype xOffset = fLogbookOffset;

		GLd.DrawSet2DMode( GLd.D2D_FLAT );

		// Draw logbook mode tabs
		for ( int i = 0; i < 3; ++i ) {
			if ( i == iLogbookMode )
				GLd.P_PushColor( 0.8f * 0.9f, 0.75f * 0.9f, 0.6f * 0.9f, 1.0f );
			else if ( i == i_mouseOver_tabSelect )
				GLd.P_PushColor( 0.8f * 0.8f, 0.75f * 0.8f, 0.6f * 0.8f, 1.0f );
			else
				GLd.P_PushColor( 0.8f * 0.6f, 0.75f * 0.6f, 0.6f * 0.6f, 1.0f );

			GLd.DrawRectangleA( 0.65f + offset.x, 0.05f + offset.y + i*0.1f, 0.06f,0.08f );
		}

		// Draw Logbook background
		GLd.P_PushColor( 0.8f, 0.75f, 0.6f, 1.0f );
		GLd.DrawRectangleA( 0 + offset.x, offset.y, 0.65f, 0.8f );

		if ( iLogbookMode == 0 ) { // Draw default logbook
			RenderLogbook ();
		}
		else if ( iLogbookMode == 1 ) { // Draw recipe book
			RenderRecipebook();
		}
		else if ( iLogbookMode == 2 ) { // player day log mode
			
		}
		
	GL.endOrtho();
	return true;
}

void CPlayerLogbook::RenderLogbook ( void )
{
	/*if ( !b_holder_InSubjectView )
	{
		int currentOffset = 0;

		// Draw the rectangle for each entry
		glColor4f( 0.9f, 0.85f, 0.7f, 1.0f );
		for ( unsigned int i = 0; i < p_holder_CurrentGroup->entries.size(); ++i )
		{
			GL.DrawRectangleA( 0.05f + xOffset, 0.18f + currentOffset*0.04f, 0.55f, 0.035f );

			if ( i_mouseOver_Offset == currentOffset )
			{
				glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
				GL.DrawSet2DMode( GL.D2D_WIRE );
				GL.DrawRectangleA( 0.05f + xOffset, 0.18f + currentOffset*0.04f, 0.55f, 0.035f );
				GL.DrawSet2DMode( GL.D2D_FLAT );
				glColor4f( 0.9f, 0.85f, 0.7f, 1.0f );
			}

			currentOffset += 1;
		}

		// Draw the reactangle for each subgroup
		glColor4f( 0.8f, 0.8f, 0.8f, 1.0f );
		for ( unsigned int i = 0; i < p_holder_CurrentGroup->subgroups.size(); ++i )
		{
			GL.DrawRectangleA( 0.05f + xOffset, 0.18f + currentOffset*0.04f, 0.55f, 0.035f );

			if ( i_mouseOver_Offset == currentOffset )
			{
				glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
				GL.DrawSet2DMode( GL.D2D_WIRE );
				GL.DrawRectangleA( 0.05f + xOffset, 0.18f + currentOffset*0.04f, 0.55f, 0.035f );
				GL.DrawSet2DMode( GL.D2D_FLAT );
				glColor4f( 0.8f, 0.8f, 0.8f, 1.0f );
			}

			currentOffset += 1;
		}
	}
			

	// Begin drawing text
	glColor4f( 0.1f, 0.1f, 0.1f, 1.0f );
	if ( !b_holder_InSubjectView )
	{
		fntPageTitle->Set();
			// Draw the current group name
			GL.DrawAutoText( 0.05f + xOffset,0.155f, p_holder_CurrentGroup->name.c_str() );
		fntPageTitle->Unbind();

		fntLogbookHelp->Set();
			int currentOffset = 0;
			// Draw the rectangle for each entry
			for ( unsigned int i = 0; i < p_holder_CurrentGroup->entries.size(); ++i )
			{
				GL.DrawAutoText( 0.10f + xOffset,0.18f + 0.03f + currentOffset*0.04f, p_holder_CurrentGroup->entries[i]->name.c_str() );
				currentOffset += 1;
			}
			// Draw the reactangle for each subgroup
			for ( unsigned int i = 0; i < p_holder_CurrentGroup->subgroups.size(); ++i )
			{
				GL.DrawAutoText( 0.10f + xOffset,0.18f + 0.03f + currentOffset*0.04f, p_holder_CurrentGroup->subgroups[i]->name.c_str() );
				currentOffset += 1;
			}
		fntLogbookHelp->Unbind();
	}
	else
	{
		fntPageTitle->Set();
			// Draw the current entry name
			//GL.DrawAutoText( 0.15f + xOffset,0.15f, p_holder_CurrentEntry->name.c_str() );
			GL.DrawAutoText( 0.05f + xOffset,0.155f, s_entry_title.c_str() );
		fntPageTitle->Unbind();
		fntPageText->Set();
			GL.DrawAutoTextWrapped( 0.05f + xOffset,0.20f,0.55f, s_entry_description.c_str() );
		fntPageText->Unbind();
	}*/
	GLd_ACCESS

	int currentOffset = 0;

	// Draw the rectangle for each entry
	GLd.P_PushColor( 0.9f, 0.85f, 0.7f, 1.0f );
	for ( unsigned int i = 0; i < s_subentries.size(); ++i )
	{
		GLd.DrawRectangleA( 0.05f + offset.x, 0.08f + offset.y + currentOffset*0.04f, 0.55f, 0.035f );
		if ( i_mouseOver_Offset == currentOffset )
		{
			GLd.P_PushColor( 1.0f, 1.0f, 1.0f, 1.0f );
			GLd.DrawSet2DMode( GLd.D2D_WIRE );
			GLd.DrawRectangleA( 0.05f + offset.x, 0.08f + offset.y + currentOffset*0.04f, 0.55f, 0.035f );
			GLd.DrawSet2DMode( GLd.D2D_FLAT );
			GLd.P_PushColor( 0.9f, 0.85f, 0.7f, 1.0f );
		}
		currentOffset += 1;
	}

	GLd.P_PushColor( 0.1f, 0.1f, 0.1f, 1.0f );
	fntPageTitle->Set();
		// Draw the current group name
		GLd.DrawAutoText( 0.05f + offset.x,0.055f + offset.y, s_entry_title.c_str() );
	fntPageTitle->Unbind();

	fntLogbookHelp->Set();
		// Draw the rectangle for each entry
		currentOffset = 0;
		for ( unsigned int i = 0; i < s_subentries.size(); ++i )
		{
			GLd.DrawAutoText( 0.10f + offset.x,0.08f + offset.y + 0.03f + currentOffset*0.04f, s_subentries[i].c_str() );
			currentOffset += 1;
		}
	fntLogbookHelp->Unbind();

	fntPageText->Set();
		GLd.DrawAutoTextWrapped( 0.05f + offset.x,0.10f + offset.y + currentOffset*0.041f,0.55f, s_entry_description.c_str() );
	fntPageText->Unbind();
}

void CPlayerLogbook::RenderRecipebook ( void )
{	GLd_ACCESS
	// Draw list of possible selections
	{
		int currentOffset = 0;

		// Draw the rectangle for each entry
		GLd.P_PushColor( 0.9f, 0.85f, 0.7f, 1.0f );
		//for ( unsigned int i = i_recipe_ScrollOffset; i < std::min<int>(s_recipe_CurrentList.size(),i_recipe_ScrollOffset+i_recipe_ScrollSize); ++i )
		//for ( unsigned int i = i_recipe_ScrollOffset; i < i_recipe_ScrollSize; ++i )
		for ( unsigned int i = i_recipe_ScrollOffset; i < std::min<int>(s_recipe_CurrentList.size(),i_recipe_ScrollOffset+i_recipe_ScrollSize); ++i )
		{
			GLd.DrawRectangleA( 0.05f + offset.x, 0.13f + offset.y + currentOffset*0.04f, 0.50f, 0.035f );

			if ( i_mouseOver_Offset == currentOffset )
			{
				GLd.P_PushColor( 1.0f, 1.0f, 1.0f, 1.0f );
				GLd.DrawSet2DMode( GLd.D2D_WIRE );
				GLd.DrawRectangleA( 0.05f + offset.x, 0.13f + offset.y + currentOffset*0.04f, 0.50f, 0.035f );
				GLd.DrawSet2DMode( GLd.D2D_FLAT );
				GLd.P_PushColor( 0.9f, 0.85f, 0.7f, 1.0f );
			}

			currentOffset += 1;
		}

		// Draw scroll bar on side
		//glColor4f( 0.9f, 0.85f, 0.7f, 1.0f );
		ftype scrollbarHeight = 0.04f*i_recipe_ScrollSize - 0.005f;
		GLd.DrawRectangleA( 0.565f + offset.x, 0.13f + offset.y, 0.035f, scrollbarHeight );

		//glColor4f( 0.9f, 0.85f, 0.7f, 1.0f );
		// Draw input box
		GLd.DrawRectangleA( 0.20f + offset.x, 0.08f + offset.y, 0.40f, 0.045f );
		if ( i_logbookSubstate == 1 ) {
			GLd.P_PushColor( 0.95f, 0.95f, 0.95f, 1.0f );
			GLd.DrawSet2DMode( GLd.D2D_WIRE );
			GLd.DrawRectangleA( 0.20f + offset.x, 0.08f + offset.y, 0.40f, 0.045f );
			GLd.DrawSet2DMode( GLd.D2D_FLAT );
		}
		// Draw top 5 buttons
		GLd.P_PushColor( 0.9f, 0.85f, 0.7f, 1.0f );
		for ( int i = 0; i < 5; ++i ) {
			GLd.DrawRectangleA( 0.05f + offset.x + 0.028f*i, 0.085f + offset.y, 0.025f, 0.035f );
		}

		// Draw scrollbar
		GLd.P_PushColor( 0.5f, 0.45f, 0.3f, 1.0f );
		GLd.DrawRectangleA( 0.565f + offset.x, 0.13f + offset.y + f_recipe_ScrollBarPos*(scrollbarHeight-0.04f), 0.035f, 0.04f );

	}
	if ( b_recipe_InSubjectView ) // A recipie is selected
	{
		// Draw the rectangle for each part
		GLd.P_PushColor( 0.9f, 0.85f, 0.7f, 1.0f );
		for ( int x = 0; x < s_recipe_CurrentEntry.recipe->width; ++x ) {
			for ( int y = 0; y < s_recipe_CurrentEntry.recipe->height; ++y ) {
				GLd.DrawRectangleA( v_recipe_DrawPos.x + offset.x + 0.07f*x, v_recipe_DrawPos.y + offset.y + 0.07f*y, 0.06f,0.06f );
			}
		}
		// Draw the result
		GLd.DrawRectangleA( 0.43f + offset.x, 0.53f + offset.y, 0.06f,0.06f );

		// Draw the item sprite for each part
		GLd.P_PushColor( 1.0f, 1.0f, 1.0f, 1.0f );
		for ( int x = 0; x < s_recipe_CurrentEntry.recipe->width; ++x ) {
			for ( int y = 0; y < s_recipe_CurrentEntry.recipe->height; ++y ) {
				if ( (*s_recipe_CurrentEntry.recipe)(x,y).id > 0 ) {
					CTexture* icon = p_recipe_CurrentEntryItems[x+y*s_recipe_CurrentEntry.recipe->width]->GetInventoryIcon();
					icon->Bind();
						GLd.DrawRectangleA( v_recipe_DrawPos.x + offset.x + 0.07f*x, v_recipe_DrawPos.y + offset.y + 0.07f*y, 0.06f,0.06f );
					icon->Unbind();
				}
			}
		}
		// Draw the result
		CTexture* icon = p_recipe_CurrentEntryItems.back()->GetInventoryIcon();
		icon->Bind();
			GLd.DrawRectangleA( 0.43f + offset.x, 0.53f + offset.y, 0.06f,0.06f );
		icon->Unbind();

	}

	// Begin drawing text
	GLd.P_PushColor( 0.1f, 0.1f, 0.1f, 1.0f );
	//if ( !b_recipe_InSubjectView )
	{
		fntPageTitle->Set();
			// Draw the current group name
			GLd.DrawAutoText( 0.05f + offset.x,0.055f + offset.y, "Crafting Recipes" );
		fntPageTitle->Unbind();

		fntLogbookHelp->Set();
			int currentOffset = 0;
			// Draw the rectangle for each entry
			for ( unsigned int i = i_recipe_ScrollOffset; i < std::min<int>(s_recipe_CurrentList.size(),i_recipe_ScrollOffset+i_recipe_ScrollSize); ++i )
			{
				GLd.DrawAutoText( 0.10f + offset.x,0.13f + offset.y + 0.03f + currentOffset*0.04f, s_recipe_CurrentList[i].recipe->name.c_str() );
				currentOffset += 1;
			}
			// Draw the search parameter
			GLd.DrawAutoText( 0.22f + offset.x, 0.08f + offset.y + 0.04f, s_recipe_SearchParams.c_str() );
		fntLogbookHelp->Unbind();
	}
	if ( b_recipe_InSubjectView )
	{
		fntPageTitle->Set();
			// Draw the current entry name
			GLd.DrawAutoText( 0.05f + offset.x,0.055f + 0.44f + offset.y, s_recipe_CurrentSubject.c_str() );
		fntPageTitle->Unbind();

		// Draw the recipe
		fntPageText->Set();
		// Draw stack size of item mouse over
		for ( int x = 0; x < s_recipe_CurrentEntry.recipe->width; ++x ) {
			for ( int y = 0; y < s_recipe_CurrentEntry.recipe->height; ++y ) {
				//GLd.DrawRectangleA( 0.05f + xOffset + 0.07f*x, 0.22f + 0.07f*y, 0.06f,0.06f );
				if ( (*(s_recipe_CurrentEntry.recipe))(x,y).id > 0 ) {
					// Draw stack size
					if ( (*(s_recipe_CurrentEntry.recipe))(x,y).stack > 1 ) {
						GLd.DrawAutoText( v_recipe_DrawPos.x + offset.x + 0.07f*x, v_recipe_DrawPos.y + offset.y + 0.07f*y + 0.06f, "%d", (*(s_recipe_CurrentEntry.recipe))(x,y).stack );
					}
					// Draw name if mouse over
					if (( i_recipe_MouseOverX == x )&&( i_recipe_MouseOverY == y )) {
						//GLd.DrawAutoText( 0.05f + xOffset + 0.07f*x, 0.22f + 0.07f*y, CWeaponItem::ItemName((*(s_recipe_CurrentEntry.recipe))(x,y).id).c_str() );
						GLd.DrawAutoText( v_recipe_DrawPos.x + offset.x + 0.07f*x, v_recipe_DrawPos.y + offset.y + 0.07f*y, p_recipe_CurrentEntryItems[x+y*s_recipe_CurrentEntry.recipe->width]->GetItemName() );
					}
				}
				else {
					if (( i_recipe_MouseOverX == x )&&( i_recipe_MouseOverY == y )) {
						GLd.DrawAutoText( v_recipe_DrawPos.x + offset.x + 0.07f*x, v_recipe_DrawPos.y + offset.y + 0.07f*y, "Empty slot" );
					}
				}
			}
		}
		// Draw the result
		GLd.DrawAutoText( 0.43f + offset.x,0.64f + offset.y, "%d %s", s_recipe_CurrentEntry.recipe->result.stack, p_recipe_CurrentEntryItems.back()->GetItemName() );
		fntPageText->Unbind();

	}
}

void CPlayerLogbook::UpdateHolderPosition ( const bool bClickedSubject )
{
	b_holder_InSubjectView = bClickedSubject;

	// First, we grab the recursion amount by counting the dots
	int targetDepth = 0;
	for ( unsigned int i = 0; i < s_holder_CurrentSubject.size(); ++i )
	{
		if ( s_holder_CurrentSubject[i] == '.' )
			targetDepth += 1;
	}
	// Now, we recurse that many times, looking for a section each recurse
	sLogbookGroup* currentGroup = &root_logbook; 

	string currentSubentry;
	int currentChar = 0;
	for ( int step = 0; step < targetDepth; ++step )
	{
		// First, grab the section name (add characters to a string until hit a dot)
		currentSubentry = "";
		while ( s_holder_CurrentSubject[currentChar] != '.' )
		{
			currentSubentry += s_holder_CurrentSubject[currentChar];
			++currentChar;
		}
		++currentChar;
		//cout << "  -" << currentSubentry << endl;

		// At current depth, look for the next section
		for ( std::vector<sLogbookGroup*>::iterator it = currentGroup->subgroups.begin(); it != currentGroup->subgroups.end(); ++it )
		{
			if ( (*it)->name == currentSubentry )
			{
				currentGroup = *it;
				break;
			}
		}
	}

	// For the last entry, choose either an entry or a group based on the holder type
	// First, grab the section name (add characters to a string until hit a dot)
	currentSubentry = "";
	while ( currentChar < (signed)s_holder_CurrentSubject.size() )
	{
		currentSubentry += s_holder_CurrentSubject[currentChar];
		++currentChar;
	}

	if ( !b_holder_InSubjectView )
	{
		// First group type
		
		// At current depth, look for the section
		for ( std::vector<sLogbookGroup*>::iterator it = currentGroup->subgroups.begin(); it != currentGroup->subgroups.end(); ++it )
		{
			if ( (*it)->name == currentSubentry )
			{
				currentGroup = *it;
				break;
			}
		}
		p_holder_CurrentGroup = currentGroup;

		cout << "Currently looking at group: " << p_holder_CurrentGroup->name << endl;
	}
	else
	{
		// Next, entry type
		p_holder_CurrentGroup = currentGroup;

		// At current depth, look for the entry
		for ( std::vector<sLogbookEntry*>::iterator it = currentGroup->entries.begin(); it != currentGroup->entries.end(); ++it )
		{
			if ( (*it)->name == currentSubentry )
			{
				p_holder_CurrentEntry = *it;
				break;
			}
		}

		cout << "Currently looking at entry: " << p_holder_CurrentEntry->name << endl;
	}
}

void CPlayerLogbook::UpdatePageInfo ( void )
{
	using std::ifstream;
	if ( p_holder_CurrentEntry )
	{
		// Load file
		ifstream inputFile ( p_holder_CurrentEntry->filename.c_str() );

		// Parse the data
		YAML::Parser ymlParser ( inputFile );

		YAML::Node ymlDocument;
		if ( ymlParser.GetNextDocument( ymlDocument ) )
		{
			const YAML::Node*		pCurrentNode;
			
			pCurrentNode = ymlDocument.FindValue( "title" );
			s_entry_title = "";
			if ( pCurrentNode )
				*pCurrentNode >> s_entry_title;

			pCurrentNode = ymlDocument.FindValue( "default" );
			s_entry_description = "";
			if ( pCurrentNode )
				*pCurrentNode >> s_entry_description;
		}

		// Explcitly close the file
		inputFile.close();
	}
}