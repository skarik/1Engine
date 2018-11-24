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
	static std::vector<RrCamera*>
						m_CameraList;

public:

	// In the Renderer, set to either the current active rendering camera, or main viewport renderer.
	// In the Engine, set to the renderer of the main viewport.
	RENDER_API static RrCamera*
						activeCamera;

	//	CameraList() : Returns a list of all the currently active cameras.
	RENDER_API static const std::vector<RrCamera*>&
							CameraList ( void )
		{ return m_CameraList; }


public:
	RENDER_API explicit		RrCamera ( void );
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
	//	PassRetrieve(array, array_size) : Writes pass information into the array given in
	// Will write either PassCount() or maxPasses passes, whatever is smaller.
	RENDER_API virtual void	PassRetrieve ( rrCameraPass* passList, const uint32_t maxPasses );

	//	RenderBegin() : Begins rendering, pushing the current camera params.
	RENDER_API virtual void	RenderBegin ( void );
	//	RenderEnd() : Called at the end of render, cleans up any camera-specific objects.
	RENDER_API virtual void	RenderEnd ( void );
	
	//	SetActive() : sets camera as active
	// Sets active camera.
	// Note that set active is only valid if the camera is not an RT camera.
	RENDER_API void			SetActive ( void );
	// Sets the rotation.
	// Since cameras in OpenGL do things via a ZYX rotation, this function takes care of the dirty bits.
	// Takes angles where X is roll, Y is pitch, and Z is yaw.
	RENDER_API void			SetRotation ( const Rotator& );

	//
	// Property & State queries:

	// Grab privately generated camera index
	RENDER_API int8_t		GetCameraIndex ( void )
		{ return m_cameraIndex; }

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
	RENDER_API bool			PointIsVisible ( const Vector3d& point );
	// SphereIsVisible( center, radius ) : Can this camera see the given sphere
	RENDER_API bool			SphereIsVisible ( const Vector3d& center, const Real radius);
	// BoundingBoxIsVisible( bbox ) : Can this camera see the given bounding box
	RENDER_API bool			BoundingBoxIsVisible ( const core::math::BoundingBox& bbox );

	// == Transform Queries ==
	RENDER_API Vector3d		GetUp ( void ) { return up; };

	// == Coordinate Queries ==
	RENDER_API Vector3d		WorldToScreenPos ( const Vector3d & ) const;
	RENDER_API Vector3d		ScreenToWorldDir ( const Vector2d & ) const;
	RENDER_API Vector3d		ScreenToWorldPos ( const Vector2d & ) const;



private:

	void					UpdateFrustum ( void );

public:
	// is the camera available for rendering
	bool				active;
	// current camera position & rotation to render from
	XrTransform			transform;
	// Render options
	Real				zNear;
	Real				zFar;
	// Viewport options
	Real				renderScale;
	Rect				viewportPercent;
	bool				mirrorView;
	// Orthographic options
	bool				orthographic;
	Vector2d			orthoSize;
	// Physical options
	Real				fieldOfView;
	Real				focalDistance;
	Real				focalRange;

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
	Vector3d			up;
	Vector3d			forward;
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

	// Needs a new frame render? True when we should render a new frame from this camera.
	// Is normally always true for the base camera class.
	bool				m_needsNewPasses;

private:
	// Camera index given by the system.
	// Gauranteed to be unique within the camera listing.
	int8_t				m_cameraIndex;

};

#endif//RENDERER_CAMERA_H_