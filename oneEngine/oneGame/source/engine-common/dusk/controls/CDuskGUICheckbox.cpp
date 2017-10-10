#include "../CDuskGUI.h"
#include "CDuskGUICheckbox.h"

void CDuskGUICheckbox::Update ( void )
{
	CDuskGUIButton::Update();

	lastBoolValue = boolValue;
	if ( isPressed )
	{
		boolValue = !boolValue;
	}
}
void CDuskGUICheckbox::Render ( void )
{
	// Material binding
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

	// Begin draw/ material
		drawRectWire( Rect( rect.pos.x, rect.pos.y, 0.03f, 0.03f ) );

	if ( boolValue )
		setSubdrawSelection();
	else
		setSubdrawDefault();

	drawRect( Rect( rect.pos.x, rect.pos.y, 0.03f, 0.03f ) );

	drawText( rect.pos.x + rect.size.x*0.01f  + 0.01f, rect.pos.y + rect.size.y*0.01f + 0.03f, label.c_str() );
}