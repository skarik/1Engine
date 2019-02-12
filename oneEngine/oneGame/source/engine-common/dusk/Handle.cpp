#include "engine-common/dusk/Dusk.h"

dusk::Element* dusk::Handle::updateElement( dusk::UserInterface* ui )
{
	m_element = NULL;

	if (ui != NULL)
	{
		m_interface = ui;
	}
	if (m_interface != NULL)
	{
		if (m_index >= 0 && m_index < (int)m_interface->ElementList().size())
		{
			m_element = m_interface->ElementList()[m_index];
		}
	}
	return m_element;
}


dusk::Element* dusk::Handle::operator*()
{
	updateElement(NULL);
	if (m_element != NULL)
	{
		return m_element;
	}
	else
	{
		throw core::NullReferenceException();
		return NULL;
	}
}
dusk::Element* dusk::Handle::operator->()
{
	updateElement(NULL);
	if (m_element != NULL)
	{
		return m_element;
	}
	else
	{
		throw core::NullReferenceException();
		return NULL;
	}
}

//void	Dusk::Handle::SetVisible ( const bool visibility )
//{
//	activeGUI->SetElementVisible( *this, visibility );
//}
//void	Dusk::Handle::SetRect ( const Rect& rect )
//{
//	activeGUI->SetElementRect( *this, rect );
//}
//void	Dusk::Handle::SetText ( const string& str )
//{
//	activeGUI->SetElementText( *this, str );
//}
//void	Dusk::Handle::SetParent ( const Handle& parent )
//{
//	activeGUI->SetElementParent( *this, parent );
//}
//void	Dusk::Handle::ToggleVisibility ( void )
//{
//	activeGUI->ToggleElementVisible( *this );
//}
//
//Rect	Dusk::Handle::GetRect ( void )
//{
//	//vElements[int(handle)]->rect = rect;
//	//return activeGUI->GetEleme
//	return activeGUI->vElements[int(*this)]->rect;
//}
//bool	Dusk::Handle::GetClicked ( void )
//{
//	return activeGUI->GetClicked( *this );
//}
//bool	Dusk::Handle::GetMouseOver ( void )
//{
//	return activeGUI->GetMouseOver( *this );
//}
//bool	Dusk::Handle::GetButtonClicked ( void )
//{
//	return activeGUI->GetButtonClicked( *this );
//}
//int		Dusk::Handle::GetDialogueResponse ( void )
//{
//	return activeGUI->GetDialogueResponse( *this );
//}