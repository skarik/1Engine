
// Include stuph
#include "../CDuskGUI.h"
#include "CDuskGUIDropdownList.h"

// A dropdown list
CDuskGUI::Handle CDuskGUI::CreateDropdownList ( const Handle & parent )
{
	vElements.push_back( new CDuskGUIDropdownList() );
	vElements.back()->parent = parent;
	return Handle(vElements.size()-1);
}
void CDuskGUI::AddDropdownOption ( const Handle& handle, const string& option, const int value )
{
	CDuskGUIDropdownList* ddl = (CDuskGUIDropdownList*)(vElements[int(handle)]);
	CDuskGUIDropdownList::ListElement_t p;
	p.str = option;
	p.value = value;
	ddl->optionList.push_back(p);
}
void CDuskGUI::SetDropdownValue ( const Handle& handle, const int value )
{
	CDuskGUIDropdownList* ddl = (CDuskGUIDropdownList*)(vElements[int(handle)]);
	for ( uint i = 0; i < ddl->optionList.size(); ++i )
	{
		if ( ddl->optionList[i].value == value )
		{
			ddl->selection = i;
		}
	}
}
// Get dropdown value
int CDuskGUI::GetDropdownOption ( const Handle & handle )
{
	CDuskGUIDropdownList* ddl = (CDuskGUIDropdownList*)(vElements[int(handle)]);
	if ( ddl->optionList.size() > 0 ) {
		return ddl->optionList[ddl->selection].value;
	}
	else {
		return -1;
	}
}
// Set dropdown value
void CDuskGUI::SetDropdownOption ( const Handle & handle, const int val )
{
	CDuskGUIDropdownList* ddl = (CDuskGUIDropdownList*)(vElements[int(handle)]);
	ddl->selection = val;
}
// Clear dropdown list
void CDuskGUI::ClearDropdownList ( const Handle & handle )
{
	CDuskGUIDropdownList* ddl = (CDuskGUIDropdownList*)(vElements[int(handle)]);
	ddl->optionList.clear();
	ddl->selection = 0;
}

void CDuskGUIDropdownList::Update ( void )
{
	CDuskGUIButton::Update();

	if ( (!visible) || (!drawn) )
		return;

	if ( !inDialogueMode )
	{
		// Button state
		homeRect = rect;
		if ( isReady && isPressed )
		{
			inDialogueMode = true;
			activeGUI->hCurrentDialogue = activeGUI->hCurrentElement;

			rect.size.y += rect.size.y * 0.95f * optionList.size();

			oldSelection = selection;
		}

		if ( selection < (int)optionList.size() )
		{
			label = optionList[selection].str;
		}

		if ( !isReady ) {
			if ( CInput::MouseUp(CInput::MBLeft) || CInput::Mouse(CInput::MBRight) ) {
				isReady = true;
			}
		}
	}
	else
	{
		// Selection picking state
		activeGUI->hCurrentDialogue = activeGUI->hCurrentElement;
		// Grab screen actually using
		Rect screen = activeGUI->GetScreenRect();
		
		if ( mouseIn )
		{
			// find selection
			int nselection = (int)floor((cursor_pos.y - rect.pos.y - activeGUI->parenting_offset.y - homeRect.size.y)/(homeRect.size.y * 0.95f));
			if ( nselection > (int)optionList.size() ) {
				nselection = optionList.size()-1;
			}
			else if ( nselection < 0 ) {
				nselection = 0;
			}
			else {
				selection = nselection;
			}

			// if left clicked, make selection
			if ( CInput::Mouse(CInput::MBLeft) )
			{
				// go to normalfag mode
				isReady = false; //body is not ready
				inDialogueMode = false;
				activeGUI->hCurrentDialogue = Handle(-1);
				rect = homeRect;
			}

		}
		else
		{
			if ( CInput::MouseDown(CInput::MBLeft) )
			{
				isReady = false;
				inDialogueMode = false;
				activeGUI->hCurrentDialogue = Handle(-1);
				rect = homeRect;
			}
			else if ( CInput::MouseDown(CInput::MBRight) )
			{
				isReady = false;
				inDialogueMode = false;
				activeGUI->hCurrentDialogue = Handle(-1);
				rect = homeRect;
				selection = oldSelection;
			}
		}
	}

}


void CDuskGUIDropdownList::Render ( void )
{
	// Material binding
	if ( !inDialogueMode )
	{
		if ( mouseIn )
		{
			if ( beginPress )
				setDrawDown();
			else
				setDrawHover();
		}
		else {
			setDrawDefault();
		}
	}
	else {
		setDrawDefault();
	}

	// Grab screen actually using
	Rect screen = activeGUI->GetScreenRect();

	//	Draw boxes:
	// Selection box
	drawRectWire( rect );
	if ( !inDialogueMode ) {
		setSubdrawDefault();
	}
	else {
		setSubdrawOpaque();
	}
	drawRect( rect );
	// Dropdown box
	if ( inDialogueMode )
	{
		setSubdrawDarkSelection();
		setSubdrawOpaque();
		drawRect( Rect(
			rect.pos.x,
			(homeRect.pos.y + homeRect.size.y*(1+0.95f*selection)),
			rect.size.x,
			homeRect.size.y*0.95f )
			);
	}

	//	Draw text:
	// Draw the selection
	drawText( rect.pos.x + rect.size.x*0.06f, rect.pos.y + homeRect.size.y*0.1f + 0.02f*screen.size.y, label.c_str() );
	// Draw the choices
	if ( inDialogueMode )
	{
		for ( unsigned int i = 0; i < optionList.size(); ++i )
		{
			drawText( rect.pos.x + rect.size.x*0.06f, rect.pos.y + homeRect.size.y*1.1f + homeRect.size.y*0.95f*i + 0.022F*screen.size.y, optionList[i].str.c_str() );
		}
	}
}