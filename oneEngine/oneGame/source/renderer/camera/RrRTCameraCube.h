#ifndef RENDERER_RENDER_TARGET_CAMERA_CUBE_H_
#define RENDERER_RENDER_TARGET_CAMERA_CUBE_H_

#include "RrRTCamera.h"

class RrRenderTextureCube;

// RrRTCameraCube
// Specialized render method to render the scene multiple times at different viewports to the same camera cube.
class RrRTCameraCube : public RrRTCamera
{
	CameraType( kCameraClassRTCube );
public:
	// Constructor/destructor
	explicit				RrRTCameraCube (
		renderer::rrInternalSettings* const targetSettings,
		Vector2i const& targetSize,
		Real renderFramerate = 30.0f,
		bool autoRender = true,
		bool staggerRender = false);

	//	PassCount() : Returns number of passes this camera will render
	// Must be 1 or greater in order to render.
	RENDER_API int			PassCount ( void ) override;
	//	PassRetrieve(params, array) : Writes pass information into the array given in
	// Will write either PassCount() or maxPasses passes, whatever is smaller.
	RENDER_API void			PassRetrieve ( const rrCameraPassInput* input, rrCameraPass* passList ) override;

	//	RenderBegin() : Begins rendering, pushing the current camera params.
	RENDER_API void			RenderBegin ( void ) override;
	//	RenderEnd() : Called at the end of render, cleans up any camera-specific objects.
	RENDER_API void			RenderEnd ( void ) override;

	//	LateUpdate() : Pre-render update
	// Updates active camera, viewports, and matrices.
	// Called just before any sorting or rendering is done.
	RENDER_API void			LateUpdate ( void ) override;

	//	UpdateMatrix() : Updates the camera matrices.
	RENDER_API void			UpdateMatrix ( void ) override;

private:

	bool		m_rendered;
	bool		m_staggerRender;
	uchar		m_staggerTarget;

	Matrix4x4	viewCubeMatrices [6];
	Matrix4x4	projCubeMatrices [6];
	Matrix4x4	viewprojCubeMatrices [6];
};

#endif//RENDERER_RENDER_TARGET_CAMERA_CUBE_H_