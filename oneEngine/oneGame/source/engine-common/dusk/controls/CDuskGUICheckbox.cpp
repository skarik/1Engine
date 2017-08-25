

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

		/*glColor4f(
			RrMaterial::current->diffuse.red * 0.6f,
			RrMaterial::current->diffuse.green * 0.6f,
			RrMaterial::current->diffuse.blue * 0.6f,
			RrMaterial::current->diffuse.alpha * 0.6f );
		GLd.DrawSet2DMode( GL.D2D_WIRE );
		GLd.DrawRectangleA(  );*/
		drawRectWire( Rect( rect.pos.x, rect.pos.y, 0.03f, 0.03f ) );

	if ( boolValue )
		setSubdrawSelection();
		/*glColor4f(
			RrMaterial::current->diffuse.red * 1.7f,
			RrMaterial::current->diffuse.green * 1.7f,
			RrMaterial::current->diffuse.blue * 1.7f,
			RrMaterial::current->diffuse.alpha * 0.7f );*/
	else
		setSubdrawDefault();
		/*glColor4f(
			RrMaterial::current->diffuse.red,
			RrMaterial::current->diffuse.green,
			RrMaterial::current->diffuse.blue,
			RrMaterial::current->diffuse.alpha * 0.6f );
		GLd.DrawSet2DMode( GL.D2D_FLAT );*/
		//GLd.DrawRectangleA( rect.pos.x, rect.pos.y, 0.03f, 0.03f );
		drawRect( Rect( rect.pos.x, rect.pos.y, 0.03f, 0.03f ) );

	//RrMaterial::current->unbind();

	// Now draw text
	/*activeGUI->matFont->bind();
	activeGUI->fntDefault->Set();
		
		GL.DrawAutoText( rect.pos.x + rect.size.x*0.01f  + 0.01f, rect.pos.y + rect.size.y*0.01f + 0.03f, label.c_str() );

	activeGUI->fntDefault->Unbind();
	activeGUI->matFont->unbind();*/
		drawText( rect.pos.x + rect.size.x*0.01f  + 0.01f, rect.pos.y + rect.size.y*0.01f + 0.03f, label.c_str() );
}