
#include <cmath>

#include "core/system/Screen.h"
#include "core/settings/CGameSettings.h"
#include "renderer/light/CLight.h"
#include "renderer/types/ObjectSettings.h"

#include "CCamera.h"

CCamera*				CCamera::activeCamera	= NULL;
std::vector<CCamera*>	CCamera::vCameraList;

CCamera::CCamera ( void )
{
	active = true;

	// Set up camera renderer
	transform.name = "Camera Transform";
	transform.owner = this;
	transform.ownerType = CTransform::TYPE_RENDERER_CAMERA;

	zNear = 0.1f;
	zFar = CGameSettings::Active()->f_ro_DefaultCameraRange;
	fov = 100.0f;

	up = Vector3d( 0,0,1 );
	forward = Vector3d( 1,0,0 );

	viewport_percent.pos = Vector2d( 0,0 );
	viewport_percent.size = Vector2d( 1,1 );

	orthographic = false;
	ortho_size = Vector2d( 512,512 );
	mirror_view = false; // Do not mirror world

	//m_type = CAMERA_TYPE_NORMAL;

	// On Default, not a RT camera
	bIsRTcamera = false;
	// On Default, always update
	bNeedsUpdate = true;
	// On Default, not a shadow camera
	shadowCamera = false;
	// On Default, clears background
	clearColor = true;
	// On Default, clears depth buffer after every draw layer
	clearDepthAfterLayer = true;

	// Set default render layer mode
	for ( uint i = 0; i <= Renderer::V2D; ++i ) {
		layerVisibility[i] = true;
	}

	// Default 0 focal distance
	focalDistance = 0;

	// Default no render target
	m_renderTexture = NULL;

	// Set default layer mode
	/*for ( char i = 0; i < 16; ++i )
		layers[i] = false;
	layers[RL_WORLD] = true;*/

	// The following modes are the hints that will be rendered. All other hints will be ignored.
	enabledHints = 0;
	enabledHints |= RL_WORLD;

	//enabledHints |= RL_SHADOW_COLOR;
	/*enabledHints |= RL_WARP;
	enabledHints |= RL_GLOW;
	enabledHints |= RL_SKYGLOW;
	enabledHints |= RL_WARP;*/
	
	// Set default ignore mode
	/*ignoreMode = false;
	drawOnlyWorld = false;
	drawOnlySolid = false;*/

	// Assign static states
	/*if ( activeCamera == NULL )
		activeCamera = this;*/
	// Grab a camera index
	cameraIndex = 0;
	bool btIndexExists;
	do
	{
		btIndexExists = false;
		for ( unsigned int i = 0; i < vCameraList.size(); ++i )
		{
			if ( cameraIndex == vCameraList[i]->GetCameraIndex() )
			{
				++cameraIndex;
				btIndexExists = true;
			}
		}
	} while ( btIndexExists );
	// Add camera to the list
	vCameraList.push_back( this );

	// Reorder the list
	/*for ( unsigned int i = 1; i < vCameraList.size(); ++i )
	{
		if ( vCameraList[i] == activeCamera )
		{
			CCamera* temp = vCameraList[0];
			vCameraList[0] = activeCamera;
			vCameraList[i] = temp;
		}
	}*/

	// Mark as active if there is no active camera
	if ( activeCamera == NULL ) {
		activeCamera = this;
	}
	// Output camera creation
	std::cout << "New camera with index " << cameraIndex << " created. Main scene: " << ((activeCamera==this) ? "yes" : "no") << std::endl;

}
CCamera::~CCamera ( void )
{
	// Reset active camera
	if ( activeCamera == this ) {
		activeCamera = NULL;
	}

	// Remove camera from the list
	for ( std::vector<CCamera*>::iterator it = vCameraList.begin(); it != vCameraList.end();  )
	{
		if ( (*it) == this )
		{
			vCameraList.erase( it );
			it = vCameraList.end();
		}
		else
		{
			++it;
		}
	}
}

void CCamera::SetActive ( void )
{
	if ( activeCamera != this ) {
		activeCamera = this;
	}
}

void CCamera::SetRotation ( const Rotator& newRotation )
{
	transform.rotation = newRotation;
}

//#include "CDebugDrawer.h"
//#include "CVoxelTerrain.h"

void CCamera::LateUpdate ( void )
{
	// Update the main active camera (meaning, look through the camera pointers if the current value is invalid)
	if ( ( activeCamera == NULL ) || (( activeCamera != NULL )&&(( !activeCamera->active )||( activeCamera->bIsRTcamera ))) )
	{
		activeCamera = this;
		// Now, look for a valid camera
		if ( activeCamera->bIsRTcamera )
		{
			activeCamera = NULL;
			// Loop through all the cameras and find the first valid one
			for ( unsigned int i = 0; i < vCameraList.size(); ++i )
			{
				if (( vCameraList[i] )&&( !(vCameraList[i]->bIsRTcamera) )&&( vCameraList[i]->active ))
				{
					// Assign found camera as current
					activeCamera = vCameraList[i];
					activeCamera->bNeedsUpdate = true;
					// Break out
					i = vCameraList.size();
				}
			}
		}
	}

	// Rotate the move vector to match the camera
	Matrix4x4 rotMatrix;
	rotMatrix.setRotation( transform.rotation );

	// Rotate the move vector to match the camera
	forward = rotMatrix*Vector3d(1,0,0);

	/*up.z = cos( (ftype)degtorad( transform.rotation.y ) );
	up.x = -cos( (ftype)degtorad( transform.rotation.z ) ) * sin( (ftype)degtorad( transform.rotation.y ) );
	up.y = -sin( (ftype)degtorad( transform.rotation.z ) ) * sin( (ftype)degtorad( transform.rotation.y ) );*/
	up = rotMatrix*Vector3d(0,0,1);
	
	// Wireframe mode
	//glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

	// Update viewport
	viewport.pos.x = viewport_percent.pos.x * Screen::Info.width;
	viewport.pos.y = viewport_percent.pos.y * Screen::Info.height;
	viewport.size.x = viewport_percent.size.x * Screen::Info.width;
	viewport.size.y = viewport_percent.size.y * Screen::Info.height;
}

#include "renderer/state/CRenderState.h"
#include "renderer/system/glMainSystem.h"
#include "renderer/texture/CRenderTexture.h"
#include "renderer/texture/CMRTTexture.h"

// == Main Render Routine ==
void CCamera::RenderScene ( void )
{
	GL_ACCESS;
	RenderSet();
	GL.pushProjection( viewTransform * projTransform );

	if ( CGameSettings::Active()->i_ro_RendererMode == RENDER_MODE_FORWARD )
	{
		SceneRenderer->RenderSceneForward(enabledHints);
	}
	else if ( CGameSettings::Active()->i_ro_RendererMode == RENDER_MODE_DEFERRED )
	{
		// Set up the MRT render target
		if ( !bIsRTcamera )
		{
			CRenderTexture* s_buf = GL.GetMainScreenBuffer();

			if ( m_renderTexture && m_renderTexture->GetSize() != s_buf->GetSize() )
			{
				delete m_renderTexture;
				m_renderTexture = NULL;
			}
			if ( !m_renderTexture )
			{
				eColorFormat	colorMode	= SceneRenderer->GetSettings().mainColorAttachmentFormat;
				eDepthFormat	depthMode	= SceneRenderer->GetSettings().mainDepthFormat;
				eStencilFormat	stencilMode	= SceneRenderer->GetSettings().mainStencilFormat;
				uint			colorCount	= SceneRenderer->GetSettings().mainColorAttachmentCount;

				glTexture		depthTexture = SceneRenderer->GetDepthTexture();
				glTexture		stencilTexture = SceneRenderer->GetStencilTexture();

				glTexture textureRequests [4];
				memset( textureRequests, 0, sizeof(glTexture) * 4 );
				textureRequests[0].format = RGBA8;
				textureRequests[1].format = RGBA16F;
				textureRequests[2].format = RGBA8;
				textureRequests[3].format = RGBA8;

				m_renderTexture = new CMRTTexture(
					s_buf->GetWidth(), s_buf->GetHeight(),
					Clamp, Clamp,
					textureRequests + 0, 4,
					&depthTexture, depthTexture.format != DepthNone,
					&stencilTexture, stencilTexture.format != StencilNone );
			}
		}
		// Render now, with this camera set, and the render texture set up
		SceneRenderer->RenderSceneDeferred(enabledHints);
	}
	else
	{
		throw Core::InvalidCallException();
	}
	GL.popProjection();
	RenderUnset();

	GL.CheckError();
}


void CCamera::RenderSet ( void )
{
	GL_ACCESS GL.setupViewport( (int)viewport.pos.x, (int)viewport.pos.y, (int)viewport.size.x, (int)viewport.size.y );

	//CameraUpdate();
	UpdateFrustum();

	CLight::UpdateLights();
	//glGetError();// clear error
}

void CCamera::UpdateMatrix ( void )
{
	if ( orthographic )
	{
		projTransform = Matrix4x4();

		// Define constants
		float left		= -ortho_size.x/2;
		float width		= ortho_size.x;
		float top		= -ortho_size.y/2;
		float height	= ortho_size.y;
		float minz		= zNear;
		float maxz		= zFar;

		// Build the orthographic projection first
		const bool flipped = mirror_view;
		float x_max, y_max, z_max;
		x_max = width; //right-left
		y_max = height; //top-bottom
		z_max = maxz-minz;

		projTransform[0][0] = 2/x_max;
		projTransform[0][1] = 0;
		projTransform[0][2] = 0;
		projTransform[0][3] = 0;

		projTransform[1][0] = 0;
		projTransform[1][1] = (flipped? (-2/y_max) : (2/y_max));
		projTransform[1][2] = 0;
		projTransform[1][3] = 0;

		projTransform[2][0] = 0;
		projTransform[2][1] = 0;
		projTransform[2][2] = -2/(z_max);
		projTransform[2][3] = 0;

		projTransform[3][0] = (left+left+width)/(-x_max);
		projTransform[3][1] = (flipped? (1) : (-1)) * ((top+top+height)/(y_max));
		projTransform[3][2] = (minz+maxz)/(-z_max);
		projTransform[3][3] = 1;
	}
	else
	{
		projTransform = Matrix4x4();

		// Define constants
		const Real f = Real( 1.0 / tan( degtorad(fov)/2 ) );

		// Build the perspective projection first
		projTransform.pData[0] = f/(viewport.size.x/viewport.size.y);
		projTransform.pData[1] = 0;
		projTransform.pData[2] = 0;
		projTransform.pData[3] = 0;

		projTransform.pData[4] = 0;
		projTransform.pData[5] = f;
		projTransform.pData[6] = 0;
		projTransform.pData[7] = 0;

		projTransform.pData[8] = 0;
		projTransform.pData[9] = 0;
		projTransform.pData[10]= (zFar+zNear)/(zNear-zFar);
		projTransform.pData[11]= mirror_view ? 1.0f : -1.0f;

		projTransform.pData[12]= 0;
		projTransform.pData[13]= 0;
		projTransform.pData[14]= (2*zFar*zNear)/(zNear-zFar);
		projTransform.pData[15]= 0;
	}

	// Switch to model view
	{
		Matrix4x4 rotMatrix;
		rotMatrix.setRotation( transform.rotation );

		// Rotate the move vector to match the camera
		forward = rotMatrix*Vector3d(1,0,0);
		up = rotMatrix*Vector3d(0,0,1);
	}
	Vector3d side = forward.cross(up);

	// Build the view transform matrix
	viewTransform[0][0] = side.x;
	viewTransform[0][1] = side.y;
	viewTransform[0][2] = side.z;
	viewTransform[0][3] = 0;

	viewTransform[1][0] = up.x;
	viewTransform[1][1] = up.y;
	viewTransform[1][2] = up.z;
	viewTransform[1][3] = 0;

	viewTransform[2][0] = -forward.x;
	viewTransform[2][1] = -forward.y;
	viewTransform[2][2] = -forward.z;
	viewTransform[2][3] = 0;

	viewTransform[3][0] = 0;
	viewTransform[3][1] = 0;
	viewTransform[3][2] = 0;
	viewTransform[3][3] = 1;

	viewTransform = !viewTransform;
	Matrix4x4 translation;
	translation.setTranslation( -transform.position );
	viewTransform = (!translation) * viewTransform;
}

void CCamera::CameraUpdate ( void )
{
	/*glPopMatrix();
	glPopMatrix();

	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix

	if ( orthographic )
	{
		glOrtho( -ortho_size.x/2, ortho_size.x/2, -ortho_size.y/2,ortho_size.y/2, zNear, zFar ); 
	}
	else
	{
		gluPerspective(fov,Screen::Info.aspect,zNear,zFar); // Change the perspective
	}

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glMultMatrixf( viewTransform.pData );
	
	glPushMatrix();
	glPushMatrix();*/
	// TODO: SET UP VIEWTRANSFORM
}

void CCamera::UpdateFrustum ( void )
{
	//glPushMatrix();
	//glLoadIdentity();
	
	float projection[16];
	/*float MV[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, MV);
	float PM[16];
	glGetFloatv(GL_PROJECTION_MATRIX, PM);
	glPopMatrix();	*/

	Matrix4x4 matxMV = viewTransform;//Matrix4x4( MV );
	Matrix4x4 matxPM = projTransform;//Matrix4x4( PM );
	Matrix4x4 result = matxMV*matxPM;
	//result = result.transpose();

	for ( char i = 0; i < 16; i += 1 )
		projection[i] = result.pData[i];

	// Left
	frustum[0].n.x = projection[3] + projection[0];
    frustum[0].n.y = projection[7] + projection[4];
    frustum[0].n.z = projection[11] + projection[8];
    frustum[0].d = projection[15] + projection[12];

	// Right
    frustum[1].n.x = projection[3] - projection[0];
    frustum[1].n.y = projection[7] - projection[4];
    frustum[1].n.z = projection[11] - projection[8];
    frustum[1].d = projection[15] - projection[12];

	// Bottom
    frustum[2].n.x = projection[3] + projection[1];
    frustum[2].n.y = projection[7] + projection[5];
    frustum[2].n.z = projection[11] + projection[9];
    frustum[2].d = projection[15] + projection[13];

	// Top
    frustum[3].n.x = projection[3] - projection[1];
    frustum[3].n.y = projection[7] - projection[5];
    frustum[3].n.z = projection[11] - projection[9];
    frustum[3].d = projection[15] - projection[13];

	// Near
    frustum[4].n.x = projection[3] + projection[2];
    frustum[4].n.y = projection[7] + projection[6];
    frustum[4].n.z = projection[11] + projection[10];
    frustum[4].d = projection[15] + projection[14];

	// Far
    frustum[5].n.x = projection[3] - projection[2];
    frustum[5].n.y = projection[7] - projection[6];
    frustum[5].n.z = projection[11] - projection[10];
    frustum[5].d = projection[15] - projection[14];
}

bool CCamera::PointIsVisible ( Vector3d const& point )
{
	// various distances
	float fDistance;

	// calculate our distances to each of the planes
	for ( char i = 0; i < 6; i += 1 )
	{
		// find the distance to this plane
		//fDistance = m_plane[i].Normal().dotProduct(refSphere.Center())+m_plane[i].Distance();
		fDistance = frustum[i].n.dot( point ) + frustum[i].d;

		// if this distance is < -sphere.radius, we are outside
		if ( fDistance < 0 )
			return false;
	}

	return true;
}

// tests if a sphere is within the frustrum
char CCamera::SphereIsVisible( Vector3d const& point, ftype radius )
{
	// various distances
	float fDistance;

	// calculate our distances to each of the planes
	for ( int i = 0; i < 6; ++i )
	{
		// find the distance to this plane
		fDistance = frustum[i].n.dot( point ) + frustum[i].d;

		// if this distance is < -sphere.radius, we are outside
		if ( fDistance < -radius )
			return 0;

		// else if the distance is between +- radius, then we intersect
		if( (float)fabs(fDistance) < radius )
			return 1;
	}

	// otherwise we are fully in view
	return 2;
}

bool CCamera::BoundingBoxIsVisible ( BoundingBox& bbox )
{
	bool bboxBehind;
	// calculate our collisions to each of the planes
	for ( int i = 0; i < 6; ++i )
	{
		// find the distance to this plane
		//fDistance = frustum[i].n.dot( point ) + frustum[i].d;
		bboxBehind = bbox.BoxOutsidePlane( frustum[i].n, Vector3d( 0,0, -frustum[i].d ) );

		if ( bboxBehind )
			return false;
	}
	return true;
}

// Coverts world coordinates to screen coordinates of this camera
// Z coordinate indicates depth
Vector3d CCamera::WorldToScreenPos ( const Vector3d & worldPos ) const
{
	//Matrix4x4 temp = (viewTransform * projTransform);
	/*Matrix4x4 temp = viewTransform;
	temp *= projTransform;
	Vector4d screen = Vector4d( worldPos ) * temp;*/
	//Vector4d screen = projTransform * ( viewTransform * Vector4d( worldPos ) );
	//Vector4d screen = (!(projTransform * viewTransform)  * Vector4d( worldPos );
	Vector4d screen = Vector4d( worldPos ).rvrMultMatx( viewTransform * projTransform );
	screen /= screen.w;

	//cout << screen << endl;

	screen.x = screen.x * 0.5f + 0.5f;
	screen.y = -screen.y * 0.5f + 0.5f;

	return Vector3d( screen );
}

// Converts screen coordinates to a direction to the point on the screen.
Vector3d CCamera::ScreenToWorldDir ( const Vector2d & screenPos ) const
{
	// TODO: Test if this still works.

	// last, rotate vector by current camera rotation
	Vector4d screen = Vector4d( screenPos.x*2.0f - 1, -screenPos.y*2.0f + 1, 1, 1 );
	//screen = ( screen * viewTransform.inverse() ) * projTransform.inverse();
	//screen = viewTransform.inverse() * ( projTransform.inverse() * screen );
	screen = (!viewTransform).inverse() * ( (!projTransform).inverse() * screen );
	//screen = !viewTransform * ( !projTransform * screen );
	screen /= screen.w;

	return (Vector3d( screen.x,screen.y,screen.z ) - transform.position).normal();
}

// Converts screen coordinates to the point on the screen the mouse is over.
Vector3d CCamera::ScreenToWorldPos ( const Vector2d & screenPos ) const
{
	// TODO: Test if this still works.

	// last, rotate vector by current camera rotation
	/*Vector4d screen = Vector4d( screenPos.x*2.0f - 1, -screenPos.y*2.0f + 1, 1, 1 );
	//screen = ( screen * viewTransform.inverse() ) * projTransform.inverse();
	//screen = viewTransform.inverse() * ( projTransform.inverse() * screen );
	screen = (!viewTransform).inverse() * ( (!projTransform).inverse() * screen );
	//screen = !viewTransform * ( !projTransform * screen );
	//screen /= screen.w;

	return Vector3d( screen.x,screen.y,screen.z );*/

	/*Vector4d screen = Vector4d( screenPos.x*2.0f - 1, -screenPos.y*2.0f + 1, 1, 1 );
	//Vector4d worldPos = screen.rvrMultMatx( (viewTransform * projTransform).inverse() );
	Vector4d worldPos = (viewTransform * projTransform).inverse() * screen;

	return Vector3d( worldPos.x,worldPos.y,worldPos.z );*/

	Vector4d screen = Vector4d( screenPos.x*2.0f - 1, -screenPos.y*2.0f + 1, 1, 1 );

	Vector3d world ( screen.x * ortho_size.x * 0.5F, -screen.y * ortho_size.y * 0.5F, 0.0F );

	return world + transform.position;
}

