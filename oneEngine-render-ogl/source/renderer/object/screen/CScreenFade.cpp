
#include "CScreenFade.h"
#include "core/time/time.h"

#include "renderer/texture/CTexture.h"
#include "renderer/camera/CCamera.h"
#include "renderer/material/glMaterial.h"

#include "renderer/system/glMainSystem.h"
#include "renderer/system/glDrawing.h"

CScreenFade::CScreenFade ( bool inbFadeIn, float infFadeTime, float infFadeDelay, Color incFadeColor )
	: CRenderableObject(), CLogicObject()
{
	bFadeIn		= inbFadeIn;
	fFadeTime	= infFadeTime;
	fFadeDelay	= infFadeDelay;
	cFadeColor	= incFadeColor;

	pTargetCamera = CCamera::activeCamera;

	renderType	= Renderer::V2D;

	fCurrentTime = 0;
	if ( bFadeIn )
		fAlpha = 1.0f;
	else
		fAlpha = 0.0f;

	screenMaterial = new glMaterial();
	screenMaterial->m_diffuse = cFadeColor;
	screenMaterial->setTexture( 0, new CTexture("textures/white.jpg") );
	screenMaterial->passinfo.push_back( glPass() );
	screenMaterial->passinfo[0].m_transparency_mode = Renderer::ALPHAMODE_TRANSLUCENT;
	screenMaterial->passinfo[0].m_lighting_mode	= Renderer::LI_NONE;
	screenMaterial->passinfo[0].shader = new glShader( "shaders/v2d/default.glsl" );
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
		DeleteObject( this );
	}
}

bool CScreenFade::Render ( const char pass )
{
	GL_ACCESS GLd_ACCESS
	if ( fAlpha > 0.0f )
	{
		GL.beginOrtho();
			GL.Translate( Vector3d( 0,0,40 ) );
			GLd.DrawSet2DMode( GLd.D2D_FLAT );
			GLd.DrawSet2DScaleMode( GLd.SCALE_DEFAULT );
			screenMaterial->m_diffuse.alpha = std::min<Real>( fAlpha, 1 );
			screenMaterial->bindPass(0);
			//screenMaterial->bind();
			//	glColor4f( 1,1,1,1 );
				GLd.DrawRectangleA( 0,0, 1,1 );
			//screenMaterial->unbind();
		GL.endOrtho();
	}

	return true;
}