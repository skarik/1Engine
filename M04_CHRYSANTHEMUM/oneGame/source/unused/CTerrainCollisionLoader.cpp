
#include "CTerrainCollisionLoader.h"
#include "after/terrain/VoxelTerrain.h"
#include "renderer/texture/CBitmapFont.h"
//#include "CBoobCollision.h"
#include "renderer/material/glMaterial.h"
#include "renderer/system/glMainSystem.h"
#include "renderer/system/glDrawing.h"

CTerrainCollisionLoader::CTerrainCollisionLoader ( void )
	: CGameBehavior(), CRenderableObject()
{
	iOffset = 0;
	//bContinueThread = true;

	//sCollisionLoader newTStruct;
	//newTStruct.caller = this;
	//mtThread = thread( newTStruct );

	renderType = Renderer::V2D;

	fntDraw	= new CBitmapFont ( "monofonto.ttf", 14, FW_BOLD );
	matFont = new glMaterial;
	matFont->m_diffuse = Color( 1.0f,1,1 );
	matFont->passinfo.push_back( glPass() );
	matFont->passinfo[0].m_lighting_mode	= Renderer::LI_NONE;
	matFont->setTexture( 0, fntDraw );

	drawAlpha = 1;
}

CTerrainCollisionLoader::~CTerrainCollisionLoader( void )
{
	bContinueThread = false;
	//mtThread.join();

	delete matFont;
	delete fntDraw;
}

void CTerrainCollisionLoader::Update ( void )
{
	++iOffset;
	if ( iOffset%4 == 0 )
		CBoobCollision::PreloadCollision();

	if ( drawAlpha > 0 ) {
		visible = true;
	}
	else {
		visible = false;
	}
}

bool CTerrainCollisionLoader::Render ( const char pass )
{
	if ( pass != 0 )
		return false;

	int calcCurCount, calcMaxCount;
	calcMaxCount = CBoobCollision::GetTargetReferenceCount();
	calcCurCount = CBoobCollision::GetCurrentReferenceCount();
	if ( calcCurCount >= calcMaxCount ) {
		drawAlpha -= Time::deltaTime;
	}
	else {
		drawAlpha = sin( Time::currentTime*3.2f )*0.3f + 0.7f;
	}

	GL_ACCESS;
	GLd_ACCESS;
	
	GL.beginOrtho();

	matFont->m_diffuse.alpha = drawAlpha;
	matFont->bindPass(0);
		GLd.DrawAutoText( 0.01f,0.95f,"preloading: %d/%d", calcCurCount, calcMaxCount );

	GL.endOrtho();

	return true;
}



// Below is unused
void CTerrainCollisionLoader::TerraLoader ( void )
{
	while ( bContinueThread ) {
		++iOffset;
		if ( iOffset%4 == 0 )
			CBoobCollision::PreloadCollision();
	}
}

void CTerrainCollisionLoader::sCollisionLoader::operator() ( void )
{
	caller->TerraLoader();
}
