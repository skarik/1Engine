
#include "../CDuskGUI.h"
#include "CDuskGUIDropdownList_GO.h"

#include "engine/state/CGameState.h"

CDuskGUI::Handle CDuskGUI::CreateDropdownGameobjectList ( const Handle& parent )
{
	vElements.push_back( new CDuskGUIDropdownList_GO() );
	vElements.back()->parent = parent;
	return Handle(vElements.size()-1);
}
void CDuskGUI::RefreshGameobjectDropdownMenu ( const Handle& handle )
{
	CDuskGUIDropdownList_GO* ddl = (CDuskGUIDropdownList_GO*)(vElements[int(handle)]);
	ddl->RepopulateList();
}
void CDuskGUI::SetGameobjectDropdownFilter ( const Handle& handle, const string& filter )
{
	CDuskGUIDropdownList_GO* ddl = (CDuskGUIDropdownList_GO*)(vElements[int(handle)]);
	ddl->typenameFilter = filter;
}

void CDuskGUIDropdownList_GO::Update ( void )
{
	CDuskGUIDropdownList::Update();
}

void CDuskGUIDropdownList_GO::RepopulateList ( void )
{
	// Clear old values
	optionList.clear();

	// Need to get a list of all objects with the given filter
	std::vector<CGameBehavior*> filterResults;
	CGameState::Active()->FindObjectsWithTypename( typenameFilter, filterResults );

	// Fill dropdown list
	ListElement_t nullchoice;
	nullchoice.str = "None";
	nullchoice.value = -2;
	optionList.push_back( nullchoice );
	for ( uint i = 0; i < filterResults.size(); ++i ) {
		ListElement_t p;
		p.str = filterResults[i]->name;
		p.value = filterResults[i]->GetId();
		optionList.push_back(p);
	}
}