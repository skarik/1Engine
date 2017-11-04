#include "core/input/CInput.h"

#include "../CDuskGUI.h"
#include "CDuskGUITextfield.h"
#include <cctype>

#include "renderer/system/glDrawing.h"

void CDuskGUITextfield::Update ( void )
{
	CDuskGUIElement::Update();

	lastTextValue = textValue;
	if ( hasFocus )
	{
		GetStringValue();
	}
	else
	{	// Bugfix for when changing value without focus
		if ( textValue != label )
		{
			textValue = label;
		}
	}
}
void CDuskGUITextfield::Render ( void )
{
	GLd_ACCESS

	// Material binding
	if ( mouseIn ) {
		setDrawHover();
	}
	else
	{
		if ( hasFocus )
			setDrawDown();
		else
			setDrawDefault();
	}

	setSubdrawDefault();
	drawRectWire( rect );
	drawRect( rect );

	Rect screen = activeGUI->GetScreenRect();

	// Now draw text
	drawText( rect.pos.x + rect.size.x*0.01f, rect.pos.y + rect.size.y*0.01f + 0.03f*screen.size.y, label.c_str() );
	//Real cursor_x = rect.pos.x + rect.size.x*0.01f + 0.002f*screen.size.x + (GLd.GetAutoTextWidth( label.c_str() )/Screen::Info.width)*screen.size.x;

	//if ( hasFocus )
	//{
	//	setSubdrawPulse();
	//	drawLine( cursor_x, rect.pos.y + rect.size.y*0.08f + 0.005f*screen.size.y, cursor_x, rect.pos.y + rect.size.y*0.82f );
	//}
}

void CDuskGUITextfield::GetStringValue ( void )
{
	if ( label.size() < 24 )
	{
		//if ( Input::KeydownAny() )
		//{
			unsigned char i = Input::GetTypeChar(); // Get typed character
			/*if ( isalpha(i) || (i=='\'') || (i=='`') || (i=='-') ) { //only allow alphabetical characters and '`- for names
				label += i;
			}*/
			if (( i != Keys.Return )&&( i != 0 ))
			{
				label += i;
			}
		//}
	}
	if ( Input::Keydown( Keys.Backspace ) ) // Delete char if backspace
	{
		if ( label.size() > 0 )
		{
			label.resize( label.size()-1 );
		}
	}

	textValue = label;
}