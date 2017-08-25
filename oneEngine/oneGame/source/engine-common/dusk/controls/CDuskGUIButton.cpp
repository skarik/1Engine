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
			//activeGUI->matDown->bindPass(0);
		else
			setDrawHover();
			//activeGUI->matHover->bindPass();
	}
	else
	{
		//activeGUI->matDefault->bindPass();
		setDrawDefault();
	}

	// Begin draw/ material

		/*glColor4f(
			!hasFocus ? RrMaterial::current->m_diffuse.red * 0.6f : RrMaterial::current->m_diffuse.red * 1.1f,
			!hasFocus ? RrMaterial::current->m_diffuse.green * 0.6f : RrMaterial::current->m_diffuse.green * 1.1f,
			!hasFocus ? RrMaterial::current->m_diffuse.blue * 0.6f : RrMaterial::current->m_diffuse.blue * 1.1f,
			RrMaterial::current->m_diffuse.alpha * 0.6f );*/
		drawRectWire( rect );

		/*glColor4f(
			RrMaterial::current->m_diffuse.red,
			RrMaterial::current->m_diffuse.green,
			RrMaterial::current->m_diffuse.blue,
			RrMaterial::current->m_diffuse.alpha * 0.6f );
		GLd.DrawSet2DMode( GL.D2D_FLAT );
		GLd.DrawRectangleA( rect.pos.x, rect.pos.y, rect.size.x, rect.size.y );*/
		drawRect( rect );

	// Now draw text
		drawText( rect.pos.x + rect.size.x*0.1f, rect.pos.y + rect.size.y*0.1f  + 0.02f*activeGUI->GetScreenRect().size.y, label.c_str() );

}