
#include "CTerrainLoadscreen.h"
#include "renderer/texture/CBitmapFont.h"
#include "after/terrain/VoxelTerrain.h"

CTerrainLoadscreen::CTerrainLoadscreen ( void )
	: CGameBehavior(), CRenderableObject()
{
/*	renderType = Renderer::V2D;

	bgMaterial = new glMaterial();
	bgMaterial->useLighting = false;
	bgMaterial->diffuse = Color( 0.0f, 0,0, 0.9f );

	barMaterial = new glMaterial();
	barMaterial->useLighting = false;
	//barMaterial->diffuse = Color( 0.3f,0.3f,0.8f );
	barMaterial->diffuse = Color( 1.0f,1.0f,1.0f );

	fntLoaderText	= new CBitmapFont ( "monofonto.ttf", 14, FW_NORMAL );
	matFntLoader = new glMaterial;
	matFntLoader->diffuse = Color( 1.0f,1,1 );
	matFntLoader->useColors = true;
	matFntLoader->useLighting = false;
	matFntLoader->useTexture = true;

	currentPercentage = 0.0f;*/
}

CTerrainLoadscreen::~CTerrainLoadscreen ( void )
{
/*	delete bgMaterial;
	delete barMaterial;*/
}


void CTerrainLoadscreen::Update ( void )
{

}
bool CTerrainLoadscreen::Render ( const char pass )
{
	return false;
/*	CVoxelTerrain* activeTerrain = CVoxelTerrain::GetActive();
	if ( activeTerrain )
	{
		//ftype targetPercentage = activeTerrain->GetLoadingPercent()*(CBoobCollision::GetCurrentReferenceCount()/ftype(CBoobCollision::GetTargetReferenceCount()));
		ftype targetPercentage = activeTerrain->GetLoadingPercent();
		currentPercentage += (targetPercentage-currentPercentage)*0.1f;

		GL.beginOrtho();
			GLd.DrawSet2DScaleMode();
			GLd.DrawSet2DMode( GLd.D2D_FLAT );
			bgMaterial->bind();
				GLd.DrawRectangleA( 0,0, 1,1 );
			//bgMaterial->unbind();

			barMaterial->bind();
				GLd.DrawSet2DMode( GL.D2D_WIRE );
				GLd.DrawRectangleA( 0.1f,0.5f, 0.8f,0.06f );
				GLd.DrawSet2DMode( GLd.D2D_FLAT );
				GLd.DrawRectangleA( 0.1f,0.5f, 0.8f * currentPercentage ,0.06f );
			//barMaterial->unbind();

			matFntLoader->bind();
			fntLoaderText->Set();
				GLd.DrawAutoText( 0.1f, 0.10f, "load list size: %d", activeTerrain->GetLoadingListSize() );
				GLd.DrawAutoText( 0.1f, 0.15f, "loading collision: %d/%d", CBoobCollision::GetCurrentReferenceCount(), CBoobCollision::GetTargetReferenceCount() );
				GLd.DrawAutoText( 0.1f, 0.20f, "generating area: %d/%d", activeTerrain->GetGenerationListSize(), CBoobCollision::GetTargetReferenceCount() );
			fntLoaderText->Unbind();
			matFntLoader->unbind();

		GL.endOrtho();
	}
	return true;*/
}