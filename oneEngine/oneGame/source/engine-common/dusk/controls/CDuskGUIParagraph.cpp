
#include "../CDuskGUI.h"
#include "CDuskGUIParagraph.h"

void CDuskGUIParagraph::Update ( void )
{
	CDuskGUIElement::Update();
}
void CDuskGUIParagraph::Render ( void )
{
	/*activeGUI->matDefault->bind();

		glColor4f(
			activeGUI->matDefault->diffuse.red * 0.6f,
			activeGUI->matDefault->diffuse.green * 0.6f,
			activeGUI->matDefault->diffuse.blue * 0.6f,
			activeGUI->matDefault->diffuse.alpha * 0.6f );
		GLd.DrawSet2DMode( GL.D2D_WIRE );
		GLd.DrawRectangleA( rect.pos.x, rect.pos.y, rect.size.x, rect.size.y );

		glColor4f(
			activeGUI->matDefault->diffuse.red,
			activeGUI->matDefault->diffuse.green,
			activeGUI->matDefault->diffuse.blue,
			activeGUI->matDefault->diffuse.alpha * 0.6f );
		GLd.DrawSet2DMode( GL.D2D_FLAT );
		GLd.DrawRectangleA( rect.pos.x, rect.pos.y, rect.size.x, rect.size.y );

	activeGUI->matDefault->unbind();*/

	// Now draw text
	/*activeGUI->matFont->bind();
	activeGUI->fntDefault->Set();
		
		GLd.DrawSet2DScaleMode( GL.SCALE_DEFAULT );*/
		drawTextWidth( rect.pos.x + 0.01f, rect.pos.y + 0.02f, rect.size.x - 0.02f, label.c_str() );
		//GL.DrawAutoTextWrapped( rect.pos.x + rect.size.x*0.01f  + 0.01f, rect.pos.y + rect.size.y*0.01f + 0.03f, rect.size.x, label.c_str() );

	//activeGUI->fntDefault->Unbind();
	//activeGUI->matFont->unbind();
}