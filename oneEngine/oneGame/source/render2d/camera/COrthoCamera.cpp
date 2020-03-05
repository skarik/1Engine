#include "core/system/Screen.h"
#include "renderer/texture/RrRenderTexture.h"
#include "renderer/state/RrRenderer.h"

#include "render2d/state/WorldPalette.h"

#include "COrthoCamera.h"

// Construct orthographic camera
COrthoCamera::COrthoCamera ( void )
	: RrCamera()
{
	pixel_scale_aspect_ratio= (Real)Screen::Info.width/(Real)Screen::Info.height;
	pixel_scale_mode		= ORTHOSCALE_MODE_CONSTANT;
	pixel_scale_factor		= 1.0f;
	viewport_target.pos		= Vector2f( 0,0 );
	viewport_target.size	= Vector2f( (Real)Screen::Info.width, (Real)Screen::Info.height );
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
		orthoSize.x = (Real)Screen::Info.width * pixel_scale_factor;
		orthoSize.y = (Real)Screen::Info.height * pixel_scale_factor;
		break;
	case ORTHOSCALE_MODE_SIMPLE:
		orthoSize.x = viewport_target.size.x;
		orthoSize.y = viewport_target.size.y;
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
	RrCamera::UpdateMatrix();
}

// Update parameters needed for 2D rendering
void COrthoCamera::RenderBegin ( void )
{
	/*RrMaterial* palette_pass_material = SceneRenderer->GetScreenMaterial( kRenderModeDeferred, renderer::kPipelineMode2DPaletted );
	palette_pass_material->setTexture(TEX_SLOT5, (RrTexture*)Render2D::WorldPalette::Active()->GetTexture());	// Set Palette
	palette_pass_material->setTexture(TEX_SLOT6, (RrTexture*)Render2D::WorldPalette::Active()->GetTexture3D());	// Set 3D lookup

	// Remove filtering on the upscaling pass
	SceneRenderer->GetDeferredBuffer()->SetFilter( SamplingPoint );*/

	// Set the 2D pipeline mode
	RrRenderer::Active->SetPipelineMode( renderer::kPipelineMode2DPaletted );

	// Set up the camera normally
	RrCamera::RenderBegin();
}