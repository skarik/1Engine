
#include "Ploverlay_Goggles01.h"
#include "core/time/time.h"
#include "renderer/object/screenshader/effects/CDepthSplitShader.h"
#include "renderer/texture/CTexture.h"
#include "renderer/material/glMaterial.h"
#include "renderer/system/glMainSystem.h"
#include "renderer/system/glDrawing.h"

Ploverlay_Goggles01::Ploverlay_Goggles01 ( void )
	: CRenderableObject(), CGameBehavior()
{
	renderSettings.renderHints = RL_WORLD;
	renderType = Renderer::V2D;

	mMaterial = new glMaterial;
	mMaterial->setTexture( 0, new CTexture( ".res/textures/hud/overlay_goggles.png" ) );
	mMaterial->passinfo.push_back( glPass() );
	mMaterial->passinfo[0].m_transparency_mode = Renderer::ALPHAMODE_TRANSLUCENT;
	mMaterial->passinfo[0].shader = new glShader( ".res/shaders/v2d/default.glsl" );
	mMaterial->removeReference();

	SetMaterial( mMaterial );

	m_screenshader = new CDepthSplitShader();
}

Ploverlay_Goggles01::~Ploverlay_Goggles01 ( void )
{
	//delete mMaterial;
	delete m_screenshader;
}

void Ploverlay_Goggles01::Update ( void )
{
	// Not a whole lot needing update
}

bool Ploverlay_Goggles01::Render ( const char pass )
{
	transform.position = Vector3d(0,0,0);

	GL_ACCESS;
	GLd_ACCESS;

	GL.beginOrtho();
	GLd.DrawSet2DScaleMode();

	mMaterial->bindPass(0);
	mMaterial->setShaderConstants( this );
	//mMaterial->setUniform( "gm_icingAmount", fabsf( sinf( Time::currentTime ) ) );
	//mMaterial->setUniform( "gm_icingAmount", sinf( Time::currentTime )*(1.5f)+0.5f );
	//mMaterial->setUniform( "gm_icingAmount", mIcingValue );
	GLd.DrawRectangleA( 0,0, 1,1 );
	
	GL.endOrtho();

	return true;
}