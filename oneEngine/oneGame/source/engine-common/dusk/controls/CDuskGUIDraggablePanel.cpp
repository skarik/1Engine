#include "core/input/CInput.h"

#include "../CDuskGUI.h"
#include "CDuskGUIDraggablePanel.h"

// Panel used for the backdrop of other elements
CDuskGUI::Handle CDuskGUI::CreateDraggablePanel	( const Handle& parent )
{
	vElements.push_back( new CDuskGUIDraggablePanel() );
	vElements.back()->parent = parent;
	return Handle(vElements.size()-1);
}

void CDuskGUIDraggablePanel::Update ( void )
{
	CDuskGUIPanel::Update();
	
	// Grab screen actually using
	Rect screen = activeGUI->GetScreenRect();
	// Add margins
	screen.pos += screen.size * 0.01F;
	screen.size *= 1.0F - 0.01F;

	if ( hasFocus && Input::Mouse(Input::MBLeft) )
	{
		if ( !isDragging )
		{
			isDragging = true;
			//vSourcePoint = Vector2f( Input::mouseX/(Real)Screen::Info.width, Input::mouseY/(Real)Screen::Info.height );
		}
		// Drag around
		//Vector2f vTargetPoint = Vector2f( Input::mouseX/(Real)Screen::Info.width, Input::mouseY/(Real)Screen::Info.height );
		Vector2f vDeltaPoint = Vector2f( Input::DeltaMouseX(), Input::DeltaMouseY() );
		if ( !activeGUI->bInPixelMode ) {
			vDeltaPoint = vDeltaPoint.divComponents( Vector2f( (Real)Screen::Info.width, (Real)Screen::Info.height ) );
		}

		// Limit position
		if ( rect.pos.x + rect.size.x < screen.pos.x ) {
			vDeltaPoint.x = screen.pos.x - ( rect.pos.x + rect.size.x );
			vDeltaPoint.x += screen.size.x * 0.001F;
		}
		else if ( rect.pos.x > screen.size.x ) {
			vDeltaPoint.x = screen.size.x - ( rect.pos.x );
			vDeltaPoint.x -= screen.size.x * 0.001F;
		}
		if ( rect.pos.y + rect.size.y < screen.pos.y ) {
			vDeltaPoint.y = screen.pos.y - ( rect.pos.y + rect.size.y );
			vDeltaPoint.y += screen.size.y * 0.001F;
		}
		else if ( rect.pos.y > screen.size.y ) {
			vDeltaPoint.y = screen.size.y - ( rect.pos.y );
			vDeltaPoint.y -= screen.size.y * 0.001F;
		}

		rect.pos += vDeltaPoint;

		// Now loop through the main to find all children
		/*std::vector<Handle> child_handles;
		// First find handle name
		Handle m_handle = activeGUI->GetFromPointer( this );
		activeGUI->GetChildren( m_handle, child_handles );
		// Now that have all children
		for ( uint i = 0; i < child_handles.size(); ++i ) {
			activeGUI->vElements[child_handles[i]]->rect.pos += vDeltaPoint;
		}*/
	}
	else {
		isDragging = false;
	}
}

void CDuskGUIDraggablePanel::Render ( void )
{
	setDrawDefault();

	drawRectWire( rect );

	if ( isDragging ) {
		setSubdrawTransparent();
	}
	else {
		setSubdrawDefault();
	}

	drawRect( rect );

	// Now draw text
	if ( !activeGUI->bInPixelMode )
		drawText( rect.pos.x + rect.size.x*0.01f  + 0.01f, rect.pos.y + rect.size.y*0.01f + 0.03f, label.c_str() );
	else
		drawText( rect.pos.x + 10, rect.pos.y + 20, label.c_str() );
}