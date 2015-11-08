
#include "core/system/Screen.h"

#include "COrthoCamera.h"

// Construct orthographic camera
COrthoCamera::COrthoCamera ( void )
	: CCamera()
{
	pixel_scale_aspect_ratio= (Real)Screen::Info.width/(Real)Screen::Info.height;
	pixel_scale_mode		= ORTHOSCALE_MODE_CONSTANT;
	pixel_scale_factor		= 1.0f;
	viewport_target.pos		= Vector2d( 0,0 );
	viewport_target.size	= Vector2d( (Real)Screen::Info.width, (Real)Screen::Info.height );
}

	// Camera position setup
void COrthoCamera::UpdateMatrix ( void )
{
	// Override to be orthographic
	orthographic = true;

	// Setup orthographic view
	switch ( pixel_scale_mode )
	{
	case ORTHOSCALE_MODE_CONSTANT:
		ortho_size.x = (Real)Screen::Info.width * pixel_scale_factor;
		ortho_size.y = (Real)Screen::Info.height * pixel_scale_factor;
		break;
	case ORTHOSCALE_MODE_SIMPLE:
		ortho_size.x = viewport_target.size.x;
		ortho_size.y = viewport_target.size.y;
		break;
	case ORTHOSCALE_MODE_TOUCH_INSIDE:
		throw Core::NotYetImplementedException();
		break;
	case ORTHOSCALE_MODE_TOUCH_OUTSIDE:
		throw Core::NotYetImplementedException();
		break;
	}

	// After projection parameters has been modified, perform orthographic view normally.
	CCamera::UpdateMatrix();
}