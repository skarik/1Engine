
#include "../CDuskGUI.h"
#include "CDuskGUIListview.h"

// A listview
CDuskGUI::Handle CDuskGUI::CreateListview ( const Handle & parent )
{
	vElements.push_back( new CDuskGUIListview() );
	vElements.back()->parent = parent;
	//SetListviewSelection( Handle(vElements.size()-1), -1 );
	return Handle(vElements.size()-1);
}
void CDuskGUI::AddListviewOption ( const Handle& handle, const string& option, const int value )
{
	CDuskGUIListview* ddl = (CDuskGUIListview*)(vElements[int(handle)]);
	CDuskGUIListview::ListElement_t p;
	p.str = option;
	p.value = value;
	ddl->optionList.push_back(p);
}
void CDuskGUI::ClearListview ( const Handle& handle )
{
	CDuskGUIListview* ddl = (CDuskGUIListview*)(vElements[int(handle)]);
	ddl->optionList.clear();
}
int  CDuskGUI::GetListviewCount ( const Handle& handle )
{
	CDuskGUIListview* ddl = (CDuskGUIListview*)(vElements[int(handle)]);
	return (int)ddl->optionList.size();
}
void CDuskGUI::SetListviewFieldHeight ( const Handle& handle, const ftype newheight )
{
	CDuskGUIListview* ddl = (CDuskGUIListview*)(vElements[int(handle)]);
	ddl->field_height = newheight;
}
// Get listview selection
int CDuskGUI::GetListviewSelection ( const Handle & handle )
{
	CDuskGUIListview* ddl = (CDuskGUIListview*)(vElements[int(handle)]);
	if ( ddl->selection < 0 ) {
		return -1;
	}
	if ( ddl->optionList.size() > 0 ) {
		if ( ddl->selection < ddl->optionList.size() ) {
			return ddl->optionList[ddl->selection].value;
		}
		else {
			ddl->selection = -1;
			return -1;
		}
	}
	else {
		return 0;
	}
}
// Get list view actual select
int CDuskGUI::GetListviewIndex ( const Handle & handle )
{
	CDuskGUIListview* ddl = (CDuskGUIListview*)(vElements[int(handle)]);
	if ( ddl->selection < 0 ) {
		return -1;
	}
	if ( ddl->optionList.size() > 0 ) {
		if ( ddl->selection < ddl->optionList.size() ) {
			return ddl->selection;
		}
		else {
			ddl->selection = -1;
			return -1;
		}
	}
	else {
		return 0;
	}
}
// Set listview selection
void CDuskGUI::SetListviewSelection ( const Handle & handle, const int val )
{
	CDuskGUIListview* ddl = (CDuskGUIListview*)(vElements[int(handle)]);
	ddl->selection = val;
}


void CDuskGUIListview::Update ( void )
{
	CDuskGUIElement::Update();

	if ( !mouseIn || !drawn ) {
		currentmouseover = -1;
	}
	else {
		int nselection = (int)floor( (cursor_pos.y - rect.pos.y)/field_height );
		if ( nselection < 0 ) {
			nselection = -1;
		}
		else if ( nselection >= (int)optionList.size() ) {
			nselection = -1;
		}

		if ( nselection != -1 ) {
			currentmouseover = nselection;
			if ( CInput::MouseDown(CInput::MBLeft) )
			{
				selection = currentmouseover;
			}
		}
	}

	if ( selection >= (int)optionList.size() ) {
		selection = -1;
	}
}

void CDuskGUIListview::Render ( void )
{
	CDuskGUIPanel::Render();

	setDrawDefault();

	// Draw the boxes around options
	for ( unsigned int i = 0; i < optionList.size(); ++i )
	{
		drawRectWire( Rect( rect.pos.x + rect.size.x*0.025f, rect.pos.y + field_height*i + 0.005f, rect.size.x*0.95f, field_height - 0.01f ) );
	}

	// Draw hover and selection
	if ( currentmouseover != -1 ) {
		setDrawHover();
		drawRect( Rect( rect.pos.x + rect.size.x*0.025f, rect.pos.y + field_height*currentmouseover + 0.005f, rect.size.x*0.95f, field_height - 0.01f ) );
	}

	if ( selection != -1 ) {
		setDrawDown();
		drawRect( Rect( rect.pos.x + rect.size.x*0.025f, rect.pos.y + field_height*selection + 0.005f, rect.size.x*0.95f, field_height - 0.01f ) );
	}

	// Now draw text
	for ( unsigned int i = 0; i < optionList.size(); ++i )
	{
		drawText( rect.pos.x + rect.size.x*0.1f, rect.pos.y + field_height*(i+0.2f) + 0.02f, optionList[i].str.c_str() );
	}

}