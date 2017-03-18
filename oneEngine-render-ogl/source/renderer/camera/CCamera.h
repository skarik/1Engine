// This will eventually be the base camera, and all crazy stuff (like input) will be done elsewhere
// But for now, it's for dicking around

#ifndef _C_CAMERA_
#define _C_CAMERA_

// Includes
//#include "CGameObject.h"
#include "core/math/Plane.h"
#include "core/math/Rect.h"
#include "core/math/BoundingBox.h"
#include "core/math/matrix/CMatrix.h"

#include "core-ext/transform/CTransform.h"

//#include "renderer/logic/CLogicObject.h"

//#include "GLCommon.h"

//#include <bitset>
//using std::bitset;

class CRenderTexture;

// Enumeration for type of camera
enum eCameraClassType
{
	CAMERA_TYPE_NORMAL,
	CAMERA_TYPE_RT,
	CAMERA_TYPE_RT_CASCADE,
	CAMERA_TYPE_RT_CUBE
};

// ==Defines==
// A define to allow for a function to differentiate between different kinds of cameras
#define CameraType(a) public: virtual eCameraClassType GetType ( void ) { return a; };

// Class Definition
class CCamera// : public CLogicObject// : public CGameObject
{
	//ClassName( "CameraBase" );
	CameraType( CAMERA_TYPE_NORMAL );
public:
	// Constructor/Destructor
	RENDER_API		CCamera ( void );
	RENDER_API		~CCamera ( void );

	// Update Last
	RENDER_API virtual void	LateUpdate ( void );

	// ======
	// === Rendering Queries ===
	// ======
	
	// == Main Render Routine ==
	RENDER_API virtual void	RenderScene ( void );

	// == Set Render Position ==
	RENDER_API virtual void	UpdateMatrix ( void );
	RENDER_API virtual void	RenderSet ( void );
	RENDER_API virtual void	RenderUnset ( void ) {}
	
	// == Property Setters ==
	// Sets active camera.
	// Note that set active is only valid if the camera is not an RT camera.
	RENDER_API void	SetActive ( void );
	// Sets the rotation.
	// Since cameras in OpenGL do things via a ZYX rotation, this function takes care of the dirty bits.
	// Takes angles where X is roll, Y is pitch, and Z is yaw.
	RENDER_API void	SetRotation ( const Rotator& );

	// == Property Queries ==
	// Grab privately generated camera index
	RENDER_API short GetCameraIndex ( void ) { return cameraIndex; }
	// Get if should render with camera or not
	RENDER_API bool	GetRender ( void ) { return (bNeedsUpdate&&active); }
	// Get if is RT camera. Yes, all cameras are technically RT cameras.
	// If a camera returns true to this query, then are not permitted to the final screen output,
	// and may only render to their target render texture.
	RENDER_API bool	IsRTCamera ( void ) { return bIsRTcamera; }

	// ======
	// === Mathematical + Game Queries ===
	// ======

	// == Visibility Queries ==
	RENDER_API bool	PointIsVisible ( Vector3d const& );
	RENDER_API char	SphereIsVisible ( Vector3d const&, ftype );
	RENDER_API bool	BoundingBoxIsVisible ( BoundingBox& );

	// == Transform Queries ==
	RENDER_API Vector3d	GetUp ( void ) { return up; };

	// == Coordinate Queries ==
	RENDER_API Vector3d	WorldToScreenPos ( const Vector3d & ) const;
	RENDER_API Vector3d	ScreenToWorldDir ( const Vector2d & ) const;
	RENDER_API Vector3d	ScreenToWorldPos ( const Vector2d & ) const;

	// == Public Getters
	RENDER_API CRenderTexture* GetRenderTexture ( void ) { return m_renderTexture; }

	//eCameraClassType	GetType ( void ) { return m_type; }
private:
	// == Camera Updating ==
	// Private routines
	void			CameraUpdate ( void );
	void			UpdateFrustum ( void );

public:
	// == Public Options ==
	bool		active;
	Transform	transform;
	// Render options
	Real		zNear;
	Real		zFar;
	Real		fov;
	Real		render_scale;
	// Viewport options
	Rect		viewport_percent;
	bool		mirror_view;
	// Orthographic options
	bool		orthographic;
	Vector2d	ortho_size;

	// Draw modes
	bool		shadowCamera;
	//bool		clearColor;
	//bool		clearDepthAfterLayer;

	bool		layerVisibility [5];

	// External states
	ftype		focalDistance;

	// Drawn layers (will skip objects with layers that don't match)
	// Not matching is somewhat of a misleading description.
	// A camera may have only RL_WORLD, but will still render objects only if RL_ALL and RL_WORLD are on the object.
	// If a camera has RL_ALL, then EVERYTHING gets rendered.
	//bitset<32>	layers;
	//bitset<32>	hin

	// Set bit means to enable a layer. By default, most layers are on.
	uint32_t		enabledHints;

	// Current camera transform
	Matrix4x4 viewTransform;
	Matrix4x4 projTransform;
	Matrix4x4 textureMatrix;

	// == Static Values ==
	// During Render, is the current active rendering camera
	// During Update, is the renderer of the main viewport
	RENDER_API static CCamera* activeCamera;

	// Vector of active cameras
	RENDER_API static std::vector<CCamera*>	vCameraList;

protected:
	// == Camera resultant properties ==
	// Face vectors
	Vector3d up;
	Vector3d forward;
	// Viewport options
	Rect viewport;

	// Camera frustum
	Maths::Plane frustum [6];

	// == Camera type properties ==
	bool	bIsRTcamera;
	bool	bNeedsUpdate;

	// == Camera RT (since engine is default buffered) ==
	CRenderTexture*	m_renderTexture;

protected:
	//eCameraClassType	m_type;

private:
	// == Private states ==
	short cameraIndex;

};

#endif