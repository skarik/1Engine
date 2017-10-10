#include "../CDuskGUI.h"
#include "CDuskGUIParagraph.h"

void CDuskGUIParagraph::Update ( void )
{
	CDuskGUIElement::Update();
}
void CDuskGUIParagraph::Render ( void )
{
	// Now draw text
	drawTextWidth( rect.pos.x + 0.01f, rect.pos.y + 0.02f, rect.size.x - 0.02f, label.c_str() );
}