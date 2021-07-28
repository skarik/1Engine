
#ifndef _RENDER2D_C_ORTHO_CAMERA_H_
#define _RENDER2D_C_ORTHO_CAMERA_H_

// Include base renderer engine's camera
#include "renderer/camera/RrCamera.h"

// Enumeration for type of camera scaling
enum orthographicScaleMode_t
{
	// Doesn't scale with the view. The size of the orthographic view is the size of the window.
	ORTHOSCALE_MODE_CONSTANT,
	// Scales the output to the window. Size of the orthographic view does not change.
	ORTHOSCALE_MODE_SIMPLE,
	// Scales the output to the window, but takes into account that the new window size may not be the same aspect ratio anymore.
	// After treating the view as a square, takes the smallest out-of-range dimension and scales the view to it, meaning the view will
	// never show more than the viewport intends to, but may cut off some pixels. Bad explanation, someone reword this.
	ORTHOSCALE_MODE_TOUCH_INSIDE,
	// Scales the output to the window, but takes into account that the new window size may not be the same aspect ratio anymore.
	// After treating the view as a square, takes the largest out-of-range dimension and scales the view to it, meaning the view will
	// always have all its data visible, but may have extra pixels shown.
	ORTHOSCALE_MODE_TOUCH_OUTSIDE,
};

// Class Definition
class COrthoCamera : public RrCamera
{
	CameraType( kCameraClassNormal );
public:
	// starting/target aspect ratio of the screen
	Real		pixel_scale_aspect_ratio;
	// how the view scales when the screen size is changed
	orthographicScaleMode_t	pixel_scale_mode;
	// scale of scaling, where 1.0 is the default behavior, and 0.5 is zoomed in
	Real		pixel_scale_factor;
	// starting/target viewport of the screen
	Rect		viewport_target;

	// roll of the screen
	Real		view_roll;

public:
	// Construct orthographic camera
	RENDER2D_API explicit		COrthoCamera ( bool isTransient );

	// Camera position setup
	RENDER2D_API void UpdateMatrix ( void ) override;

	// Update parameters needed for 2D rendering
	RENDER2D_API void RenderBegin ( gpu::GraphicsContext* graphics_context ) override;
};

#endif//_RENDER2D_C_ORTHO_CAMERA_H_