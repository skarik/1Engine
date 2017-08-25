
#include "EnvSunlight.h"
#include "engine/physics/raycast/Raycaster.h"
#include "after/physics/Caster.h"
#include "core/time/time.h"
#include "after/states/CWorldState.h"
#include "after/types/terrain/BlockTracker.h"
#include "renderer/light/DirectionalLight.h"
#include "renderer/material/glMaterial.h"
#include "renderer/texture/CTexture.h"
#include "renderer/object/shapes/CBillboard.h"
#include "renderer/state/Settings.h"
#include "renderer/camera/CCamera.h"

// == EnvSunlight Constructor ==
EnvSunlight::EnvSunlight ( void )
	: CGameBehavior()
{
	skyLight = new DirectionalLight;
	//skyLight->generateShadows = true;

	pSunMat = new glMaterial();
	pSunMat->m_diffuse = Color( 1.6f,1.6f,1.6f );
	pSunMat->setTexture( 0, new CTexture(".res/textures/sun_flat.jpg") );
	pSunMat->passinfo.push_back( glPass() );
	pSunMat->passinfo[0].m_lighting_mode = Renderer::LI_NONE;
	pSunMat->passinfo[0].m_transparency_mode = Renderer::ALPHAMODE_TRANSLUCENT;
	pSunMat->passinfo[0].shader = new glShader( ".res/shaders/d/default_billboard.glsl" );
	pSunMat->passinfo[0].m_blend_mode = Renderer::BM_ADD;

	pSunMat->passinfo.push_back( glPass() );
	pSunMat->passinfo[1].m_lighting_mode = Renderer::LI_NONE;
	pSunMat->passinfo[1].m_transparency_mode = Renderer::ALPHAMODE_TRANSLUCENT;
	pSunMat->passinfo[1].shader = new glShader( ".res/shaders/d/default_billboard.glsl" );
	pSunMat->passinfo[1].m_blend_mode = Renderer::BM_ADD;
	pSunMat->passinfo[1].m_hint = RL_SKYGLOW;

	/*pSunMat->deferredinfo.push_back( glPass_Deferred() );
	pSunMat->deferredinfo[0].m_blend_mode = Renderer::BM_ADD;
	pSunMat->deferredinfo[0].m_transparency_mode = Renderer::ALPHAMODE_TRANSLUCENT;
	pSunMat->deferredinfo[0].m_lighting_mode = Renderer::LI_NONE;
	pSunMat->deferredinfo[0].m_diffuse_method = Renderer::Deferred::DIFFUSE_ALPHA_ADDITIVE;
	*/
	pSunMat->removeReference();

	pSunMatGlare = new glMaterial();
	pSunMatGlare->setTexture( 0, new CTexture(".res/textures/sun_glare.jpg") );
	pSunMatGlare->m_diffuse = Color( 1.5f,1.5f,1.5f );
	pSunMatGlare->passinfo.push_back( glPass() );
	pSunMatGlare->passinfo[0].m_lighting_mode = Renderer::LI_NONE;
	pSunMatGlare->passinfo[0].m_transparency_mode = Renderer::ALPHAMODE_TRANSLUCENT;
	pSunMatGlare->passinfo[0].m_blend_mode = Renderer::BM_ADD;
	pSunMatGlare->passinfo[0].shader = new glShader( ".res/shaders/d/default_billboard.glsl" );

	/*pSunMatGlare->deferredinfo.push_back( glPass_Deferred() );
	pSunMatGlare->deferredinfo[0].m_blend_mode = Renderer::BM_ADD;
	pSunMatGlare->deferredinfo[0].m_transparency_mode = Renderer::ALPHAMODE_TRANSLUCENT;
	pSunMatGlare->deferredinfo[0].m_lighting_mode = Renderer::LI_NONE;
	pSunMatGlare->deferredinfo[0].m_diffuse_method = Renderer::Deferred::DIFFUSE_ALPHA_ADDITIVE;
	*/
	pSunMatGlare->removeReference();

	pSunMatForeground = new glMaterial();
	pSunMatForeground->setTexture( 0, new CTexture(".res/textures/sun_glare.jpg") );
	pSunMatForeground->m_diffuse = Color( 1.5f,1.5f,1.5f );
	pSunMatForeground->passinfo.push_back( glPass() );
	pSunMatForeground->passinfo[0].m_lighting_mode = Renderer::LI_NONE;
	pSunMatForeground->passinfo[0].m_transparency_mode = Renderer::ALPHAMODE_TRANSLUCENT;
	pSunMatForeground->passinfo[0].m_blend_mode = Renderer::BM_ADD;
	pSunMatForeground->passinfo[0].shader = new glShader( ".res/shaders/d/default_billboard.glsl" );

	/*pSunMatForeground->deferredinfo.push_back( glPass_Deferred() );
	pSunMatForeground->deferredinfo[0].m_blend_mode = Renderer::BM_ADD;
	pSunMatForeground->deferredinfo[0].m_transparency_mode = Renderer::ALPHAMODE_TRANSLUCENT;
	pSunMatForeground->deferredinfo[0].m_lighting_mode = Renderer::LI_NONE;
	pSunMatForeground->deferredinfo[0].m_diffuse_method = Renderer::Deferred::DIFFUSE_ALPHA_ADDITIVE;
	*/
	pSunMatForeground->removeReference();

	fSunMatBrightness = 0.6f;

	pSunBillboard = new CBillboard();
	pSunBillboard->SetSize( 3.7f );
	pSunBillboard->SetRenderType( Renderer::Background );
	pSunBillboard->SetMaterial( pSunMat );
	pSunBillboard->SetVisible( true );

	pSunBillboardGlare = new CBillboard();
	pSunBillboardGlare->SetSize( 13.0f );
	pSunBillboardGlare->SetRenderType( Renderer::Background );
	pSunBillboardGlare->SetMaterial( pSunMatGlare );
	pSunBillboardGlare->SetVisible( true );

	pSunBillboardForeground = new CBillboard();
	pSunBillboardForeground->SetSize( 14.0f );
	pSunBillboardForeground->SetRenderType( Renderer::Foreground );
	pSunBillboardForeground->SetMaterial( pSunMatForeground );
	pSunBillboardForeground->SetVisible( true );

	fFogFalloff = 1.0f;

	vLightDir = Color( 1.0f, 0.0f, 0.0f, 1.0f );
}


// == EnvSunlight Destructor ==
EnvSunlight::~EnvSunlight ( void )
{
	delete skyLight;
	delete pSunBillboard;
	delete pSunBillboardGlare;
	delete pSunBillboardForeground;
	//delete pSunMatForeground;
}

// == EnvSunlight LateUpdate ==
void EnvSunlight::LateUpdate ( void )
{
	if ( CCamera::activeCamera == NULL )
		return;

	// Change the fog color based on the facing direction of the camera
	Renderer::Settings.fogColor = Color::Lerp( cFogAmbientColor, cFogSunColor, CCamera::activeCamera->transform.Forward().dot( Vector3d( vLightDir.red, vLightDir.green, vLightDir.blue ) ) );

	// Set default fog settings
	Renderer::Settings.fogStart = 10.0f;
	Renderer::Settings.fogEnd = 300.0f;
}

// == EnvSunlight PostUpdate ==
void EnvSunlight::PostUpdate ( void )
{
	if ( CCamera::activeCamera == NULL )
		return;

	//pSunBillboard->transform.position = CCamera::activeCamera->transform.position + vPosition*8;
	pSunBillboard->transform.position = CCamera::activeCamera->transform.position + Vector3d( vLightDir.red, vLightDir.green, vLightDir.blue ).normal()*8; // THIS WORK FOG ADDITIVE
	pSunBillboardGlare->transform.position = pSunBillboard->transform.position;
	//pSunBillboardGlare->transform.Get( pSunBillboard->transform );
	pSunBillboardForeground->transform.position = pSunBillboard->transform.position;
	//pSunBillboardForeground->transform.Get( pSunBillboard->transform );
	skyLight->vLightDir = vLightDir;
	skyLight->diffuseColor = diffuseColor;
	if (( skyLight->diffuseColor.red > 0.05f )&&( skyLight->diffuseColor.green > 0.05f )&&( skyLight->diffuseColor.blue > 0.05f ))
	{
		skyLight->generateShadows = true;
	}
	else
	{
		skyLight->generateShadows = false;
	}

	//pSunBillboard->transform.position.y += 6;
	//pSunBillboardGlare->transform.position.y -= 6;
	//pSunBillboardGlare->transform.position.x -= 6;
	//pSunBillboard->transform.localPosition.y += 6;
	//pSunBillboardGlare->transform.localPosition.y -= 6;
	Color glowOffset (0,0,0,1);
	if ( ActiveGameWorld ) {
		glowOffset.red += std::min<Real>(ActiveGameWorld->cBaseAmbient.red,0.02f);
		glowOffset.green += std::min<Real>(ActiveGameWorld->cBaseAmbient.green,0.02f);
		glowOffset.blue += std::min<Real>(ActiveGameWorld->cBaseAmbient.blue,0.02f);
	}

	// Have a glow that's occluded by the world
	Ray castRay;
	castRay.dir = -Vector3d( vLightDir.red, vLightDir.green, vLightDir.blue ).normal();
	castRay.pos = CCamera::activeCamera->transform.position - castRay.dir*803.0f;
	RaycastHit		result;
	if ( ( castRay.dir.z > (CCamera::activeCamera->transform.position.z*0.0007f) ) || Raycaster.Raycast( castRay, 800.0f, &result, 1|2|4 ) )
	{
		fSunMatBrightness = std::max<ftype>( fSunMatBrightness - 1.2f*Time::deltaTime, 0 );
	}
	else
	{
		fSunMatBrightness = std::min<ftype>( fSunMatBrightness + 2.4f*Time::deltaTime, 0.7f );
	}
	pSunMatForeground->m_diffuse = (diffuseColor+glowOffset) * fSunMatBrightness * 1.4f;

	// Set sun color
	//pSunMat->m_diffuse = (diffuseColor+Color(1.4f,1.4f,1.4f)) * (fSunMatBrightness+2.0f) * 0.1f;
	pSunMat->m_diffuse = (diffuseColor+Color(1.4f,1.4f,1.4f)) * (fSunMatBrightness+2.0f) * 0.5f;
	pSunMatGlare->m_diffuse = (pSunMat->m_diffuse+glowOffset) * 0.5f * 0.2f;

	// Alpha
	pSunMat->m_diffuse.alpha = 1;
	pSunMatGlare->m_diffuse.alpha = 1;
	pSunMatForeground->m_diffuse.alpha = 1;
}