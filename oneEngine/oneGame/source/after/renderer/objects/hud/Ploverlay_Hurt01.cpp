
#include "Ploverlay_Hurt01.h"
#include "core/time/time.h"
#include "renderer/texture/CTexture.h"
#include "renderer/material/glMaterial.h"
#include "renderer/system/glMainSystem.h"
#include "renderer/system/glDrawing.h"

Ploverlay_Hurt01::Ploverlay_Hurt01 ( void )
	: CRenderableObject(), CGameBehavior()
{
	mHurtValue = 0.5f;

	renderSettings.renderHints = RL_WORLD;
	renderType = Renderer::V2D;

	mMaterial = new glMaterial;
	mMaterial->m_diffuse = Color( 1,1,1,1 );
	mMaterial->setTexture( 0, new CTexture( ".res/textures/hud/overlay_hurt01.png" ) );
	mMaterial->passinfo.push_back( glPass() );
	mMaterial->passinfo[0].m_transparency_mode = Renderer::ALPHAMODE_TRANSLUCENT;
	mMaterial->passinfo[0].shader = new glShader( ".res/shaders/v2d/default.glsl" );
	mMaterial->removeReference();

	SetMaterial( mMaterial );
}

Ploverlay_Hurt01::~Ploverlay_Hurt01 ( void )
{
	//delete mMaterial;
}

void Ploverlay_Hurt01::Update ( void )
{
	mHurtValue -= Time::smoothDeltaTime;
	if ( mHurtValue <= 0.0 ) {
		DeleteObject( this );
	}
}

bool Ploverlay_Hurt01::Render ( const char pass )
{
	transform.position = Vector3d(0,0,0);

	GL_ACCESS;
	GLd_ACCESS;

	GL.beginOrtho();
	GLd.DrawSet2DScaleMode();

	mMaterial->m_diffuse.alpha = std::min<ftype>( mHurtValue, 0.9f );
	mMaterial->bindPass(0);
	mMaterial->setShaderConstants( this );
	GLd.DrawRectangleA( 0,0, 1,1 );
	
	GL.endOrtho();

	return true;
}