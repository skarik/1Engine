#include "core/system/Screen.h"
#include "core/math/Math.h"
#include "core/debug/Console.h"

#include "renderer/light/RrLight.h"
#include "renderer/types/ObjectSettings.h"
#include "renderer/texture/RrRenderTexture.h"

#include "renderer/state/RrRenderer.h"
#include "renderer/texture/RrRenderTexture.h"
#include "renderer/texture/CMRTTexture.h"

#include "renderer/object/postprocess/PostProcessPass.h"

#include "RrCamera.h"

#include <cmath>

RrCamera*				RrCamera::activeCamera	= NULL;
RrCamera*				RrCamera::g_FirstCamera	= NULL;
RrCamera*				RrCamera::g_LastCamera	= NULL;

RrCamera::RrCamera ( bool isTransient )
	: isTransient(isTransient)
{
	up = Vector3f(0, 0, 1);
	forward = Vector3f::forward;

	// Set all layers visible by defeault.
	for ( size_t i = 0; i <= renderer::kRenderLayer_MAX; ++i )
	{
		layerVisibility[i] = true;
	}
	// The following modes are the hints that will be rendered. All other hints will be ignored.
	enabledHints = 0 | kRenderHintBitmaskWorld;

	// Camera list management:
	if (!isTransient)
	{
		if (g_LastCamera != nullptr)
		{
			ARCORE_ASSERT(g_FirstCamera != nullptr);
			g_LastCamera->m_nextCamera = this;
		}
		else
		{
			ARCORE_ASSERT(g_FirstCamera == nullptr);
			g_FirstCamera = this;
		}
		m_prevCamera = g_LastCamera;
		g_LastCamera = this;
	}

	// Mark as active if there is no active camera
	if ( activeCamera == NULL ) {
		activeCamera = this;
	}
	// Output camera creation
	if (!isTransient)
	{
		debug::Console->PrintMessage("New permanent camera created. Main scene: %s\n", (activeCamera == this) ? "yes" : "no");
	}
}

void RrCamera::RemoveFromCameraList ( void )
{
	// Update the first & last cameras
	if (this == g_FirstCamera)
	{
		g_FirstCamera = m_nextCamera;
	}
	if (this == g_LastCamera)
	{
		g_LastCamera = m_prevCamera;
	}

	// Redo links to skip this current one.
	if (m_prevCamera != nullptr)
	{
		m_prevCamera->m_nextCamera = m_nextCamera;
	}
	if (m_nextCamera != nullptr)
	{
		m_nextCamera->m_prevCamera = m_prevCamera;
	}
}

RrCamera::~RrCamera ( void )
{
	// Remove camera from the list
	if (!isTransient)
	{
		RemoveFromCameraList();
	}

	// Reset active camera
	if ( activeCamera == this )
	{
		//activeCamera = NULL;
		activeCamera = g_FirstCamera;
	}
}

void RrCamera::SetActive ( void )
{
	ARCORE_ASSERT(this->GetType() == kCameraClassNormal);

	if ( activeCamera != this )
	{
		// Mark self as the active camera.
		activeCamera = this;

		// We need to now resort the cameras:
		if (this != g_FirstCamera)
		{
			RemoveFromCameraList();

			// Insert into the front of the list:
			if (g_FirstCamera != nullptr)
			{
				g_FirstCamera->m_prevCamera = this;
			}
			m_nextCamera = g_FirstCamera;
			m_prevCamera = nullptr;
			g_FirstCamera = this;
		}
	}
}

void RrCamera::SetRotation ( const Rotator& newRotation )
{
	transform.rotation = newRotation;
}

//#include "RrDebugDrawer.h"
//#include "CVoxelTerrain.h"

void RrCamera::LateUpdate ( void )
{
	// Update the main active camera (meaning, look through the camera pointers if the current value is invalid)
	if ( activeCamera == NULL || !activeCamera->active || activeCamera->GetType() != kCameraClassNormal /*activeCamera->m_renderTexture != NULL*/ )
	{
		// Start off attempting to set this first camera as active camera.
		activeCamera = this;

		// Is this not a valid camera?
		//if ( !activeCamera->active || activeCamera->GetType() != kCameraClassNormal /*activeCamera->m_renderTexture != NULL*/ )
		//{
		//	activeCamera = NULL; // Go back to invalid camera.
		//	// Loop through all the cameras and find the first valid one
		//	for ( unsigned int i = 0; i < m_CameraList.size(); ++i )
		//	{
		//		RrCamera* potentialCamera = m_CameraList[i];
		//		if ( potentialCamera == NULL || !potentialCamera->active || potentialCamera->GetType() != kCameraClassNormal /*potentialCamera->m_renderTexture != NULL*/ )
		//			{}
		//		else
		//		{
		//			// Assign found camera as current
		//			activeCamera = potentialCamera;
		//			// Force an immediate update of the pass
		//			activeCamera->m_needsNewPasses = true;
		//			break;
		//		}
		//	}
		//}
	}

	// Rotate the move vector to match the camera
	Matrix4x4 rotMatrix;
	rotMatrix.setRotation( transform.rotation );

	// Rotate the move vector to match the camera
	forward = rotMatrix * Vector3f::forward;

	/*up.z = cos( (Real)degtorad( transform.rotation.y ) );
	up.x = -cos( (Real)degtorad( transform.rotation.z ) ) * sin( (Real)degtorad( transform.rotation.y ) );
	up.y = -sin( (Real)degtorad( transform.rotation.z ) ) * sin( (Real)degtorad( transform.rotation.y ) );*/
	up = rotMatrix * Vector3f(0, 0, 1);
	
	// Wireframe mode
	//glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

	// Update viewport
	//if (m_renderTexture == NULL)
	/*{
		viewport.pos.x  = viewportPercent.pos.x * Screen::Info.width;
		viewport.pos.y  = viewportPercent.pos.y * Screen::Info.height;
		viewport.size.x = viewportPercent.size.x * Screen::Info.width;
		viewport.size.y = viewportPercent.size.y * Screen::Info.height;
	}*/
	/*else
	{
		viewport.pos.x  = viewportPercent.pos.x * m_renderTexture->GetWidth();
		viewport.pos.y  = viewportPercent.pos.y * m_renderTexture->GetHeight();
		viewport.size.x = viewportPercent.size.x * m_renderTexture->GetWidth();
		viewport.size.y = viewportPercent.size.y * m_renderTexture->GetHeight();
	}*/
}

int RrCamera::PassCount ( void )
{
	return 1;
}
void RrCamera::PassRetrieve ( const rrCameraPassInput* input, rrCameraPass* passList )
{
	ARCORE_ASSERT(input->m_outputInfo != nullptr && input->m_maxPasses > 0);

	passList[0].m_bufferChain	= NULL; // Use the default buffer chain for rendering.
	passList[0].m_passType		= kCameraRenderWorld;
	passList[0].m_viewport		= input->m_outputInfo->GetOutputViewport();
	passList[0].m_viewTransform	= viewTransform;
	passList[0].m_projTransform	= projTransform;
	passList[0].m_viewprojTransform	= viewprojMatrix;

	int cbuffer_index = input->m_bufferingIndex;
	UpdateCBuffer(cbuffer_index, input->m_bufferingCount, &passList[0]);
	passList[0].m_cbuffer = &m_cbuffers[cbuffer_index];

	passList[0].m_graphicsContext = input->m_graphicsContext;
}

void RrCamera::UpdateCBuffer ( const uint index, const uint predictedMax, const rrCameraPass* passinfo )
{
	if (m_cbuffers.size() < predictedMax) {
		m_cbuffers.resize(predictedMax);
	}
	if (!m_cbuffers[index].valid()) {
		m_cbuffers[index].initAsConstantBuffer(NULL, sizeof(renderer::cbuffer::rrPerCamera));
	}

	// Generate structure information to shunt to the GPU...
	renderer::cbuffer::rrPerCamera cameraData = {};
	cameraData.viewProjection = passinfo[0].m_viewprojTransform;
	cameraData.worldCameraPosition = transform.position;
	cameraData.screenSizeScaled = Vector2f(passinfo->m_viewport.size.x * renderScale, passinfo->m_viewport.size.y * renderScale);
	cameraData.screenSize = Vector2f(passinfo->m_viewport.size.x, passinfo->m_viewport.size.y);
	cameraData.pixelRatio = Vector2f(1, 1) * (orthoSize.x / passinfo->m_viewport.size.x);

	// And shunt it to the GPU!
	m_cbuffers[index].upload(NULL, &cameraData, sizeof(renderer::cbuffer::rrPerCamera), gpu::kTransferStream);
}


//// == Main Render Routine ==
//void RrCamera::RenderScene ( void )
//{
//	// Bind camera
//	RenderBegin();
//	//GL.pushProjection( viewTransform * projTransform );
//	camera_VP = viewTransform * projTransform;
//	RrMaterial::pushConstantsPerCamera();
//
//	// Limit render scale between 10% and 100%
//	render_scale = math::clamp( render_scale, 0.1F, 1.0F );
//
//	// Perform rendering
//	auto renderMode = RrRenderer::Active->GetRenderMode();
//	if ( renderMode == kRenderModeForward )
//	{
//		SceneRenderer->RenderSceneForward(enabledHints);
//	}
//	else if ( renderMode == kRenderModeDeferred )
//	{
//		SceneRenderer->RenderSceneDeferred(enabledHints);
//	}
//	else
//	{
//		throw core::InvalidCallException();
//	}
//
//	// Unbind camera
//	//GL.popProjection();
//	RenderEnd();
//}


void RrCamera::RenderBegin ( void )
{
	//auto gfx = graphics_context;//gpu::getDevice()->getContext();

	// TODO: move this to the output settings, rather than have in the camera
	/*ARCORE_ERROR("Move this elsewhere. Cameras are not outputs, so their viewports shouldn't affect anything");
	gfx->setViewport(
		(uint32_t)math::round(viewport.pos.x),
		(uint32_t)math::round(viewport.pos.y),
		(uint32_t)math::round(viewport.pos.x + viewport.size.x), 
		(uint32_t)math::round(viewport.pos.y + viewport.size.y)); */

	//CameraUpdate();
	UpdateFrustum();

	// Update the light listing from this camera:
	RrLight::UpdateLights(this);

	// TODO: Push camera constants now.

	// TODO: Loop through postprocess stack (properly)
	for (size_t i = 0; i < postProcessStack.size(); ++i)
	{
		postProcessStack[i]->RenderBegin(this);
	}
}
void RrCamera::RenderEnd ( void )
{
	// Loop through postprocess stack and call end render.
	for (size_t i = 0; i < postProcessStack.size(); ++i)
	{
		postProcessStack[i]->RenderEnd(this);
	}
}

void RrCamera::UpdateMatrix ( const RrOutputInfo& viewport_info )
{
	if ( orthographic )
	{
		projTransform = Matrix4x4();

		// Define constants
		float left		= -orthoSize.x/2;
		float width		=  orthoSize.x;
		float top		= -orthoSize.y/2;
		float height	=  orthoSize.y;
		float minz		= zNear;
		float maxz		= zFar;

		// Build the orthographic projection first
		float x_max, y_max, z_max;
		x_max = width; //right-left
		y_max = height; //top-bottom
		z_max = maxz - minz;

		projTransform[0][0] = 2 / x_max;
		projTransform[0][1] = 0;
		projTransform[0][2] = 0;
		projTransform[0][3] = 0;

		projTransform[1][0] = 0;
		projTransform[1][1] = (mirrorView? (-2 / y_max) : (2 / y_max));
		projTransform[1][2] = 0;
		projTransform[1][3] = 0;

		projTransform[2][0] = 0;
		projTransform[2][1] = 0;
		//projTransform[2][2] = -2 / (z_max);
		projTransform[2][2] = -1 / (z_max); // We aren't in NDC. We're in 0-1 that is defined by the engine, rather than specs.
		projTransform[2][3] = 0;

		projTransform[3][0] = (left + left + width) / (-x_max);
		projTransform[3][1] = (mirrorView ? 1 : -1) * ((top + top + height) / (y_max));
		//projTransform[3][2] = (minz + maxz) / (-z_max);
		projTransform[3][2] = 0; // We aren't in NDC. We're in 0-1 that is defined by the engine, rather than specs.
		projTransform[3][3] = 1;
	}
	else
	{
		projTransform = Matrix4x4();

		// Define constants
		const rrViewport viewport = viewport_info.GetOutputViewport();
		const Real f = Real( 1.0 / tan( degtorad(fieldOfView)/2 ) );

		// Build the perspective projection first
		projTransform.pData[0] = f / (viewport.size.x / (Real)viewport.size.y);
		projTransform.pData[1] = 0;
		projTransform.pData[2] = 0;
		projTransform.pData[3] = 0;

		projTransform.pData[4] = 0;
		projTransform.pData[5] = f;
		projTransform.pData[6] = 0;
		projTransform.pData[7] = 0;

		projTransform.pData[8] = 0;
		projTransform.pData[9] = 0;
		projTransform.pData[10]= (zFar + zNear) / (zNear - zFar);
		projTransform.pData[11]= mirrorView ? 1.0f : -1.0f;

		projTransform.pData[12]= 0;
		projTransform.pData[13]= 0;
		projTransform.pData[14]= (2 * zFar * zNear) / (zNear - zFar);
		projTransform.pData[15]= 0;
	}

	// Switch to model view
	{
		Matrix4x4 rotMatrix;
		rotMatrix.setRotation( transform.rotation );

		// Rotate the move vector to match the camera
		forward = rotMatrix * Vector3f::forward;
		up = rotMatrix * Vector3f(0,0,1);
	}
	Vector3f side = forward.cross(up);

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

	//
	// Important line below!

	// Update the view-projection
	viewprojMatrix = viewTransform * projTransform;
}

//void RrCamera::CameraUpdate ( void )
//{
//	/*glPopMatrix();
//	glPopMatrix();
//
//	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
//	glLoadIdentity();									// Reset The Projection Matrix
//
//	if ( orthographic )
//	{
//		glOrtho( -ortho_size.x/2, ortho_size.x/2, -ortho_size.y/2,ortho_size.y/2, zNear, zFar ); 
//	}
//	else
//	{
//		gluPerspective(fov,Screen::Info.aspect,zNear,zFar); // Change the perspective
//	}
//
//	glMatrixMode(GL_MODELVIEW);
//	glLoadIdentity();
//	glMultMatrixf( viewTransform.pData );
//	
//	glPushMatrix();
//	glPushMatrix();*/
//	// TODO: SET UP VIEWTRANSFORM
//}

void RrCamera::UpdateFrustum ( void )
{
	/*float projection[16];

	for ( char i = 0; i < 16; i += 1 )
		projection[i] = viewprojMatrix.pData[i];

	// Left
	frustum.plane[0].n.x = projection[3] + projection[0];
	frustum.plane[0].n.y = projection[7] + projection[4];
	frustum.plane[0].n.z = projection[11] + projection[8];
	frustum.plane[0].d = projection[15] + projection[12];

	// Right
	frustum.plane[1].n.x = projection[3] - projection[0];
	frustum.plane[1].n.y = projection[7] - projection[4];
	frustum.plane[1].n.z = projection[11] - projection[8];
	frustum.plane[1].d = projection[15] - projection[12];

	// Bottom
	frustum.plane[2].n.x = projection[3] + projection[1];
	frustum.plane[2].n.y = projection[7] + projection[5];
	frustum.plane[2].n.z = projection[11] + projection[9];
	frustum.plane[2].d = projection[15] + projection[13];

	// Top
	frustum.plane[3].n.x = projection[3] - projection[1];
	frustum.plane[3].n.y = projection[7] - projection[5];
	frustum.plane[3].n.z = projection[11] - projection[9];
	frustum.plane[3].d = projection[15] - projection[13];

	// Near
	frustum.plane[4].n.x = projection[3] + projection[2];
	frustum.plane[4].n.y = projection[7] + projection[6];
	frustum.plane[4].n.z = projection[11] + projection[10];
	frustum.plane[4].d = projection[15] + projection[14];

	// Far
	frustum.plane[5].n.x = projection[3] - projection[2];
	frustum.plane[5].n.y = projection[7] - projection[6];
	frustum.plane[5].n.z = projection[11] - projection[10];
	frustum.plane[5].d = projection[15] - projection[14];*/

	frustum = core::math::Frustum::BuildFromProjectionMatrix(viewprojMatrix);
}

bool RrCamera::PointIsVisible ( Vector3f const& point )
{
	/*
	// various distances
	float fDistance;

	// calculate our distances to each of the planes
	for ( char i = 0; i < 6; i += 1 )
	{
		// find the distance to this plane
		//fDistance = m_plane[i].Normal().dotProduct(refSphere.Center())+m_plane[i].Distance();
		fDistance = frustum.plane[i].n.dot( point ) + frustum.plane[i].d;

		// if this distance is < -sphere.radius, we are outside
		if ( fDistance < 0 )
			return false;
	}

	return true;*/
	return frustum.PointIsInside(point) != core::math::kShapeCheckResultOutside;
}

// tests if a sphere is within the frustrum
bool RrCamera::SphereIsVisible( const Vector3f& center, const Real radius )
{
	// various distances
	/*float fDistance;

	// calculate our distances to each of the planes
	for ( int i = 0; i < 6; ++i )
	{
		// find the distance to this plane
		fDistance = frustum.plane[i].n.dot( point ) + frustum.plane[i].d;

		// if this distance is < -sphere.radius, we are outside
		if ( fDistance < -radius )
			return 0;

		// else if the distance is between +- radius, then we intersect
		if( (float)fabs(fDistance) < radius )
			return 1;
	}

	// otherwise we are fully in view
	return 2;*/
	return frustum.SphereIsInside(center, radius) != core::math::kShapeCheckResultOutside;
}

bool RrCamera::BoundingBoxIsVisible ( const core::math::BoundingBox& bbox )
{
	/*bool bboxBehind;
	// calculate our collisions to each of the planes
	for ( int i = 0; i < 6; ++i )
	{
		// find the distance to this plane
		//fDistance = frustum[i].n.dot( point ) + frustum[i].d;
		bboxBehind = bbox.BoxOutsidePlane( frustum.plane[i].n, Vector3f( 0,0, -frustum.plane[i].d ) );

		if ( bboxBehind )
			return false;
	}
	return true;*/
	return frustum.BoundingBoxIsInside(bbox) != core::math::kShapeCheckResultOutside;
}

// Coverts world coordinates to screen coordinates of this camera
// Z coordinate indicates depth
Vector3f RrCamera::WorldToScreenPos ( const Vector3f & worldPos ) const
{
	//Matrix4x4 temp = (viewTransform * projTransform);
	/*Matrix4x4 temp = viewTransform;
	temp *= projTransform;
	Vector4f screen = Vector4f( worldPos ) * temp;*/
	//Vector4f screen = projTransform * ( viewTransform * Vector4f( worldPos ) );
	//Vector4f screen = (!(projTransform * viewTransform)  * Vector4f( worldPos );
	Vector4f screen = Vector4f( worldPos ).rvrMultMatx( viewTransform * projTransform );
	screen /= screen.w;

	//cout << screen << endl;

	screen.x =  screen.x * 0.5f + 0.5f;
	screen.y = -screen.y * 0.5f + 0.5f;

	return Vector3f( screen );
}

// Converts screen coordinates to a direction to the point on the screen.
Vector3f RrCamera::ScreenToWorldDir ( const Vector2f & screenPos ) const
{
	// TODO: Test if this still works.
	// TODO: fix for ortho. Z seems flipped.

	// last, rotate vector by current camera rotation
	Vector4f screen = Vector4f( screenPos.x*2.0f - 1, -screenPos.y*2.0f + 1, 1, 1 );
	//screen = ( screen * viewTransform.inverse() ) * projTransform.inverse();
	//screen = viewTransform.inverse() * ( projTransform.inverse() * screen );
	screen = (!viewTransform).inverse() * ( (!projTransform).inverse() * screen );
	//screen = !viewTransform * ( !projTransform * screen );
	screen /= screen.w;

	return (Vector3f( screen.x,screen.y,screen.z ) - transform.position).normal();
}

// Converts screen coordinates to the point on the screen the mouse is over.
Vector3f RrCamera::ScreenToWorldPos ( const Vector2f & screenPos ) const
{
	// TODO: Test if this still works.

	// last, rotate vector by current camera rotation
	/*Vector4f screen = Vector4f( screenPos.x*2.0f - 1, -screenPos.y*2.0f + 1, 1, 1 );
	//screen = ( screen * viewTransform.inverse() ) * projTransform.inverse();
	//screen = viewTransform.inverse() * ( projTransform.inverse() * screen );
	screen = (!viewTransform).inverse() * ( (!projTransform).inverse() * screen );
	//screen = !viewTransform * ( !projTransform * screen );
	//screen /= screen.w;

	return Vector3f( screen.x,screen.y,screen.z );*/

	/*Vector4f screen = Vector4f( screenPos.x*2.0f - 1, -screenPos.y*2.0f + 1, 1, 1 );
	//Vector4f worldPos = screen.rvrMultMatx( (viewTransform * projTransform).inverse() );
	Vector4f worldPos = (viewTransform * projTransform).inverse() * screen;

	return Vector3f( worldPos.x,worldPos.y,worldPos.z );*/

	if (!orthographic)
	{
		Vector4f screen = Vector4f( screenPos.x*2.0f - 1, -screenPos.y*2.0f + 1, 1, 1 );
		//Vector4f worldPos = screen.rvrMultMatx( (viewTransform * projTransform).inverse() );
		Vector4f worldPos = (viewTransform * projTransform).inverse() * screen;

		return Vector3f( worldPos.x,worldPos.y,worldPos.z );
	}
	else
	{
		Vector4f screen = Vector4f( screenPos.x*2.0f - 1, -screenPos.y*2.0f + 1, 1, 1 );
		Vector3f world ( screen.x * orthoSize.x * 0.5F, -screen.y * orthoSize.y * 0.5F, 0.0F );
		return world + transform.position;
	}
}

