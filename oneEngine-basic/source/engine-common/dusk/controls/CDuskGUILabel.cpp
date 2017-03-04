
#include "../CDuskGUI.h"
#include "CDuskGUILabel.h"

void CDuskGUILabel::Update ( void )
{
	CDuskGUIElement::Update();
	rect.size.x = 64.0F;
	rect.size.y = 24.0F;
}
void CDuskGUILabel::Render ( void )
{
	Rect screen = activeGUI->GetScreenRect();
	//drawText( rect.pos.x + rect.size.x*0.01f  + 0.01f*screen.size.x, rect.pos.y + rect.size.y*0.01f + 0.03f*screen.size.y, label.c_str() );
	//drawText( rect.pos.x + rect.size.x*0.01f, rect.pos.y + rect.size.y*0.01f + 0.03f*screen.size.y, label.c_str() );
	drawText( rect.pos.x, rect.pos.y + 0.03f*screen.size.y, label.c_str() );
}