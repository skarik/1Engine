#include "CScreenFade.h"
#include "core/time/time.h"

#include "renderer/texture/CTexture.h"
#include "renderer/camera/CCamera.h"
#include "renderer/material/RrMaterial.h"

#include "renderer/system/glMainSystem.h"
#include "renderer/system/glDrawing.h"

#include "renderer/object/immediate/immediate.h"

CScreenFade::CScreenFade ( bool inbFadeIn, float infFadeTime, float infFadeDelay, Color incFadeColor )
	: CRenderableObject(), CLogicObject()
{
	bFadeIn		= inbFadeIn;
	fFadeTime	= infFadeTime;
	fFadeDelay	= infFadeDelay;
	cFadeColor	= incFadeColor;

	pTargetCamera = CCamera::activeCamera;

	renderType	= renderer::V2D;

	fCurrentTime = 0;
	if ( bFadeIn )
		fAlpha = 1.0f;
	else
		fAlpha = 0.0f;

	screenMaterial = new RrMaterial();
	screenMaterial->m_diffuse = cFadeColor;
	screenMaterial->setTexture( TEX_DIFFUSE, new CTexture("textures/white.jpg") );
	screenMaterial->passinfo.push_back( RrPassForward() );
	screenMaterial->passinfo[0].m_transparency_mode = renderer::ALPHAMODE_TRANSLUCENT;
	screenMaterial->passinfo[0].m_lighting_mode	= renderer::LI_NONE;
	screenMaterial->passinfo[0].shader = new RrShader( "shaders/v2d/default.glsl" );
	SetMaterial( screenMaterial );
}
CScreenFade::~CScreenFade ( void )
{
	screenMaterial->removeReference();
	delete screenMaterial;
}

void CScreenFade::PreStepSynchronus ( void )
{
	fCurrentTime += Time::smoothDeltaTime;

	if ( bFadeIn )
		fAlpha = 1.0f;
	else
		fAlpha = 0.0f;
	if ( fCurrentTime > fFadeDelay )
	{
		if ( bFadeIn )
			fAlpha -= (fCurrentTime-fFadeDelay)/fFadeTime;
		else
			fAlpha += (fCurrentTime-fFadeDelay)/fFadeTime;
	}
	if ( fCurrentTime > (fFadeDelay+fFadeTime) )
	{
		if ( bFadeIn )
			fAlpha = 0.0f;
		else
			fAlpha = 1.0f;
		//this->DeleteObject( this );
		// TODO: Fix this
	}
}

bool CScreenFade::PreRender ( void )
{
	//GL.Translate( Vector3d( 0,0,40 ) );
	screenMaterial->prepareShaderConstants();
	return true;
}
bool CScreenFade::Render ( const char pass )
{
	GL_ACCESS GLd_ACCESS
	if ( fAlpha > 0.0f )
	{
		//GL.beginOrtho();
		core::math::Cubic::FromPosition( Vector3d(0, 0, -45.0F), Vector3d((Real)Screen::Info.width, (Real)Screen::Info.height, +45.0F) );
			//GL.Translate( Vector3d( 0,0,40 ) );
			GLd.P_PushColor(1,1,1);
			GLd.DrawSet2DMode( GLd.D2D_FLAT );
			GLd.DrawSet2DScaleMode( GLd.SCALE_DEFAULT );
			screenMaterial->m_diffuse.alpha = std::min<Real>( fAlpha, 1 );
			screenMaterial->bindPass(0);
				GLd.DrawRectangleA( 0,0, 1,1 );
		//GL.endOrtho();
	}
	return true;
}