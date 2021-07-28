#ifndef RENDERER_CAMERA_H_
#define RENDERER_CAMERA_H_

#include "core/math/Frustum.h"
#include "core/math/Rect.h"
#include "core/math/BoundingBox.h"
#include "core/math/matrix/CMatrix.h"
#include "core-ext/transform/Transform.h"
#include "renderer/types/ObjectSettings.h"
#include "renderer/camera/CameraPass.h"

class RrRenderTexture;
class RrPostProcessPass;

namespace gpu
{
	class GraphicsContext;
}

// Enumeration for type of camera
enum rrCameraClassType
{
	kCameraClassNormal,
	kCameraClassRTNormal,
	kCameraClassRTCascade,
	kCameraClassRTCube
};

// Macros

#define CameraType(a) public: virtual rrCameraClassType GetType ( void ) { return a; };

// Base camera class for points to render the current state from.
class RrCamera
{
	CameraType( kCameraClassNormal );
protected:
	// List of current cameras.
	//static std::vector<RrCamera*>
	//					m_CameraList;
	RrCamera*			m_prevCamera = nullptr;
	RrCamera*			m_nextCamera = nullptr;
	static RrCamera*	g_FirstCamera;
	static RrCamera*	g_LastCamera;

public:

	// In the Renderer, set to either the current active rendering camera, or main viewport renderer.
	// In the Engine, set to the renderer of the main viewport.
	RENDER_API static RrCamera*
						activeCamera;

	//	CameraList() : Returns a list of all the currently active cameras.
	//RENDER_API static const std::vector<RrCamera*>&
	//						CameraList ( void )
	//	{ return m_CameraList; }

	static RrCamera*	GetFirstCamera ( void )
		{ return g_FirstCamera; }
	static RrCamera*	GetLastCamera ( void )
		{ return g_LastCamera; }

	RrCamera*			GetNextCamera ( void )
		{ return m_nextCamera; }

public:
	//	Constructor RrCamera(isTransient)
	//	arguments:
	//		isTransient: If true, the camera is assumed to be used for helping rendering, and is not added to the internal camera list.
	//					 Transient cameras typically only exist for a short time, ie, a subset of a single frame.
	RENDER_API explicit		RrCamera ( bool isTransient );
	RENDER_API virtual		~RrCamera ( void );

	//	LateUpdate() : Pre-render update
	// Updates active camera, viewports, and matrices.
	// Called just before any sorting or rendering is done.
	RENDER_API virtual void	LateUpdate ( void );

	// 
	// Rendering Queries:

	//	UpdateMatrix() : Updates the camera matrices.
	RENDER_API virtual void	UpdateMatrix ( void );

	//	PassCount() : Returns number of passes this camera will render
	// Must be 1 or greater in order to render.
	RENDER_API virtual int	PassCount ( void );
	//	PassRetrieve(params, array) : Generates and writes pass information into the given array.
	// Will write either PassCount() or maxPasses passes, whatever is smaller.
	// For the returned constant buffers, it will create a constant buffer if needed, then update it with the camera's parameters.
	RENDER_API virtual void	PassRetrieve ( const rrCameraPassInput* input, rrCameraPass* passList );

	//	RenderBegin() : Begins rendering, pushing the current camera params.
	// Should be called after PassRetrieve or immediately before PassRetrieve to ensure constant buffer values are correct.
	RENDER_API virtual void	RenderBegin ( gpu::GraphicsContext* graphics_context );
	//	RenderEnd() : Called at the end of render, cleans up any camera-specific objects.
	RENDER_API virtual void	RenderEnd ( void );
	
	//	SetActive() : sets camera as active
	// Sets active camera. This will force a resorting the of the cameras.
	// Note that set active is only valid if the camera is not an RT camera.
	RENDER_API void			SetActive ( void );
	// Sets the rotation.
	// Since cameras in OpenGL do things via a ZYX rotation, this function takes care of the dirty bits.
	// Takes angles where X is roll, Y is pitch, and Z is yaw.
	RENDER_API void			SetRotation ( const Rotator& );

	//
	// Property & State queries:

	// Grab privately generated camera index
	/*RENDER_API int8_t		GetCameraIndex ( void )
		{ return m_cameraIndex; }*/

	// Get if should render with camera or not
	RENDER_API bool			GetRender ( void )
		{ return m_needsNewPasses && active; }

	// Return the current render texture
	/*RENDER_API RrRenderTexture*
							GetRenderTexture ( void )
		{ return m_renderTexture; }*/

	// 
	// Mathematical & Game Queries:

	// Visibility:

	// PointIsVisible( point ) : Can this camera see the given point
	RENDER_API bool			PointIsVisible ( const Vector3f& point );
	// SphereIsVisible( center, radius ) : Can this camera see the given sphere
	RENDER_API bool			SphereIsVisible ( const Vector3f& center, const Real radius);
	// BoundingBoxIsVisible( bbox ) : Can this camera see the given bounding box
	RENDER_API bool			BoundingBoxIsVisible ( const core::math::BoundingBox& bbox );

	// == Transform Queries ==
	RENDER_API Vector3f		GetUp ( void ) { return up; };

	// == Coordinate Queries ==
	RENDER_API Vector3f		WorldToScreenPos ( const Vector3f & ) const;
	RENDER_API Vector3f		ScreenToWorldDir ( const Vector2f & ) const;
	RENDER_API Vector3f		ScreenToWorldPos ( const Vector2f & ) const;

protected:
	//	UpdateCBuffer(index) : creates if needed and updates given cbuffer in the list.
	void					UpdateCBuffer ( const uint index, const uint predictedMax, const rrCameraPass* passinfo );

private:
	//	UpdateFrustum() : recalculates view frustum based on the currently set state.
	void					UpdateFrustum ( void );

	//	RemoveFromCameraList() : Removes this current camera from the camera linked list. Useful for resorting.
	void					RemoveFromCameraList ( void );

public:
	// is the camera only created for a single frame
	bool				isTransient = false;
	// is the camera available for rendering
	bool				active = true;
	// current camera position & rotation to render from
	XrTransform			transform;
	// Render options
	Real				zNear = 0.1F;
	Real				zFar = 200.0F;
	// Viewport options
	Real				renderScale = 1.0F;
	Rect				viewportPercent = Rect(0.0F, 0.0F, 1.0F, 1.0F);
	bool				mirrorView = false;
	// Orthographic options
	bool				orthographic = false;
	Vector2f			orthoSize = Vector2f(512.0F, 512.0F);
	// Physical options
	Real				fieldOfView = 100.0F;
	Real				focalDistance = 10.0F;
	Real				focalRange = 100.0F;

	// World layers to enable
	bool				layerVisibility [renderer::kRenderLayer_MAX];
	// Set bit means to enable a layer. By default, most layers are on.
	uint32_t			enabledHints;

	// Current post-process stack
	std::vector<RrPostProcessPass*>
						postProcessStack;

protected:
	//
	// Camera resultant properties:

	// Face vectors
	Vector3f			up;
	Vector3f			forward;
	// Viewport options
	Rect				viewport;

	// Camera frustum
	//core::math::Plane frustum [6];
	core::math::Frustum	frustum;

	// Current camera transform
	Matrix4x4			viewTransform;
	Matrix4x4			projTransform;
	//Matrix4x4			textureMatrix;

	// Calculated camera transform
	Matrix4x4			viewprojMatrix;

	// Cbuffers for camera passes. Count corresponds to the buffering mode of the engine.
	// Ex: If the engine is triple (3) buffered, there will be three (3) cbuffers here.
	// For inheriting classes, the number of cbuffers may be a multiple.
	// Ex: Cubemap renderers will have 24 cbuffers for the same case (6 per frame).
	std::vector<gpu::Buffer>
						m_cbuffers;

	// Needs a new frame render? True when we should render a new frame from this camera.
	// Is normally always true for the base camera class.
	bool				m_needsNewPasses;

private:
	// Camera index given by the system.
	// Gauranteed to be unique within the camera listing.
	//int8_t				m_cameraIndex = -1;

};

#endif//RENDERER_CAMERA_H_