
#include "CDuskGUI.h"
#include "CDuskGUIHandle.h"

CDuskGUI* Dusk::activeGUI = NULL;

CDuskGUIElement* Dusk::Handle::operator*() const
{
	if (index >= 0 && index < activeGUI->vElements.size())
	{
		return activeGUI->vElements[index];
	}
	return NULL;
}
CDuskGUIElement* Dusk::Handle::operator->() const
{
	if (index >= 0 && index < activeGUI->vElements.size())
	{
		return activeGUI->vElements[index];
	}
	return NULL;
}

void	Dusk::Handle::SetVisible ( const bool visibility )
{
	activeGUI->SetElementVisible( *this, visibility );
}
void	Dusk::Handle::SetRect ( const Rect& rect )
{
	activeGUI->SetElementRect( *this, rect );
}
void	Dusk::Handle::SetText ( const string& str )
{
	activeGUI->SetElementText( *this, str );
}
void	Dusk::Handle::SetParent ( const Handle& parent )
{
	activeGUI->SetElementParent( *this, parent );
}
void	Dusk::Handle::ToggleVisibility ( void )
{
	activeGUI->ToggleElementVisible( *this );
}
bool	Dusk::Handle::GetClicked ( void )
{
	return activeGUI->GetClicked( *this );
}
bool	Dusk::Handle::GetMouseOver ( void )
{
	return activeGUI->GetMouseOver( *this );
}
bool	Dusk::Handle::GetButtonClicked ( void )
{
	return activeGUI->GetButtonClicked( *this );
}
int		Dusk::Handle::GetDialogueResponse ( void )
{
	return activeGUI->GetDialogueResponse( *this );
}