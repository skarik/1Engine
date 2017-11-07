
#include "core/system/Screen.h"
#include "renderer/texture/CRenderTexture.h"
#include "renderer/state/CRenderState.h"
#include "renderer/material/RrMaterial.h"

#include "render2d/state/WorldPalette.h"

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
	view_roll = 0;
}

	// Camera position setup
void COrthoCamera::UpdateMatrix ( void )
{
	// Override to be orthographic
	orthographic = true;

	// Override certain camera aspects to get the right projection
	transform.rotation = Rotator( view_roll,90,-90 );
	transform.position.z = -500;
	zNear = 1;
	zFar = 1000;

	// Round to the pixel position
	transform.position.x = std::floor(transform.position.x);
	transform.position.y = std::floor(transform.position.y);

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
		throw core::NotYetImplementedException();
		/*
		Real targetdim_view  = 

		// Use those two values to set the ortho_size based on a new pixel scale factor
		Real targetdim_scalar = targetdim_view / targetdim_screen;
		ortho_size.x = (Real)Screen::Info.width * targetdim_scalar;
		ortho_size.y = (Real)Screen::Info.height * targetdim_scalar;
		*/
		break;
	case ORTHOSCALE_MODE_TOUCH_OUTSIDE:
		throw core::NotYetImplementedException();
		break;
	}

	// After projection parameters has been modified, perform orthographic view normally.
	CCamera::UpdateMatrix();
}

// Update parameters needed for 2D rendering
void COrthoCamera::RenderSet ( void )
{
	RrMaterial* palette_pass_material = SceneRenderer->GetScreenMaterial( RENDER_MODE_DEFERRED, renderer::SP_MODE_2DPALETTE );
	palette_pass_material->setTexture(TEX_SLOT5, (CTexture*)Render2D::WorldPalette::Active()->GetTexture());	// Set Palette
	palette_pass_material->setTexture(TEX_SLOT6, (CTexture*)Render2D::WorldPalette::Active()->GetTexture3D());	// Set 3D lookup
	// Remove filtering on the upscaling pass
	SceneRenderer->GetDeferredBuffer()->SetFilter( SamplingPoint );

	RrMaterial::special_mode = renderer::SP_MODE_2DPALETTE;

	// Set up the camera normally
	CCamera::RenderSet();
}