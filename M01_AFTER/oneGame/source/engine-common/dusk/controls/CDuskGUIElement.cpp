
// Includes
#include "../CDuskGUI.h"
#include "CDuskGUIElement.h"

// Static variable declaration
Vector2d CDuskGUIElement::cursor_pos = Vector2d();
CDuskGUI* CDuskGUIElement::activeGUI = NULL;


// Update
// Base code that simply checks for mouse in rect
void CDuskGUIElement::Update ( void )
{
	if (( cursor_pos.x > rect.pos.x )&&
		( cursor_pos.y > rect.pos.y )&&
		( cursor_pos.x < rect.pos.x+rect.size.x )&&
		( cursor_pos.y < rect.pos.y+rect.size.y )&&
		visible)
	{
		mouseIn = true;
	}
	else
	{
		mouseIn = false;
	}
}


void CDuskGUIElement::setDrawDown ( void )
{
	activeGUI->setDrawDown();
}
void CDuskGUIElement::setDrawHover ( void )
{
	activeGUI->setDrawHover();
}
void CDuskGUIElement::setDrawDefault ( void )
{
	activeGUI->setDrawDefault();
}

// These two cannot mix
void CDuskGUIElement::setSubdrawSelection ( void )
{
	activeGUI->setSubdrawSelection();
}
void CDuskGUIElement::setSubdrawDarkSelection ( void )
{
	activeGUI->setSubdrawDarkSelection();
}
// These two cannot mix
void CDuskGUIElement::setSubdrawTransparent ( void )
{
	activeGUI->setSubdrawTransparent();
}
void CDuskGUIElement::setSubdrawOpaque ( void )
{
	activeGUI->setSubdrawOpaque();
}
// These two cannot mix
void CDuskGUIElement::setSubdrawNotice ( void )
{
	activeGUI->setSubdrawNotice();
}
void CDuskGUIElement::setSubdrawError ( void )
{
	activeGUI->setSubdrawError();
}
// Special states
void CDuskGUIElement::setSubdrawPulse ( void )
{
	activeGUI->setSubdrawPulse();
}
void CDuskGUIElement::setSubdrawOverrideColor ( const Color& color )
{
	activeGUI->setSubdrawOverrideColor(color);
}
// Reset states
void CDuskGUIElement::setSubdrawDefault ( void )
{
	activeGUI->setSubdrawDefault();
}

void CDuskGUIElement::drawRect ( const Rect& rect )
{
	activeGUI->drawRect(rect);
}
void CDuskGUIElement::drawRectWire ( const Rect& rect )
{
	// if has focus, draws at a bright color.
	// other draws at a dark color. automatically sets.
	activeGUI->drawRectWire(rect,hasFocus);
}
void CDuskGUIElement::drawLine ( const ftype x1, const ftype y1, const ftype x2, const ftype y2 )
{
	activeGUI->drawLine(x1,y1,x2,y2);
}
	
void CDuskGUIElement::drawText ( const ftype x, const ftype y, const char* str )
{
	activeGUI->drawText(x,y,str);
}
void CDuskGUIElement::drawTextWidth ( const ftype x, const ftype y, const ftype w, const char* str )
{
	activeGUI->drawTextWidth(x,y,w,str);
}
void CDuskGUIElement::drawTextCentered ( const ftype x, const ftype y, const char* str )
{
	activeGUI->drawTextCentered(x,y,str);
}
