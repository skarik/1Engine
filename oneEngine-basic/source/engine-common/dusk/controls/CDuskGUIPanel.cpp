
#include "../CDuskGUI.h"
#include "CDuskGUIPanel.h"

void CDuskGUIPanel::Update ( void )
{
	CDuskGUIElement::Update();
}
void CDuskGUIPanel::Render ( void )
{
	setDrawDefault();
	drawRectWire( rect );
	drawRect( rect );

	// Now draw text
	drawText( rect.pos.x + rect.size.x*0.01f  + 0.01f, rect.pos.y + rect.size.y*0.01f + 0.03f, label.c_str() );
}