
// Main include
#include "CSplashScreens.h"

// Includes
#include "renderer/camera/CCamera.h"
#include "renderer/object/shapes/CRenderPlane.h"
#include "renderer/texture/CTexture.h"
#include "renderer/material/glMaterial.h"

#include "core/time/time.h"
#include "core/math/Math.h"
#include "core/math/noise/Perlin.h"

#include "core/input/CInput.h"

#include "after/entities/world/environment/DayAndNightCycle.h"

#include "CStudioSplash.h"

// Constructor + Destructor
CSplashScreens::CSplashScreens ( void )
	: CGameBehavior()
{
	iState	= SPLASH_LOAD;
	timer	= 0.0f;

	pCamera	= NULL;
	pGroundPlane	= NULL;
	pSplashPlane	= NULL;

	pTexHavok	= NULL;
	pTexDisclaimer	= NULL;

	pMatSplash	= NULL;

	noise	= NULL;
}

CSplashScreens::~CSplashScreens ( void )
{
	delete_safe( pCamera );
	delete_safe( pGroundPlane );
	delete_safe( pSplashPlane );

	pMatSplash->removeReference();
	delete_safe( pMatSplash	);

	delete_safe( pTexHavok );
	delete_safe( pTexDisclaimer	);

	delete_safe( noise );
}

void CSplashScreens::Update ( void )
{
	static ftype timeOfDay = 60*60*-1;
	if ( iState != SPLASH_STUDIO ) {
		timeOfDay += 60 * 20 * Time::deltaTime;
		if ( Daycycle::DominantCycle ) {
			Daycycle::DominantCycle->SetTimeOfDay( timeOfDay );
		}
	}

	// Increment timer
	timer += Time::deltaTime * 0.85f;
	
	if ( pCamera && noise )
	{
		pCamera->transform.rotation = Vector3d( 0, noise->Get( 0.1f, Time::currentTime ), noise->Get( Time::currentTime, 0.1f ) );
	}

	// Work on state
	switch ( iState )
	{
	case SPLASH_LOAD:
		Splash_Load();
		break;
	case SPLASH_STUDIO:
		if ( mStudioSplash->Done() ) {
			DeleteObject(mStudioSplash);
			// Go to next state
			iState = eCurrentSplashState(iState+1);
			// Reset timer
			timer = 0;
		}
		break;
	case SPLASH_HAVOK:
		Splash_Havok();
		break;
	case SPLASH_DISCLAIMER:
		Splash_Disclaimer();
		break;
	};

	// Do a skip if key pressed
	//if ( Input::KeydownAny() ) {
	if ( Input::Keydown( Keys.Escape ) || Input::Keydown( Keys.Return ) ) {
		timer = 3.2f;
		iState = SPLASH_DISCLAIMER;
	}
}

void CSplashScreens::Splash_Load ( void )
{
	// Set up the camera
	pCamera = new CCamera();
	pCamera->transform.position = Vector3d( -10,0,1.0f );

	// Load the textures
	pTexHavok = new CTexture( ".res/textures/logos/Havok_Logo.png" );
	pTexDisclaimer = new CTexture( ".res/textures/logos/Disclaimer.png" );

	// Create the material that will be used
	pMatSplash = new glMaterial();
	pMatSplash->passinfo.push_back( glPass() );
	pMatSplash->passinfo[0].m_transparency_mode = Renderer::ALPHAMODE_ALPHATEST;
	pMatSplash->passinfo[0].m_lighting_mode = Renderer::LI_NONE;
	pMatSplash->passinfo[0].m_face_mode = Renderer::FM_FRONTANDBACK;
	pMatSplash->passinfo[0].shader = new glShader( ".res/shaders/v2d/default.glsl" );
	pMatSplash->passinfo.push_back( glPass() );
	pMatSplash->passinfo[1].m_transparency_mode = Renderer::ALPHAMODE_ALPHATEST;
	pMatSplash->passinfo[1].m_lighting_mode = Renderer::LI_NONE;
	pMatSplash->passinfo[1].m_face_mode = Renderer::FM_FRONTANDBACK;
	pMatSplash->passinfo[1].shader = new glShader( ".res/shaders/sys/black.glsl" );
	pMatSplash->passinfo[1].m_hint = RL_SKYGLOW;

	// Create the splash plane
	pSplashPlane = new CRenderPlane ();
	pSplashPlane->SetMaterial( pMatSplash );
	pMatSplash->setTexture( 0, pTexHavok );

	// Immediately go to next state
	//iState = iState+1;
	iState = eCurrentSplashState(iState+1);

	// Create the splash
	mStudioSplash = new CStudioSplash();
	mStudioSplash->RemoveReference();

	// Create noise
	noise = new Perlin ( 2, 0.8f, 1, 1337 );
}

void CSplashScreens::Splash_Havok ( void )
{
	static ftype angleOffset = -90;
	/*if ( angleOffset < 0 )
	{
		angleOffset += Time::deltaTime * 120.0f;
	}
	else
	{
		if ( timer < 2.0f )
		{
			angleOffset = 0;
		}
		else
		{
			angleOffset += Time::deltaTime * 120.0f;
			if ( angleOffset > 90 )
				angleOffset = 90;
		}
	}*/
	if ( timer < 0.8f )
	{
		angleOffset = Math.Smoothlerp( timer/0.8f, -90, 0 );
	}
	else if ( timer < 2.4f )
	{
		angleOffset = 0;
	}
	else if ( timer < 3.2f )
	{
		angleOffset = Math.Smoothlerp( (timer-2.4f)/0.8f, 0, 90 );
	}

	pSplashPlane->transform.position = Vector3d( 2.0f-sqrt(timer),0,1.0f );
	pSplashPlane->transform.rotation = Vector3d( 90+angleOffset,angleOffset*0.1f,90-angleOffset*0.2f );
	ftype scalar = 33.0f / (ftype)pTexHavok->GetWidth();
	pSplashPlane->transform.scale = Vector3d( (ftype)pTexHavok->GetWidth(), (ftype)pTexHavok->GetHeight(), 1.0f ) * scalar;

	if ( timer > 3.2f )
	{
		// Go to next state
		iState = eCurrentSplashState(iState+1);
		// Set new texture
		pMatSplash->setTexture( 0, pTexDisclaimer );
		// Reset timer 
		timer = 0;
	}
}

#include "after/scenes/gmsceneMainGame.h"
#include "after/scenes/gmsceneMenu.h"
#include "after/scenes/gmsceneGame_5.h"

void CSplashScreens::Splash_Disclaimer ( void )
{
	static ftype angleOffset = -90;
	/*if ( angleOffset < 0 )
	{
		angleOffset += Time::deltaTime * 120.0f;
	}
	else
	{
		if ( timer < 2.0f )
		{
			angleOffset = 0;
		}
		else
		{
			angleOffset += Time::deltaTime * 120.0f;
			if ( angleOffset > 90 )
				angleOffset = 90;
		}
	}*/
	if ( timer < 0.8f )
	{
		angleOffset = Math.Smoothlerp( timer/0.8f, -90, 0 );
	}
	else if ( timer < 2.4f )
	{
		angleOffset = 0;
	}
	else if ( timer < 3.2f )
	{
		angleOffset = Math.Smoothlerp( (timer-2.4f)/0.8f, 0, 90 );
	}

	pSplashPlane->transform.position = Vector3d( 2.0f-sqrt(timer),0,1.0f );
	pSplashPlane->transform.rotation = Vector3d( 90-angleOffset,0,90-angleOffset*0.2f );
	ftype scalar = 38.0f / (ftype)pTexDisclaimer->GetWidth();
	pSplashPlane->transform.scale = Vector3d( (ftype)pTexDisclaimer->GetWidth(), (ftype)pTexDisclaimer->GetHeight(), 1.0f ) * scalar;

	if ( timer > 3.2f )
	{
		// Go to next state
		//iState = eCurrentSplashState(iState+1);
		// Reset timer 
		timer = -9000;

		CGameScene* pNewScene;
		//pNewScene = CGameScene::NewScene<gmsceneMainGame> ();
		pNewScene = CGameScene::NewScene<gmsceneGame_5> ();
		CGameScene::SceneGoto( pNewScene );
	}
}