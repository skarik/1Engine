#include "RrLight.h"
#include "renderer/camera/RrCamera.h"
//#include "CRenderableObject.h"

//#include "renderer/camera/RrRTCamera.h"
//#include "renderer/camera/RrRTCameraCascade.h"
//#include "renderer/texture/RrRenderTexture.h"
//#include "renderer/system/glMainSystem.h"

// Render settings
#include "renderer/state/Settings.h"
#include "core/settings/CGameSettings.h"

// Effects
//#include "renderer/material/RrMaterial.h"
//#include "renderer/object/shapes/CBillboard.h"

// Sorting and finding
#include <algorithm>

// Usings
using std::vector;
using std::find;
using std::priority_queue;

// Static vars
vector<RrLight*>												RrLight::lightList;
//vector<RrLight*>												RrLight::activeLightList;
//priority_queue<RrLight*, vector<RrLight*>, RrLightComparison>	RrLight::activeLightQueue;

//bool		RrLight::bListReset		= true;
//Vector3d	RrLight::vCameraPos		= Vector3d( 0,0,0 );

// Light comparison.
// Returns true if light1 should come before light2
bool RrLightComparison::operator() ( RrLight* light1, RrLight* light2 )
{
	// Make sure active lights are first in the listing
	if (light1->active != light2->active)
		return light1->active;
	
	// Make sure directional lights come in first
	if (light1->type != light2->type)
		return (light1->type == kLightTypeDirectional);

	// If directional...
	if (light1->type == kLightTypeDirectional)
	{
		// Sort by the brightness:
		Real lum1 = light1->color.red * .299F + light1->color.green * .587F + light1->color.blue * .114F;
		Real lum2 = light2->color.red * .299F + light2->color.green * .587F + light2->color.blue * .114F;
		// Brighter is first.
		return lum1 > lum2;
	}
	// Otherwise...
	else
	{
		// Sort by distance:
		Vector3f delta1 = light1->position - m_camera->transform.position;
		Vector3f delta2 = light2->position - m_camera->transform.position;
		// Closer is first:
		return delta1.sqrMagnitude() < delta2.sqrMagnitude();
	}
}

//// Comparison between light priorities
//// Returns true if pLight2 is higher priority
//bool RrLightComparison::operator() ( RrLight* pLight2, RrLight* pLight1 )
//{
//	if ( pLight1->range <= 0 )
//		return true;
//	if ( pLight2->range <= 0 )
//		return false;
//
//	// Check if both are directional, and sort by brightness
//	if ( pLight1->isDirectional && pLight2->isDirectional ) {
//		Real l1, l2;
//		l1 = pLight1->diffuseColor.red * .299f + pLight1->diffuseColor.green * .587f + pLight1->diffuseColor.blue * .114f;
//		l2 = pLight2->diffuseColor.red * .299f + pLight2->diffuseColor.green * .587f + pLight2->diffuseColor.blue * .114f;
//		return (l1>l2);
//	}
//	else if ( pLight1->isDirectional )
//		return true;
//	else if ( pLight2->isDirectional )
//		return false;
//
//	// Else, sort by distance
//	Vector3d vDist1 = pLight1->m_position - RrLight::vCameraPos;
//	Vector3d vDist2 = pLight2->m_position - RrLight::vCameraPos;
//	if ( vDist1.sqrMagnitude() > vDist2.sqrMagnitude() )
//	{
//		return false;
//	}
//	return true;
//}
//// Returns true if pLight1 is higher priority
//bool RrLightComparisonExternal::operator() ( RrLight* pLight1, RrLight* pLight2 )
//{
//	if ( pLight1->range <= 0 )
//		return true;
//	if ( pLight2->range <= 0 )
//		return false;
//
//	// Check if both are directional, and sort by brightness
//	if ( pLight1->isDirectional && pLight2->isDirectional ) {
//		Real l1, l2;
//		l1 = pLight1->diffuseColor.red * .299f + pLight1->diffuseColor.green * .587f + pLight1->diffuseColor.blue * .114f;
//		l2 = pLight2->diffuseColor.red * .299f + pLight2->diffuseColor.green * .587f + pLight2->diffuseColor.blue * .114f;
//		return (l1>l2);
//	}
//	else if ( pLight1->isDirectional )
//		return true;
//	else if ( pLight2->isDirectional )
//		return false;
//
//	// Else, sort by distance
//	Vector3d vDist1 = pLight1->position - RrLight::vCameraPos;
//	Vector3d vDist2 = pLight2->position - RrLight::vCameraPos;
//	if ( vDist1.sqrMagnitude() > vDist2.sqrMagnitude() )
//	{
//		return false;
//	}
//	return true;
//}

// == Constructors ==
RrLight::RrLight ( void )
	: RrLogicObject()
{
	lightList.push_back( this );

	type			= kLightTypeArea;
	area_type		= kLightAreaTypePoint;

	position		= Vector3f(0, 0, 0);
	direction		= Vector3f(0, 0, -1.0F);
	size			= Vector3f(0.1F, 0.1F, 0.1F);

	color			= Color(1.0F, 1.0F, 1.0F, 1.0F);
	falloff_range	= 500.0F;
	falloff_invpower	= 2.0F;
	falloff_passthru	= 0.0F;

	shadows_enabled	= false;
	shadows_resolution_factor	= 1.0F;

	effect_halo_strength	= 0.0F;

	/*attenuation		= Color( 0.5,0,1,1 );
	vLightPos		= Color( 0,0,0,1 );
	vLightDir		= Color( 0,0,-1,1 );
	isDirectional	= false;

	range	= 6.0f;
	falloff	= 2.0f;
	pass	= 0.0f;

	generateShadows	= false;
	shadowStrength	= 1.0f;
	shadowResolution= 512;

	shadowCamera = NULL;
	shadowTexture = NULL;

	mHalo = NULL;
	drawHalo = false;
	haloStrength = 0.7f;*/
}

// Destructors
RrLight::~RrLight ( void )
{
	vector<RrLight*>::iterator index;
	index = find( lightList.begin(), lightList.end(), this );
	if ( index != lightList.end() )
	{
		lightList.erase( index );
	}

	/*generateShadows = false;
	UpdateShadows();

	delete_safe(mHalo);*/
}

// Update
void RrLight::PreStepSynchronus ( void )
{
	/*if ( drawHalo )
	{
		if ( !mHalo )
		{
			mHalo = new CBillboard;
			mHalo->transform.world.position = position;
			RrMaterial* newMat = new RrMaterial;
			newMat->removeReference();
			newMat->passinfo.push_back( RrPassForward() );
			newMat->passinfo[0].shader = new RrShader( "shaders/d/light_halo.glsl" );
			newMat->passinfo[0].m_blend_mode = renderer::BM_SOFT_ADD;
			newMat->passinfo[0].m_transparency_mode = renderer::ALPHAMODE_TRANSLUCENT;
			mHalo->SetMaterial( newMat );
		}
		else
		{
			mHalo->transform.world.position = position;
			mHalo->GetMaterial()->m_diffuse = diffuseColor;
			mHalo->GetMaterial()->m_diffuse.alpha = range;
			mHalo->GetMaterial()->m_emissive = Color( haloStrength, falloff, pass );
			mHalo->SetSize( haloStrength*range*0.5f );
		}
	}
	else
	{
		delete_safe(mHalo);
	}*/

	//bListReset = true;

	// Update the attentuation based on range
	/*attenuation.blue = 0;
	attenuation.red = 0;
	attenuation.green = 1.0f / ( 0.09f * range * ( 1 - powf( 0.09f, 1.0f/falloff ) ) );

	// Update shadows
	UpdateShadows();*/
}


// Update light list on render
void RrLight::UpdateLights ( RrCamera* rendering_camera )
{
	//UpdateLightList();
	// Loop through all the lights and update
	/*for ( vector<RrLight*>::iterator it = lightList.begin(); it != lightList.end(); ++it )
	{
		(*it)->UpdateLight();
	}*/

	RrLightComparison comparison;
	comparison.m_camera = rendering_camera;
	std::sort(lightList.begin(), lightList.end(), comparison);
}

// Update Light List
//void RrLight::UpdateLightList ( void )
//{
//	//if ( bListReset )
//	{
//		//RrLight* target;
//		//bListReset = false;
//		//vCameraPos = RrCamera::activeCamera->transform.position;
//
//		//if ( !CGameSettings::Active()->b_ro_EnableShaders ) {
//		//	for ( int i = 0; i < (renderer::Settings.maxLights-1); i += 1 ) {
//		//		//glDisable( GL_LIGHT1+i );
//		//	}
//		//}
//
//		/*vector<RrLight*>::iterator index;
//		for ( index = lightList.begin(); index != lightList.end(); index++ )
//		{
//			if ( (*index)->active )
//			{
//				//activeLightQueue.push( *index );
//				activeLightList.push_back( *index );
//			}
//		}*/
//		// Assign light indexes!
//		/*activeLightList.clear();
//		char current_index = 0;
//		while ( !activeLightQueue.empty() )
//		{	
//			target = activeLightQueue.top();
//			pTarget->lightIndex = current_index;
//
//			activeLightQueue.pop();
//			current_index += 1;
//			activeLightList.push_back( pTarget );
//		}*/
//	}
//
//	{
//		//vector<RrLight*>* lightList = RrLight::GetLightList();
//		//vector<RrLight*>* lightList = RrLight::GetActiveLightList();
//		// Sort it
//		//sort( lightList->begin(), lightList->end(), LightComparator );
//	}
//}

// Late update
/*void RrLight::LateUpdate ( void )
{
	bListReset = true;

	// Update the attentuation based on range
	attenuation.blue = 0;
	attenuation.red = 0;
	attenuation.green = 1.0f / ( 0.09f * range * ( 1 - powf( 0.09f, 1.0f/falloff ) ) );

	// Update shadows
	UpdateShadows();
}*/

// Update Lights
/*void RrLight::UpdateLight ( void )
{
	// Push the light positions to the 4-component structure
	vLightPos.red	= position.x;
	vLightPos.green = position.y;
	vLightPos.blue	= position.z;

	if ( (int)lightIndex < (renderer::Settings.maxLights-1) )
	{
		//glLoadIdentity();

		//int i_glLightIndex = GL_LIGHT1+(int)lightIndex;
#		ifdef GL_2_1
		if ( CGameSettings::Active()->b_ro_EnableShaders == false )
		{
			glLightfv( i_glLightIndex, GL_DIFFUSE, diffuseColor.start_point() );	
			glLightfv( i_glLightIndex, GL_POSITION, vLightPos.start_point() );
			glLightf( i_glLightIndex, GL_CONSTANT_ATTENUATION, attenuation.blue );
			glLightf( i_glLightIndex, GL_LINEAR_ATTENUATION, attenuation.green );
			glLightf( i_glLightIndex, GL_QUADRATIC_ATTENUATION, attenuation.red );
			glEnable( i_glLightIndex );
		}
#		endif
	}
}*/
//
//// Update shadows
//void RrLight::UpdateShadows ( void )
//{
//	if ( generateShadows && CGameSettings::Active()->b_ro_EnableShadows )
//	{
//		if ( !shadowTexture )
//		{
//			if ( isDirectional )
//			{
//				shadowTexture = new RrRenderTexture(
//					(unsigned int)(shadowResolution*2), (unsigned int)(shadowResolution/2), 
//					Clamp, Clamp,
//					RGBA8,
//					RrGpuTexture(0,Depth24),true,
//					RrGpuTexture(), false );
//			}
//			else
//			{
//				shadowTexture = new RrRenderTexture(
//					(unsigned int)(shadowResolution), (unsigned int)(shadowResolution), 
//					Clamp, Clamp,
//					RGBA8,
//					RrGpuTexture(0,Depth24),true,
//					RrGpuTexture(), false );
//			}
//		}
//			//shadowTexture = new RrRenderTexture( RGBA8, (unsigned int)shadowResolution, (unsigned int)shadowResolution, Clamp, Clamp, Texture2D, Depth16,true,true );
//		if ( !shadowCamera )
//		{
//			if ( isDirectional ) {
//				shadowCamera = new RrRTCameraCascade ( shadowTexture, 48.0f, true );
//			}
//			else {
//				shadowCamera = new RrRTCamera ( shadowTexture, 48.0f, true );
//			}
//			//shadowCamera = new RrRTCamera ( shadowTexture, 2.0f, true );
//			/*shadowCamera->ignoreMode = true;
//			shadowCamera->drawOnlySolid = true;
//			shadowCamera->drawOnlyWorld = true;
//			shadowCamera->shadowCamera	= true;*/
//		}
//		UpdateShadowCamera();
//	}
//	else
//	{
//		if ( shadowCamera )
//			delete shadowCamera;
//		shadowCamera = NULL;
//		if ( shadowTexture )
//			delete shadowTexture;
//		shadowTexture = NULL;
//	}
//}
//// Update shadow camera
//void RrLight::UpdateShadowCamera ( void )
//{
//	//shadowCamera->transform.Get( transform );
//	if ( shadowCamera != NULL )
//	{
//		shadowCamera->transform.position = position;
//	}
//}
//
//// == Getters ==
//// Return vector to send to shaders
//Color RrLight::GetSendVector ( void )
//{
//	return vLightPos;
//}