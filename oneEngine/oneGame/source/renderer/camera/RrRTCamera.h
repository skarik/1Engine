#ifndef RENDERER_RENDER_TARGET_CAMERA_H_
#define RENDERER_RENDER_TARGET_CAMERA_H_

#include "RrCamera.h"
#include "renderer/state/InternalSettings.h"
#include "renderer/state/RrHybridBufferChain.h"

// Camera class with various changes to provide common techniques to optimize rendering to texture.
class RrRTCamera : public RrCamera
{
	CameraType( kCameraClassRTNormal );
public:
	RENDER_API explicit		RrRTCamera (
		renderer::rrInternalSettings* const targetSettings,
		Vector2i const& targetSize,
		Real renderFramerate = 30.0f,
		bool autoRender = false
		);
	RENDER_API				~RrRTCamera ( void );

	//	LateUpdate() : Pre-render update
	// Updates active camera, viewports, and matrices.
	// Called just before any sorting or rendering is done.
	void					LateUpdate ( void ) override;
	//	UpdateMatrix() : Updates the camera matrices.
	void					UpdateMatrix ( void ) override;

	//	RenderBegin() : Begins rendering, pushing the current camera params.
	void					RenderBegin ( void ) override;
	//	RenderEnd() : Called at the end of render, cleans up any camera-specific objects.
	void					RenderEnd ( void ) override;

	//	PassCount() : Returns number of passes this camera will render
	// Must be 1 or greater in order to render.
	RENDER_API int			PassCount ( void ) override;
	//	PassRetrieve(params, array) : Writes pass information into the array given in
	// Will write either PassCount() or maxPasses passes, whatever is smaller.
	RENDER_API void			PassRetrieve ( const rrCameraPassInput* input, rrCameraPass* passList ) override;

	//	SetTargetInfo(settings, size) : Sets up render target for the camera.
	// Will free previously created buffers.
	// Returns true on successful creation.
	RENDER_API bool			SetTargetInfo ( renderer::rrInternalSettings* const settings, Vector2i const& size );
	//	FreeTarget() : Frees up the render target.
	RENDER_API bool			FreeTarget ( void );

public:
	// Does this camera render automatically?
	bool			m_autoRender;
	// The time in seconds between each frame.
	Real			m_renderStepTime;

protected:
	//	UpdateTextureMatrix() : Update texture matrix.
	void					UpdateTextureMatrix ( void );

	// Counter for rendering
	Real			m_renderCounter;
	// Current camera transforms
	Matrix4x4		textureMatrix;
	// Actual render settings
	renderer::rrInternalSettings
					m_usedTargetSettings;
	// Buffer chain used for rendering
	RrHybridBufferChain
					m_chain;
	// Buffer chain size
	Vector2i		m_targetSize;

};

#endif//RENDERER_RENDER_TARGET_CAMERA_H_