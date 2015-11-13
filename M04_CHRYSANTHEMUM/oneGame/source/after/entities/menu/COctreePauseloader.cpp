
#include "COctreePauseloader.h"
#include "renderer/texture/CBitmapFont.h"
//#include "COctreeTerrain.h"
#include "after/terrain/Zones.h"
#include "renderer/material/glMaterial.h"
#include "renderer/system/glMainSystem.h"
#include "renderer/system/glDrawing.h"

COctreePauseloader::COctreePauseloader ( void )
	: CGameBehavior(), CRenderableObject()
{
	renderSettings.renderHints = RL_WORLD;
	renderType = Renderer::V2D;

	bgMaterial = new glMaterial();
	bgMaterial->passinfo.push_back( glPass() );
	bgMaterial->passinfo[0].m_lighting_mode	= Renderer::LI_NONE;
	bgMaterial->passinfo[0].shader = new glShader( ".res/shaders/v2d/default.glsl" );
	bgMaterial->setTexture( 0, new CTexture( "null" ) );
	bgMaterial->m_diffuse = Color( 1,1,1 );
	SetMaterial( bgMaterial );

	barMaterial = new glMaterial();
	barMaterial->passinfo.push_back( glPass() );
	barMaterial->passinfo[0].m_lighting_mode	= Renderer::LI_NONE;
	barMaterial->passinfo[0].shader = new glShader( ".res/shaders/v2d/default.glsl" );
	barMaterial->setTexture( 0, new CTexture( "null" ) );
	barMaterial->m_diffuse = Color( 0.5,0.5,0.5 );

	fntLoaderText	= new CBitmapFont ( "monofonto.ttf", 14, FW_NORMAL );
	matFntLoader = new glMaterial;
	matFntLoader->m_diffuse = Color( 0.3,0.3,0.3 );
	matFntLoader->passinfo.push_back( glPass() );
	matFntLoader->passinfo[0].m_lighting_mode	= Renderer::LI_NONE;
	matFntLoader->passinfo[0].shader = new glShader( ".res/shaders/v2d/default.glsl" );
	matFntLoader->setTexture( 0, fntLoaderText );

	currentPercentage = 0.0f;
	targetPercentage = 0;
	displayTime = 0;
	slowDisplayAlpha = 0;
	displayAlpha = 0;

	m_targetLoadcheckPosition = Vector3d( 1,1,1 );
}

COctreePauseloader::~COctreePauseloader ( void )
{
	SetMaterial( NULL );
	bgMaterial->removeReference();
	delete bgMaterial;
	barMaterial->removeReference();
	delete barMaterial;
}

void COctreePauseloader::Reset ( void )
{
	currentPercentage = 0.0f;
	targetPercentage = 0;
	displayTime = 0;
	slowDisplayAlpha = 0;
	displayAlpha = 0;
}

void COctreePauseloader::Update ( void )
{
	targetPercentage = 0;
	// Just check start area for collision. That's what really matters.
	int matches = 0;
	int checks = 0;
	const int checkRange = 1;
	for ( int x = -checkRange; x <= checkRange; ++x )
	{
		for ( int y = -checkRange; y <= checkRange; ++y )
		{
			for ( int z = -checkRange; z <= checkRange; ++z )
			{
				if ( Zones.IsCollidableArea( m_targetLoadcheckPosition + Vector3d((Real)x,(Real)y,(Real)z)*64.0F ) ) {
					matches += 1;
				}
				checks += 1;
			}
		}
	}
	targetPercentage = matches/Real(checks) + 0.01F;

	currentPercentage += (targetPercentage-currentPercentage)*Time::TrainerFactor(0.2F);


	displayTime += Time::deltaTime;
	if ( displayTime > 5.0 ) {
		slowDisplayAlpha += Time::deltaTime * 0.5f;
		if ( slowDisplayAlpha > 1.0 ) {
			slowDisplayAlpha = 1;
		}
	}
	if ( displayTime > 1.0 ) {
		displayAlpha += Time::deltaTime;
		if ( displayAlpha > 1.0 ) {
			displayAlpha = 1;
		}	
	}
}
bool COctreePauseloader::GetDoneLoad ( void )
{
	return targetPercentage>1.00;
}
bool COctreePauseloader::Render ( const char pass )
{
	GL_ACCESS;
	GLd_ACCESS;
	GL.beginOrtho();
		//GL.Translate( Vector3d( 0,0,1 ) );

		GLd.DrawSet2DScaleMode();
		GLd.DrawSet2DMode( GLd.D2D_FLAT );
		bgMaterial->bindPass(0);
			GLd.DrawRectangleA( 0,0, 1,1 );

		barMaterial->m_diffuse.alpha = displayAlpha;
		barMaterial->bindPass(0);
			GLd.DrawSet2DMode( GLd.D2D_WIRE );
			GLd.DrawRectangleA( 0.1f,0.5f, 0.8f,0.06f );
			GLd.DrawSet2DMode( GLd.D2D_FLAT );
			GLd.DrawRectangleA( 0.1f,0.5f, 0.8f * currentPercentage ,0.06f );

		matFntLoader->m_diffuse.alpha = displayAlpha;
		matFntLoader->bindPass(0);
			GLd.DrawAutoTextCentered( 0.5f,0.6f, "Loading World" );

		if ( slowDisplayAlpha > 0 ) {
			matFntLoader->m_diffuse.alpha = slowDisplayAlpha;
			matFntLoader->bindPass(0);
				GLd.DrawAutoTextCentered( 0.5f,0.65f, "(this will take a while the first time)" );
			matFntLoader->m_diffuse.alpha = 1;
		}

	GL.endOrtho();
	return true;
}