#include "CScreenFade.h"
#include "core/time/time.h"

#include "renderer/texture/RrTexture.h"
#include "renderer/camera/RrCamera.h"
//#include "renderer/material/RrMaterial.h"

//#include "renderer/system/glMainSystem.h"
//#include "renderer/system/glDrawing.h"
#include "renderer/material/RrPass.h"
#include "renderer/material/RrShaderProgram.h"

#include "renderer/object/immediate/immediate.h"

CScreenFade::CScreenFade ( bool inbFadeIn, float infFadeTime, float infFadeDelay, Color incFadeColor )
	: CRenderableObject(), RrLogicObject()
{
	bFadeIn		= inbFadeIn;
	fFadeTime	= infFadeTime;
	fFadeDelay	= infFadeDelay;
	cFadeColor	= incFadeColor;

	pTargetCamera = RrCamera::activeCamera;

	//renderLayer	= renderer::kRLV2D;

	fCurrentTime = 0;
	if ( bFadeIn )
		fAlpha = 1.0f;
	else
		fAlpha = 0.0f;

	RrPass screenPass;
	screenPass.utilSetupAs2D();
	screenPass.m_type = kPassTypeForward;
	screenPass.m_layer = renderer::kRenderLayerV2D;
	screenPass.setTexture( TEX_MAIN, RrTexture::Load("null") );
	screenPass.setProgram( RrShaderProgram::Load( rrShaderProgramVsPs({"shaders/v2d/default_vv.spv", "shaders/v2d/default_p.spv"}) ) );
	screenPass.m_orderOffset = 1000;
	PassInitWithInput(0, &screenPass);
}
CScreenFade::~CScreenFade ( void )
{
	;
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

bool CScreenFade::PreRender ( rrCameraPass* cameraPass )
{
	//GL.Translate( Vector3d( 0,0,40 ) );
	//screenMaterial->m_diffuse.alpha = std::min<Real>( fAlpha, 1.0F );
	//screenMaterial->prepareShaderConstants();

	m_passes[0].m_surface.diffuseColor[3] = std::min<Real>( fAlpha, 1.0F );

	PushCbufferPerObject(XrTransform(), cameraPass);
	
	return true;
}
bool CScreenFade::Render ( const char pass )
{ /*GLd_ACCESS;
	if ( fAlpha > 0.0F )
	{
		GLd.DrawScreenQuad(screenMaterial);
	}*/

	// draw a quad like one would draw a mesh here

	return true;
}