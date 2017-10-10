
#include "../CDuskGUI.h"
#include "CDuskGUIMaterialField.h"
#include "renderer/material/RrMaterial.h"
#include "renderer/system/glDrawing.h"

DuskGUI::Handle CDuskGUI::CreateMaterialfield ( const Handle& parent, const string& defVal, RrMaterial* mat )
{
	vElements.push_back( new CDuskGUIMaterialField() );
	vElements.back()->parent = parent;
	((CDuskGUIMaterialField*)vElements.back())->label = defVal;
	((CDuskGUIMaterialField*)vElements.back())->target = mat;
	return Handle(vElements.size()-1);
}
void CDuskGUI::SetMaterialfieldTarget ( const Handle& handle, RrMaterial* mat )
{
	CDuskGUIMaterialField* tf = (CDuskGUIMaterialField*)vElements[int(handle)];
	tf->target = mat;
}

void CDuskGUIMaterialField::Update ( void )
{
	CDuskGUITextfield::Update();

	if ( lastTextValue != textValue ) {
		hasValidValue = RrMaterial::isValidFile( textValue.c_str() );
		if ( hasValidValue ) {
			if ( target ) {
				target->loadFromFile( textValue.c_str() );
			}
		}
	}
}

void CDuskGUIMaterialField::Render ( void )
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

	if ( !hasValidValue ) {
		setSubdrawError();
	}
	else {
		setSubdrawDefault();
	}
	drawRectWire( rect );
	drawRect( rect );


	// Now draw text
		
	drawText( rect.pos.x + rect.size.x*0.01f  + 0.01f, rect.pos.y + rect.size.y*0.01f + 0.03f, label.c_str() );
	//GLd.DrawSet2DScaleMode(GLd.SCALE_WIDTH);
	//Real cursor_x = rect.pos.x + rect.size.x*0.01f + 0.012f + (GLd.GetAutoTextWidth( label.c_str() )/Screen::Info.width);

	//if ( hasFocus )
	//{
	//	setSubdrawPulse();
	//	drawLine( cursor_x, rect.pos.y + rect.size.y*0.08f + 0.005f, cursor_x, rect.pos.y + rect.size.y*0.82f );
	//}
}