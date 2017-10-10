#include "core/input/CInput.h"

#include "../CDuskGUI.h"
#include "CDuskGUIButton.h"

void CDuskGUIButton::Update ( void )
{
	CDuskGUIElement::Update();

	isPressed = false;
	if ( mouseIn && drawn )
	{
		// Mouse controls
		if ( CInput::MouseDown(CInput::MBLeft) )
		{
			beginPress = true;
		}
		else if ( beginPress )
		{
			if ( CInput::MouseUp(CInput::MBLeft) )
			{
				beginPress = false;
				isPressed = true;
			}
		}
	}
	else
	{
		beginPress = false;

		// Keyboard controls
		if ( hasFocus )
		{
			if ( CInput::Keydown( Keys.Return ) )
			{
				isPressed = true;
			}
		}
	}
}
void CDuskGUIButton::Render ( void )
{
	// Material binding
	if ( mouseIn )
	{
		if ( beginPress )
			setDrawDown();
		else
			setDrawHover();
	}
	else
	{
		setDrawDefault();
	}

	// Begin draw/ material
	drawRectWire( rect );
	drawRect( rect );

	// Now draw text
	drawText( rect.pos.x + rect.size.x*0.1f, rect.pos.y + rect.size.y*0.1f  + 0.02f*activeGUI->GetScreenRect().size.y, label.c_str() );
}