
#include "DayAndNightCycle.h"
#include "core/math/Math.h"
#include "core/system/io/FileUtils.h"
#include "core/system/io/CBinaryFile.h"
#include "core-ext/system/io/Resources.h"

#include "after/terrain/Zones.h"
#include "after/entities/world/environment/EnvSunlight.h"

#include "renderer/camera/CCamera.h"
#include "renderer/material/glMaterial.h"
#include "renderer/texture/CTexture.h"
#include "renderer/logic/model/CModel.h"
#include "renderer/object/shapes/CRenderPlane.h"
#include "renderer/state/Settings.h"


// Public listing of dominant day cycle
Daycycle* Daycycle::DominantCycle = NULL;

// Daycycle constructor
Daycycle::Daycycle ( )
	: CGameBehavior()
{
	vPosition = Vector3d(0, 0, 0); // Initializes the position vector to the default position.
	skyObject = new EnvSunlight;
	//skyObject = new DirectionalLight;
	//speed = 60;
	timeofDay = 60*60*3; //6+3= 9 oclock morning
	//timeofDay = 60*60*12; //6+12= 18 = 6 oclock afternoon
	//timeofDay = 60*60*18; //6+18= 24 = 12 oclock midnight (DEFAULT)
	/*pSunBillboard = new CBillboard();
	pSunBillboard->SetSize( 2.0f );
	pSunBillboard->SetRenderType( Background );*/
	cloudCoverage = 0;
	spaceEffect = 0;

	// Skysphere
	pSkyMat = new glMaterial();
	pSkyMat->setTexture( 0, new CTexture( ".res/textures/starmap.jpg" ) );
	pSkyMat->setTexture( 1, new CTexture( ".res/textures/cloudmap_hf.png" ) );
	pSkyMat->passinfo.push_back( glPass() );
	pSkyMat->passinfo[0].m_lighting_mode = Renderer::LI_NONE;
	pSkyMat->passinfo[0].m_face_mode = Renderer::FM_FRONTANDBACK;
	pSkyMat->passinfo[0].shader = new glShader ( ".res/shaders/sky/skycolors.glsl" );

	pSkyMat->passinfo.push_back( glPass() );
	pSkyMat->passinfo[1].m_lighting_mode = Renderer::LI_NONE;
	pSkyMat->passinfo[1].m_face_mode = Renderer::FM_FRONTANDBACK;
	pSkyMat->passinfo[1].shader = new glShader ( ".res/shaders/sky/skycolors.glsl" );
	pSkyMat->passinfo[1].m_hint = RL_SKYGLOW;

	/*pSkyMat->deferredinfo.push_back( glPass_Deferred() );
	pSkyMat->deferredinfo[0].m_lighting_mode = Renderer::LI_NONE;
	pSkyMat->deferredinfo[0].m_transparency_mode = Renderer::ALPHAMODE_NONE;
	pSkyMat->deferredinfo[0].m_diffuse_method = Renderer::Deferred::DIFFUSE_SKYSPHERE;*/

	pSkyMat->removeReference();

	skyModel = new CModel ( string("models/geosphere.FBX") );
	skyModel->SetMaterial( pSkyMat );
	skyModel->SetRenderType( Renderer::Background );
	skyModel->transform.scale = Vector3d( 1000,1000,-1000 );
	skyModel->SetFrustumCulling( false );

	// Starsphere
	pStarMat = new glMaterial();
	pStarMat->setTexture( 0, new CTexture( "textures/starmap.jpg" ) );
	pStarMat->setTexture( 1, new CTexture( "textures/cloudmap_hf.png" ) );
	pStarMat->passinfo.push_back( glPass() );
	pStarMat->passinfo[0].m_transparency_mode = Renderer::ALPHAMODE_TRANSLUCENT;
	pStarMat->passinfo[0].b_depthmask = false;
	pStarMat->passinfo[0].m_lighting_mode = Renderer::LI_NONE;
	pStarMat->passinfo[0].m_blend_mode = Renderer::BM_ADD;
	pStarMat->passinfo[0].shader = new glShader ( "shaders/sky/starfields.glsl" );
	pStarMat->removeReference();

	starModel = new CModel ( string("models/geosphere.FBX") );
	starModel->SetMaterial( pStarMat );
	starModel->SetRenderType( Renderer::Background );
	starModel->transform.scale = Vector3d( 940,940,-940 );
	starModel->SetFrustumCulling( false );

	// Horizon Object
	pHorizonMat = new glMaterial();
	pHorizonMat->setTexture( 0, new CTexture( "null" ) );
	pHorizonMat->passinfo.push_back( glPass() );
	pHorizonMat->passinfo[0].m_lighting_mode = Renderer::LI_NORMAL;
	pHorizonMat->passinfo[0].shader = new glShader ( "shaders/sky/horizon.glsl" );
	//pHorizonMat->passinfo[0].shader = new glShader ( ".res/shaders/particles/colorBlended.glsl" );
	pHorizonMat->removeReference();

	horizonPlane = new CRenderPlane( 1024.0f, 1024.0f );
	horizonPlane->SetRenderType( Renderer::Background );
	horizonPlane->SetMaterial( pHorizonMat );
	horizonPlane->transform.scale = Vector3d( 8,8,1 );
	//horizonPlane->visible = false;

	clearColors.push_back( Color( 0.77f,0.65f,0.09f ) );
	clearColors.push_back( Color( 0.72f,0.82f,1.0f ) );
	clearColors.push_back( Color( 0.72f,0.82f,1.0f ) );
	clearColors.push_back( Color( 0.78f,0.11f,0.08f ) );
	clearColors.push_back( Color( 0.02f,0.02f,0.04f ) );
	clearColors.push_back( Color( 0.02f,0.02f,0.04f ) );
	clearColors.loop = true;

	sunColors.push_back( Color( 0.77f,0.65f,0.09f ) );
	sunColors.push_back( Color( 0.72f,0.82f,1.0f ) );
	sunColors.push_back( Color( 0.72f,0.82f,1.0f ) );
	sunColors.push_back( Color( 0.78f,0.11f,0.08f ) );
	sunColors.push_back( Color( 0.02f,0.02f,0.04f ) );
	sunColors.push_back( Color( 0.02f,0.02f,0.04f ) );
	sunColors.loop = true;

	skyColors.push_back( Color( 0.72f,0.82f,1.0f ) );
	skyColors.push_back( Color( 0.72f,0.82f,1.0f ) );
	skyColors.push_back( Color( 0.72f,0.82f,1.0f ) );
	skyColors.push_back( Color( 0.72f,0.82f,1.0f ) );
	skyColors.push_back( Color( 0.02f,0.02f,0.04f ) );
	skyColors.push_back( Color( 0.02f,0.02f,0.04f ) );
	skyColors.loop = true;

	ambientColors.push_back( Color( 0.40f, 0.38f, 0.25f ) );
	ambientColors.push_back( Color( 0.67f, 0.67f, 0.56f ) );
	ambientColors.push_back( Color( 0.67f, 0.67f, 0.56f ) );
	ambientColors.push_back( Color( 0.44f, 0.24f, 0.32f ) );
	ambientColors.push_back( Color( 0.018f,0.015f,0.033f ) );
	ambientColors.push_back( Color( 0.018f,0.015f,0.033f ) );
	ambientColors.loop = true;

	cAmbientOffset	= Color( 0,0,0 );

	if ( DominantCycle == NULL ) {
		DominantCycle = this;
	}

	string sunfile = Core::Resources::PathTo( "terra/sun_terra0" );
	if ( IO::FileExists( sunfile ) ) {
		CBinaryFile io;
		io.Open( sunfile.c_str(), io.IO_READ );
		io.ReadData( (char*)&clearColors[0], sizeof(Color)*6 );
		io.ReadData( (char*)&sunColors[0], sizeof(Color)*6 );
		io.ReadData( (char*)&skyColors[0], sizeof(Color)*6 );
		io.ReadData( (char*)&ambientColors[0], sizeof(Color)*6 );
		io.Close();
	}
}

Daycycle::~Daycycle(void)
{
	delete skyObject;
	delete starModel;
	delete skyModel;
	delete horizonPlane;

	if ( DominantCycle == this ) {
		DominantCycle = NULL;
	}
}

void Daycycle::SetTimeOfDay ( const ftype newTime )
{
	timeofDay = newTime;
}
ftype Daycycle::GetTimeOfDay ( void )
{
	return timeofDay;
}
void Daycycle::SetSkyCoverage ( const ftype newCoverage )
{
	cloudCoverage = newCoverage;
}
Color Daycycle::GetSkyobjectDiffuse ( void )
{
	return Color(1,1,1,1);//skyObject->diffuseColor;
}
void Daycycle::SetSpaceEffect ( const Real newScE )
{
	spaceEffect = newScE;
}

void Daycycle::Update ( void )
{
	if ( CCamera::activeCamera == NULL )
		return;

	// Scale sky and stars to the camera zFar
	skyModel->transform.scale = Vector3d( 1,1,-1 ) * CCamera::activeCamera->zFar;
	starModel->transform.scale = Vector3d( 0.94f,0.94f,-0.94f ) * CCamera::activeCamera->zFar;
	
	vPosition.x = (ftype)cos(timeofDay * (((degtorad(360) / 24) / 60) / 60 )); // X position of light source.
	vPosition.z = (ftype)sin(timeofDay * (((degtorad(360) / 24) / 60) / 60 ));	// Z position of light source.
	//vPosition.y = 0.0f;
	vPosition.y = (ftype)sin(timeofDay * (((degtorad(360) / 24) / 60) / 60 )) * 0.1f; // Y position of light source. (rolls the sun a little)
	//vPosition.normalize();

	skyObject->vLightDir = Color(vPosition.x, vPosition.y, vPosition.z, 0);
	skyObject->diffuseColor = (Color( 0.6f,0.6f,0.6f )*0.5f+((Renderer::Settings.clearColor)*0.6f)) * std::max<ftype>( 0, std::min<ftype>( 1, (vPosition.z+0.2f)/0.2f ) ); // Fades out sunlight at night
	skyObject->diffuseColor = skyObject->diffuseColor.Lerp( Color( 0.7f,0.7f,0.6f ), spaceEffect ); // Lerp towards single light for out of atmosphere
	skyObject->diffuseColor = skyObject->diffuseColor * (1-cloudCoverage); // Darken from cloud coverage
	
	//==Background sky color, based on Z of sun==
	Renderer::Settings.clearColor = clearColors.get_target_value( Math.Wrap( timeofDay/86400, 0, 1 ) );
	Renderer::Settings.clearColor = Renderer::Settings.clearColor.Lerp( Color(0,0,0), spaceEffect );

	//==Ambient Light Color, based on Z of sun==
	Renderer::Settings.ambientColor = ambientColors.get_target_value( Math.Wrap( timeofDay/86400, 0, 1 ) );
	Renderer::Settings.ambientColor = Renderer::Settings.ambientColor.Lerp( Color(0.12,0.11,0.14), spaceEffect );

	// Add ambient light
	Renderer::Settings.ambientColor = Renderer::Settings.ambientColor + cAmbientOffset;
}

void Daycycle::PostUpdate ( void )
{ 
	if ( CCamera::activeCamera == NULL )
		return;

	// Check if half the day cycle is over.
	if (timeofDay <= (86400 / 2)) { 
		// If day cycle is less then or equal to half the cycle, day time is true.
		dayPhase = true; 
	}
	else {
		// if day cycle is greater than half the cylce, day time is false.
		dayPhase = false;
	}

	/******************************************************************************************
	NOTE:
	180 degrees or PI is a day or night cycle, 360 is a full day-night cycle.
	Conversion: degtorad(360) / 24 hours / 60 min / 60 sec = radians/sec for a full day cycle.
	******************************************************************************************/

	starModel->transform.position = CCamera::activeCamera->transform.position;
	starModel->transform.rotation.Euler( Vector3d( 90,timeofDay/86400 * 360,0 ) );

	skyModel->transform.position = CCamera::activeCamera->transform.position;
	skyModel->transform.rotation.Euler( Vector3d( 90,timeofDay/86400 * 360,0 ) );
	//skyModel->transform.rotation = starModel->transform.rotation;

	pStarMat->m_diffuse = Color::Lerp( Color( 1.0f,1.0f,1.0f ), Color( spaceEffect,spaceEffect,spaceEffect ), vPosition.z+1 );
	pSkyMat->m_diffuse = Renderer::Settings.clearColor;
	skyModel->SetShaderUniform( "gm_LightDirection", GetSunDirection() );
	skyModel->SetShaderUniform( "gm_SunColor", Color::Lerp( sunColors.get_target_value( Math.Wrap( timeofDay/86400, 0, 1 ) ), Color(0.3,0.3,0.25), spaceEffect ) );
	skyModel->SetShaderUniform( "gm_SkyColor", Color::Lerp( skyColors.get_target_value( Math.Wrap( timeofDay/86400, 0, 1 ) ), Color(0,0,0), spaceEffect ) );
	skyModel->SetShaderUniform( "gm_HorizonStrength", 1-spaceEffect );
	starModel->SetShaderUniform( "gm_HorizonStrength", 1-spaceEffect );

	skyObject->cFogSunColor = Color::Lerp( Renderer::Settings.clearColor, Color( 0.8f,0.8f,0.8f ), 0.2f );
	skyObject->cFogAmbientColor = Color::Lerp( Renderer::Settings.ambientColor * 0.8f, Color( 0.3f,0.3f,0.3f ), 0.2f );
	skyObject->fFogFalloff = 1.0f;

	horizonPlane->transform.position = CCamera::activeCamera->transform.position - Vector3d( 0,0, (Real)Zones.GetCurrentRoot().z );
	horizonPlane->transform.position.z = std::min<Real>(
		std::min<Real>( horizonPlane->transform.position.z*0.3f, horizonPlane->transform.position.z - 0.5f ),
		horizonPlane->transform.position.z + 5.0f )
		- (spaceEffect*4000.0f);

	pHorizonMat->m_diffuse = (Renderer::Settings.clearColor + skyColors.get_target_value( Math.Wrap( timeofDay/86400, 0, 1 ) ) + Renderer::Settings.ambientColor) * 0.33f;
	pHorizonMat->m_diffuse.alpha = 1;
}

Vector3d Daycycle::GetSunPosition ( CCamera* cam )
{
	Vector3d resultPos = cam->WorldToScreenPos( cam->transform.position + Vector3d( skyObject->vLightDir.red, skyObject->vLightDir.green, skyObject->vLightDir.blue ).normal()*8 );
	// but override (nearly) useless z
	resultPos.z = CCamera::activeCamera->transform.Forward().dot( Vector3d( skyObject->vLightDir.red, skyObject->vLightDir.green, skyObject->vLightDir.blue ).normal() );
	return resultPos;
}
Vector3d Daycycle::GetSunDirection ( void )
{
	//return Vector3d( 1,0,0 );
	return Vector3d( skyObject->vLightDir.red, skyObject->vLightDir.green, skyObject->vLightDir.blue ).normal();
}

Mooncycle::Mooncycle ( void )
	: CGameBehavior()
{
	timeofCycle = 0;
	cycleLength = 104400;

	pMoonMat = new glMaterial();
	/*pMoonMat->setShader( new glShader( ".res/shaders/
	pMoonMat->useLighting = false;
	pMoonMat->loadTexture( ".res/textures/null.jpg" );
	pMoonMat->diffuse = Color( 0.8f, 0.8f, 0.8f, 1.0f );*/
	pMoonMat->loadFromFile( "sky/moon_terra" );

	moonModel = new CModel ( string("models/geosphere.FBX") );
	moonModel->SetMaterial( pMoonMat );
	moonModel->SetRenderType( Renderer::Background );
	moonModel->transform.scale = Vector3d( 28,28,28 );
	moonModel->SetFrustumCulling( false );

}

Mooncycle::~Mooncycle ( void )
{
	delete moonModel;
	delete pMoonMat;
}

void Mooncycle::Update ( void )
{
	vPosition.x = (ftype)cos(timeofCycle/cycleLength * degtorad(360) + 0.1f); // X position of light source.
	vPosition.z = (ftype)sin(timeofCycle/cycleLength * degtorad(360) + 0.1f)+0.1f;	// Z position of light source.
	vPosition.y = (ftype)sin(timeofCycle/cycleLength * degtorad(360)* (1/4.92f) + 0.6f)*0.7f; // Y position of light source.
	vPosition.normalize();
}

void Mooncycle::PostUpdate ( void )
{ 
	if ( CCamera::activeCamera == NULL )
		return;

	moonModel->transform.position = CCamera::activeCamera->transform.position + vPosition * 600;
	moonModel->transform.rotation.Euler( Vector3d( 90,timeofCycle/cycleLength * 360,0 ) );
}


void Mooncycle::SetTimeOfCycle ( ftype newTime )
{
	timeofCycle = newTime;
}
void Mooncycle::SetSkyCoverage ( ftype newCoverage )
{
	cloudCoverage = newCoverage;
}