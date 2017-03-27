
// Includes
//#include "CRenderableObject.h"
#include "DirectionalLight.h"
#include "core/settings/CGameSettings.h"

#include "renderer/state/Settings.h"
#include "renderer/camera/CRTCamera.h"
#include "renderer/camera/CRTCameraCascade.h"

#include "renderer/system/glMainSystem.h"
//#include "CRenderTexture.h"

// Constructor
DirectionalLight::DirectionalLight ( void )
	: CLight()
{
	shadowResolution = CGameSettings::Active()->i_ro_ShadowMapResolution;//512;
	shadowRange = 400;

	isDirectional = true;

	vLightDir.alpha = 0;
}

// Different update
/*void DirectionalLight::Update ( void )
{
	CLight::Update();

	if ( CCamera::activeCamera != NULL )
	{
		// Set position to the main scene's camera
		transform.position = CCamera::activeCamera->transform.position;
		// Offset by the focal distance (to a point)
		transform.position += CCamera::activeCamera->transform.Forward() * std::min<ftype>( CCamera::activeCamera->focalDistance*0.4f, shadowRange*0.15f );
	}
	else
	{
		// If there's no camera, set to the last known camera position
		transform.position = vCameraPos;
	}
}

void DirectionalLight::LateUpdate ( void )
{
	// Update shadows
	UpdateShadows();
}*/
void DirectionalLight::PreStepSynchronus ( void )
{
	CLight::PreStepSynchronus();

	if ( CCamera::activeCamera != NULL )
	{
		// Set position to the main scene's camera
		transform.position = CCamera::activeCamera->transform.position;
		// Offset by the focal distance (to a point)
		transform.position += CCamera::activeCamera->transform.Forward() * std::min<ftype>( CCamera::activeCamera->focalDistance*0.4f, shadowRange*0.15f );
	}
	else
	{
		// If there's no camera, set to the last known camera position
		transform.position = vCameraPos;
	}

	// Update shadows
	UpdateShadows();
}

// Different lateupdate
void DirectionalLight::UpdateLight ( void )
{
	if ( (int)lightIndex < (renderer::Settings.maxLights-1) )
	{
		//int i_glLightIndex = GL_LIGHT1+(int)lightIndex;
		
		if (!CGameSettings::Active()->b_ro_EnableShaders)
		{
			/*glLightfv( i_glLightIndex, GL_DIFFUSE, diffuseColor.start_point() );	
			glLightfv( i_glLightIndex, GL_POSITION, vLightDir.start_point() );
			glLightf( i_glLightIndex, GL_CONSTANT_ATTENUATION, attenuation.blue );
			glLightf( i_glLightIndex, GL_LINEAR_ATTENUATION, attenuation.green );
			glLightf( i_glLightIndex, GL_QUADRATIC_ATTENUATION, attenuation.red );
			glEnable( i_glLightIndex );*/
		}
	}
}

// Different vector in the shader
Color DirectionalLight::GetSendVector ( void )
{
	return vLightDir;
}

// Special update code
void DirectionalLight::UpdateShadowCamera ( void )
{
	//shadowCamera->clearDepthAfterLayer = false;
	shadowCamera->orthographic = true;
	shadowCamera->ortho_size = Vector2d( shadowRange,shadowRange );
	shadowCamera->zNear = 1.0f;
	shadowCamera->zFar = shadowRange*2.5f;
	shadowCamera->fov = 70;

	Vector3d ds = Vector3d( vLightDir.red,vLightDir.green,vLightDir.blue ).normal();
	shadowCamera->transform.position = transform.position + ds*shadowRange*1.5f;
	ds = -ds;
	shadowCamera->transform.rotation = Vector3d(
		0,
		(ftype)radtodeg( atan2(ds.z, sqrt((ds.x * ds.x) + (ds.y * ds.y))) ),
		-(ftype)radtodeg( atan2(ds.y, ds.x) )
		);
	ds = -ds;

	// Now, generate the positions of the shadow cascades
	Vector3d offsetDirection = CCamera::activeCamera->transform.Forward();
	Real cascadeDistance;
	cascadeDistance = (shadowRange/64.0F) * 0.433f;
	cascadeDistance += std::min<Real>( cascadeDistance*0.5f, std::max<Real>( CCamera::activeCamera->focalDistance-3.0f, 0 ) );
	((CRTCameraCascade*)shadowCamera)->m_renderPositions[3] = CCamera::activeCamera->transform.position + offsetDirection*cascadeDistance + ds*shadowRange*1.5f;
	cascadeDistance += (shadowRange/16.0F) * 0.433f;
	((CRTCameraCascade*)shadowCamera)->m_renderPositions[2] = CCamera::activeCamera->transform.position + offsetDirection*cascadeDistance + ds*shadowRange*1.5f;
	cascadeDistance += (shadowRange/4.0F) * 0.433f;
	((CRTCameraCascade*)shadowCamera)->m_renderPositions[1] = CCamera::activeCamera->transform.position + offsetDirection*cascadeDistance + ds*shadowRange*1.5f;
	cascadeDistance += (shadowRange/1.0F) * 0.433f;
	((CRTCameraCascade*)shadowCamera)->m_renderPositions[0] = CCamera::activeCamera->transform.position + offsetDirection*cascadeDistance + ds*shadowRange*1.5f;

	shadowCamera->transform.position = ((CRTCameraCascade*)shadowCamera)->m_renderPositions[3];

	// Lock the texels on the shadow map
	/*Vector3d pos = shadowCamera->transform.position;
	pos = (!shadowCamera->transform.rotation) * pos;

	Real worldTexelSize = ((shadowRange * 2) / shadowResolution) / 2;
	pos.x = fmod(pos.x, worldTexelSize);
	pos.y = fmod(pos.y, worldTexelSize);
	pos.z = fmod(pos.z, worldTexelSize);

	pos = (shadowCamera->transform.rotation) * pos;

	shadowCamera->transform.position -= pos;*/
	/*for ( uint i = 0; i < 4; ++i ) 
	{
		Vector3d t_pos = ((CRTCameraCascade*)shadowCamera)->m_renderPositions[i];
		t_pos = (!shadowCamera->transform.rotation) * t_pos; 

		Real worldTexelSize = ((shadowRange/pow(4,i)) / shadowResolution);
		t_pos.x = fmod(t_pos.x, worldTexelSize);
		t_pos.y = fmod(t_pos.y, worldTexelSize);
		t_pos.z = fmod(t_pos.z, worldTexelSize);

		t_pos = (shadowCamera->transform.rotation) * t_pos;

		((CRTCameraCascade*)shadowCamera)->m_renderPositions[i] -= t_pos;
	}*/
}