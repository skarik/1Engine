
#include "../CDuskGUI.h"
#include "CDuskGUIEdgePanel.h"

// Panel used for the backdrop of other elements
CDuskGUI::Handle CDuskGUI::CreateEdgePanel	( const Handle& parent )
{
	vElements.push_back( new CDuskGUIEdgePanel() );
	vElements.back()->parent = parent;
	return Handle(vElements.size()-1);
}

void CDuskGUIEdgePanel::Update ( void )
{
	CDuskGUIPanel::Update();

	// Get screen rect
	Rect screen_rect = activeGUI->GetScreenRect();

	// Rework the actual rect
	actual_rect = rect;
	if ( rect.size.x > rect.size.y )
	{
		actual_rect.size.x = screen_rect.size.x;
		if ( rect.pos.y < screen_rect.size.y * 0.5F )
			actual_rect.pos.y = 0.0F;
		else
			actual_rect.pos.y = screen_rect.size.y - actual_rect.size.y;
	}
	else
	{
		actual_rect.size.y = screen_rect.size.y;
		if ( rect.pos.x < screen_rect.size.x * 0.5F )
			actual_rect.pos.x = 0.0F;
		else
			actual_rect.pos.x = screen_rect.size.x - actual_rect.size.x;
	}
}

void CDuskGUIEdgePanel::Render ( void )
{
	// draw panel
	setDrawDefault();
	drawRectWire( actual_rect );
	setSubdrawDefault();
	drawRect( actual_rect );

	// Now draw text
	drawText( actual_rect.pos.x + actual_rect.size.x*0.01f  + 0.01f, actual_rect.pos.y + actual_rect.size.y*0.01f + 0.03f, label.c_str() );
}