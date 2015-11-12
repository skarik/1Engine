
#include "../CDuskGUI.h"
#include "CDuskGUILabel.h"

void CDuskGUILabel::Update ( void )
{
	CDuskGUIElement::Update();
}
void CDuskGUILabel::Render ( void )
{
	drawText( rect.pos.x + rect.size.x*0.01f  + 0.01f, rect.pos.y + rect.size.y*0.01f + 0.03f, label.c_str() );
}