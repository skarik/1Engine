
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
		
	if ( hasFocus && Input::Mouse(Input::MBLeft) ) {
		if ( !isDragging ) {
			isDragging = true;
			//vSourcePoint = Vector2d( Input::mouseX/(ftype)Screen::Info.width, Input::mouseY/(ftype)Screen::Info.height );
		}
		// Drag around
		//Vector2d vTargetPoint = Vector2d( Input::mouseX/(ftype)Screen::Info.width, Input::mouseY/(ftype)Screen::Info.height );
		Vector2d vDeltaPoint = Vector2d( Input::DeltaMouseX()/(ftype)Screen::Info.width, Input::DeltaMouseY()/(ftype)Screen::Info.height );

		// Limit position
		if ( rect.pos.x + rect.size.x < 0.01f ) {
			vDeltaPoint.x = 0.01f - ( rect.pos.x + rect.size.x );
			vDeltaPoint.x *= 1.1f;
		}
		else if ( rect.pos.x > 0.99f ) {
			vDeltaPoint.x = 0.99f - ( rect.pos.x );
			vDeltaPoint.x *= 1.1f;
		}
		if ( rect.pos.y + rect.size.y < 0.012f ) {
			vDeltaPoint.y = 0.012f - ( rect.pos.y + rect.size.y );
			vDeltaPoint.y *= 1.1f;
		}
		else if ( rect.pos.y > 0.988f ) {
			vDeltaPoint.y = 0.988f - ( rect.pos.y );
			vDeltaPoint.y *= 1.1f;
		}

		rect.pos += vDeltaPoint;

		// Now loop through the main to find all children
		std::vector<Handle> child_handles;
		/*vector<Handle> parent_handles;
		//vector<CDuskGUIElement*>::iterator foundVal = find( activeGUI->vElements.begin(), activeGUI->vElements.end(), this );
		for ( uint i = 0; i < activeGUI->vElements.size(); ++i ) {
			if ( activeGUI->vElements[i] == this ) {
				parent_handles.push_back( (Handle)i );
				break;
			}
		}
		for ( uint i = 0; i < activeGUI->vElements.size(); ++i ) {
			// First look for objects that have a parent in the list, then add them both to child_handles and parent_handles	
		}*/
		// First find handle name
		Handle m_handle = activeGUI->GetFromPointer( this );
		activeGUI->GetChildren( m_handle, child_handles );
		/*for ( uint i = 0; i < child_handles.size(); ++i ) {
			activeGUI->vElements[child_handles[i]]->rect.pos += vDeltaPoint;
		}*/

		// Now that have all children
		for ( uint i = 0; i < child_handles.size(); ++i ) {
			activeGUI->vElements[child_handles[i]]->rect.pos += vDeltaPoint;
		}	
	}
	else {
		isDragging = false;
	}
}

void CDuskGUIDraggablePanel::Render ( void )
{
	//activeGUI->matDefault->bind();

		/*if ( hasFocus ) {
			glColor4f(
				activeGUI->matDefault->diffuse.red * 2.0f,
				activeGUI->matDefault->diffuse.green * 2.0f,
				activeGUI->matDefault->diffuse.blue * 2.0f,
				activeGUI->matDefault->diffuse.alpha * 0.6f );
		}
		else {
			glColor4f(
				activeGUI->matDefault->diffuse.red * 0.6f,
				activeGUI->matDefault->diffuse.green * 0.6f,
				activeGUI->matDefault->diffuse.blue * 0.6f,
				activeGUI->matDefault->diffuse.alpha * 0.6f );
		}
		GLd.DrawSet2DMode( GL.D2D_WIRE );
		GLd.DrawRectangleA( rect.pos.x, rect.pos.y, rect.size.x, rect.size.y );*/
		setDrawDefault();

		drawRectWire( rect );

		if ( isDragging ) {
			setSubdrawTransparent();
		}
		else {
			setSubdrawDefault();
		}
		//GLd.DrawSet2DMode( GL.D2D_FLAT );
		//GLd.DrawRectangleA( rect.pos.x, rect.pos.y, rect.size.x, rect.size.y );
		drawRect( rect );

	// Now draw text
	drawText( rect.pos.x + rect.size.x*0.01f  + 0.01f, rect.pos.y + rect.size.y*0.01f + 0.03f, label.c_str() );
}