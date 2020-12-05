#include "Element.h"
#include "engine-common/dusk/UI.h"

// Static variable declaration
//Vector2f CDuskGUIElement::cursor_pos = Vector2f();
//CDuskGUI* CDuskGUIElement::activeGUI = NULL;

// Update
// Base code that simply checks for mouse in rect
void dusk::Element::Update ( const UIStepInfo* stepinfo )
{
	// TODO: move this to a common function
	Vector2f offset_cursor = stepinfo->mouse_position;
	if (m_visible && m_absoluteRect.Contains(offset_cursor))
	{
		m_isMouseIn = true;
	}
	else
	{
		m_isMouseIn = false;
	}
}

dusk::Element::~Element ( void )
{
	// Destructor removes self from the DuskUI's list
	m_interface->RemoveElement(m_index);
}

//
//void CDuskGUIElement::setDrawDown ( void )
//{
//	activeGUI->setDrawDown();
//}
//void CDuskGUIElement::setDrawHover ( void )
//{
//	activeGUI->setDrawHover();
//}
//void CDuskGUIElement::setDrawDefault ( void )
//{
//	activeGUI->setDrawDefault();
//}
//
//// These two cannot mix
//void CDuskGUIElement::setSubdrawSelection ( void )
//{
//	activeGUI->setSubdrawSelection();
//}
//void CDuskGUIElement::setSubdrawDarkSelection ( void )
//{
//	activeGUI->setSubdrawDarkSelection();
//}
//// These two cannot mix
//void CDuskGUIElement::setSubdrawTransparent ( void )
//{
//	activeGUI->setSubdrawTransparent();
//}
//void CDuskGUIElement::setSubdrawOpaque ( void )
//{
//	activeGUI->setSubdrawOpaque();
//}
//// These two cannot mix
//void CDuskGUIElement::setSubdrawNotice ( void )
//{
//	activeGUI->setSubdrawNotice();
//}
//void CDuskGUIElement::setSubdrawError ( void )
//{
//	activeGUI->setSubdrawError();
//}
//// Special states
//void CDuskGUIElement::setSubdrawPulse ( void )
//{
//	activeGUI->setSubdrawPulse();
//}
//void CDuskGUIElement::setSubdrawOverrideColor ( const Color& color )
//{
//	activeGUI->setSubdrawOverrideColor(color);
//}
//// Reset states
//void CDuskGUIElement::setSubdrawDefault ( void )
//{
//	activeGUI->setSubdrawDefault();
//}
//
//void CDuskGUIElement::drawRect ( const Rect& rect )
//{
//	activeGUI->drawRect(rect);
//}
//void CDuskGUIElement::drawRectWire ( const Rect& rect )
//{
//	// if has focus, draws at a bright color.
//	// other draws at a dark color. automatically sets.
//	activeGUI->drawRectWire(rect,hasFocus);
//}
//void CDuskGUIElement::drawLine ( const Real x1, const Real y1, const Real x2, const Real y2 )
//{
//	activeGUI->drawLine(x1,y1,x2,y2);
//}
//
//void CDuskGUIElement::drawText ( const Real x, const Real y, const char* str )
//{
//	activeGUI->drawText(x,y,str);
//}
//void CDuskGUIElement::drawTextWidth ( const Real x, const Real y, const Real w, const char* str )
//{
//	activeGUI->drawTextWidth(x,y,w,str);
//}
//void CDuskGUIElement::drawTextCentered ( const Real x, const Real y, const char* str )
//{
//	activeGUI->drawTextCentered(x,y,str);
//}
//
//rrTextBuilder2D* CDuskGUIElement::getMeshBuilder ( void )
//{
//	return activeGUI->m_builder;
//}