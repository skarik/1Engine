// The CWorldCartographer has routines to render one-shot maps of the target world.
// The cartographer assumes the proper realm has already been set.

#include "CWorldCartographer.h"
//#include "after/terrain/generation/regions/CTownController.h"

#include "core/system/Screen.h"
#include "core/types/ModelData.h"

#include "renderer/texture/CRenderTexture.h"

#include "renderer/system/glMainSystem.h"
#include "renderer/system/glDrawing.h"

#include "renderer/material/glMaterial.h"
#include "renderer/texture/CBitmapFont.h"

#include "after/terrain/generation/CWorldGenerator.h"
#include "after/states/world/CMCCPlanet.h"
#include "after/types/terrain/TerrainTypes.h"
#include "after/types/terrain/BiomeTypes.h"
#include "after/terrain/generation/regions/CRegionGenerator.h"
#include "after/states/world/ProvinceManager.h"

#include "after/terrain/generation/patterns/CPatternController.h"

using std::list;
using std::vector;
using std::pair;

CWorldCartographer::CWorldCartographer( const char* planetName, CRenderTexture* renderTarget )
	: m_targetPlanet( planetName ), m_generator( NULL )
{
	m_planet	= new CMCCPlanet( m_targetPlanet );
	m_rt		= renderTarget;

	r_pos		= Vector2d(0,0);
	r_halfsize	= Vector2d(2048,2048);

	m_max_x = 64;
	m_max_y = 64;

	verts = new CTerrainVertex [m_max_x*m_max_y];

	// Set vertex positions across the render target
	for ( uint x = 0; x < m_max_x; ++x )
	{
		for ( uint y = 0; y < m_max_y; ++y )
		{
			CTerrainVertex& vert = verts[x+(y*m_max_x)];
			// Set vertex position
			vert.x = x/Real(m_max_x-1) * m_rt->GetWidth(); 
			vert.y = (1-(y/Real(m_max_y-1))) * m_rt->GetHeight();
			vert.z = 0.0f;
			// Set vertex as invisible
			vert.r = 0.4f;
			vert.g = 0.6f;
			vert.b = 0.5f;
			vert.a = 0.0f;
		}
	}

	m_font = NULL;
}

void CWorldCartographer::ForceReload ( void )
{
	delete m_planet;
	m_planet = new CMCCPlanet( m_targetPlanet );
}


CWorldCartographer::~CWorldCartographer ( void )
{
	delete m_planet;
	delete [] verts;
}

void CWorldCartographer::GiveGenerator ( Terrain::CWorldGenerator* n_generator )
{
	m_generator = n_generator;
}



//	RenderTerrainMap
// Renders out a map with colors indicating the dominant terrain at the position.
void CWorldCartographer::RenderTerrainMap ( void )
{
	// Query planet for data to build map. Sample at discrete points across the halfsize.
	Vector2d samplePoint;
	for ( uint x = 0; x < 64; ++x )
	{
		for ( uint y = 0; y < 64; ++y )
		{
			CTerrainVertex& vert = verts[x+(y*64)];

			// Convert iteration into point
			Vector2d samplePoint ( r_pos.x + ((x/32.0f) - 1)*r_halfsize.x, r_pos.y + ((y/32.0f) - 1)*r_halfsize.y );
			
			// Sample
			SetTerrainVertex( vert, samplePoint );
		}
	}
	RenderVerts();
}
void CWorldCartographer::StutterRenderTerrainMap ( sRenderProgressState & renderState )
{
	// Don't update past max point.
	if ( renderState.yindex >= 64 ) {
		renderState.done = true;
		return;
	}
	renderState.done = false;

	// Get vert for this target
	CTerrainVertex& vert = verts[renderState.xindex+(renderState.yindex*64)];

	// Set vertex position
	Vector2d samplePoint ( r_pos.x + ((renderState.xindex/32.0f) - 1)*r_halfsize.x, r_pos.y + ((renderState.yindex/32.0f) - 1)*r_halfsize.y );
	// Sample
	SetTerrainVertex( vert, samplePoint );

	// Render out
	RenderVerts();

	// Increment render state
	renderState.xindex += 1;
	if ( renderState.xindex >= 64 ) {
		renderState.yindex += 1;
		renderState.xindex = 0;
	}
}
// Renders out a world texture map a step at a time
void CWorldCartographer::StutterRenderWorldTextureMap ( sRenderProgressState & renderState )
{
	// Don't update past max point.
	if ( renderState.yindex >= m_max_y ) {
		renderState.done = true;
		return;
	}
	renderState.done = false;

	// Get vert for this target
	CTerrainVertex& vert = verts[renderState.xindex+(renderState.yindex*m_max_x)];

	// Set vertex position
	//Vector2d samplePoint ( r_pos.x + ((renderState.xindex/32.0f) - 1)*r_halfsize.x, r_pos.y + ((renderState.yindex/32.0f) - 1)*r_halfsize.y );
	Vector2d_d samplePoint ( (renderState.xindex/Real_d(m_max_x))*m_generator->GetWorldSize().x, (renderState.yindex/Real_d(m_max_y))*m_generator->GetWorldSize().y );
	// Sample
	//SetTerrainVertex( vert, samplePoint );
	bool isLand = m_generator->GetInland( samplePoint );
	if ( isLand ) {
		vert.a = 1.00f;
		vert.r = 0.71f;
		vert.g = 0.65f;
		vert.b = 0.23f;
	}
	else {
		vert.a = 1.00f;
		vert.r = 0.04f;
		vert.g = 0.12f;
		vert.b = 0.62f;
	}

	// Render out
	RenderVerts();

	// Increment render state
	renderState.xindex += 1;
	if ( renderState.xindex >= m_max_x ) {
		renderState.yindex += 1;
		renderState.xindex = 0;
	}
}
void CWorldCartographer::SetTerrainVertex ( CTerrainVertex & vert, const Vector2d& samplePoint )
{
	// Sample at point
	World::terrainSampleQuery_t data;
	if ( m_planet->GetDataAt( samplePoint, &data ) )
	{
		vert.a = 1.0f;

		// Depending on terrain, change vertex color
		switch ( data.terra ) {
			case Terrain::TER_OCEAN:
				vert.r = .176f;	vert.g = .605f;	vert.b = .746f;
				break;
			case Terrain::TER_ISLANDS:
			case Terrain::TER_SPIRES:
				vert.r = .320f;	vert.g = .672f;	vert.b = .684f;
				break;
			case Terrain::TER_FLATLANDS:
				vert.r = .133f;	vert.g = .691f;	vert.b = .297f;
				break;
			case Terrain::TER_HILLLANDS:
				if ( data.elevation < 6.4f ) {
					vert.r = .266f;	vert.g = .555f;	vert.b = .313f;
				}
				else {
					vert.r = .426f;	vert.g = .586f;	vert.b = .438f;
				}
				break;
			case Terrain::TER_MOUNTAINS:
				vert.r = .586f;	vert.g = .617f;	vert.b = .586f;
				break;
			case Terrain::TER_DESERT:
				vert.r = .902f;	vert.g = .801f;	vert.b = .227f;
				break;
			case Terrain::TER_BADLANDS:
				vert.r = .848f;	vert.g = .617f;	vert.b = .148f;
				break;
			case Terrain::TER_THE_EDGE:
				vert.r = 0.0f;	vert.g = 0.0f;	vert.b = 0.0f;
				break;
			default:
				vert.r = .781f;	vert.g = .320f;	vert.b = .102f;
				break;
		}
		// Depending on biome, change vertex color
		switch ( data.biome ) {
			case Terrain::BIO_TAIGA:
			case Terrain::BIO_TUNDRA:
				vert.r += 0.125f;	vert.g += 0.125f;	vert.b += 0.195f;
				break;
			case Terrain::BIO_COLD_DESERT:
				vert.b += 0.094f;
				break;
			case Terrain::BIO_S_TUNDRA:
				vert.r += 0.219f;	vert.g += 0.188f;	vert.b += .297f;
				break;
		}
	}
	else
	{
		// If no data, then don't draw shit
		vert.r = .176f;	vert.g = .605f;	vert.b = .746f;
		vert.a = 0.0f;
	}
}

//	RenderRegionMap
// Renders out a map showing delimited regions.
//#include "CRegionManager.h"
void CWorldCartographer::RenderRegionMap ( void )
{
	// Query planet for data to build map. Sample at discrete points across the halfsize.
	Vector2d samplePoint;
	for ( uint x = 0; x < 64; ++x )
	{
		for ( uint y = 0; y < 64; ++y )
		{
			CTerrainVertex& vert = verts[x+(y*64)];

			// Convert iteration into point
			Vector2d samplePoint ( r_pos.x + ((x/32.0f) - 1)*r_halfsize.x, r_pos.y + ((y/32.0f) - 1)*r_halfsize.y );
			// Sample at point
			uint32_t region = World::ProvinceManager->GetRegion( rangeint(samplePoint.x/64.0f), rangeint(samplePoint.y/64.0f) );

			if (( region != uint32_t(-2) )&&( region != uint32_t(-1) )) {
				vert.r = (region%3)*0.5f;	vert.g = ((region/3)%3)*0.5f;	vert.b = ((region/9)%3)*0.5f;
				vert.a = 1.0f;
			}
			else {
				vert.r = .176f;	vert.g = .605f;	vert.b = .746f;
				vert.a = 0.0f;
			}
		}
	}

	RenderVerts();
}

//	RenderAreaMap
// Renders out a map that shows general terrain and province borders.
void CWorldCartographer::RenderAreaMap ( void )
{
	GL_ACCESS;
	GLd_ACCESS;
	// Create draw material
	glMaterial tempMat;
	{
		tempMat.passinfo.push_back( glPass() );
		tempMat.passinfo[0].m_blend_mode = Renderer::BM_NORMAL;
		tempMat.passinfo[0].m_lighting_mode = Renderer::LI_NONE;
		tempMat.passinfo[0].m_transparency_mode = Renderer::ALPHAMODE_TRANSLUCENT;
		tempMat.passinfo[0].m_face_mode = Renderer::FM_FRONTANDBACK;
	}
	// Create font material
	glMaterial fontMat;
	{
		if ( !m_font ) m_font = new CBitmapFont( "gothic_bozo.ttf", 22 );
		fontMat.setTexture( 0, m_font );
		fontMat.passinfo[0].m_blend_mode = Renderer::BM_NORMAL;
		fontMat.passinfo[0].m_lighting_mode = Renderer::LI_NONE;
		fontMat.passinfo[0].m_transparency_mode = Renderer::ALPHAMODE_TRANSLUCENT;
		fontMat.passinfo[0].m_face_mode = Renderer::FM_FRONTANDBACK;
	}

	// Render to texture
	{
		m_rt->BindBuffer();
		GL.setupViewport( 0,0, m_rt->GetWidth(), m_rt->GetHeight() );
		GL.beginOrtho( 0,0, (ftype)m_rt->GetWidth(), (ftype)m_rt->GetHeight(), -45,45, false );
	}

	// Clear render target.
	GL.clearColor( Color( 0.3f, 0.3f, 0.5f, 0.0f ) );
	GL.ClearBuffer( GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT );

	{
		// Query planet for data to build map. Sample at discrete points across the halfsize.
		Vector2d samplePoint;
		for ( uint x = 0; x < 64; ++x )
		{
			for ( uint y = 0; y < 64; ++y )
			{
				CTerrainVertex& vert = verts[x+(y*64)];

				// Convert iteration into point
				Vector2d samplePoint ( r_pos.x + ((x/32.0f) - 1)*r_halfsize.x, r_pos.y + ((y/32.0f) - 1)*r_halfsize.y );
			
				// Sample
				SetTerrainVertex( vert, samplePoint );
			}
		}
		// Draw the prim
		tempMat.bindPass(0);
		GLd.BeginPrimitive( GL_QUADS );
		for ( uint x = 0; x < 63; ++x )
		{
			for ( uint y = 0; y < 63; ++y )
			{
				CTerrainVertex vert;
				vert = verts[x+(y*64)];
				GLd.P_PushColor( vert.r, vert.g, vert.b, vert.a );
				GLd.P_AddVertex( vert.x, vert.y, vert.z );

				vert = verts[x+1+(y*64)];
				GLd.P_PushColor( vert.r, vert.g, vert.b, vert.a );
				GLd.P_AddVertex( vert.x, vert.y, vert.z );

				vert = verts[x+1+((y+1)*64)];
				GLd.P_PushColor( vert.r, vert.g, vert.b, vert.a );
				GLd.P_AddVertex( vert.x, vert.y, vert.z );

				vert = verts[x+((y+1)*64)];
				GLd.P_PushColor( vert.r, vert.g, vert.b, vert.a );
				GLd.P_AddVertex( vert.x, vert.y, vert.z );
			}
		}
		GLd.EndPrimitive();
		//tempMat.unbind();
	}
	{
		// Query planet for data to build map. Sample at discrete points across the halfsize.
		Vector2d samplePoint;
		for ( uint x = 0; x < 64; ++x )
		{
			for ( uint y = 0; y < 64; ++y )
			{
				CTerrainVertex& vert = verts[x+(y*64)];

				// Convert iteration into point
				Vector2d samplePoint ( r_pos.x + ((x/32.0f) - 1)*r_halfsize.x, r_pos.y + ((y/32.0f) - 1)*r_halfsize.y );
				// Sample at point
				uint32_t region = World::ProvinceManager->GetRegion( rangeint(samplePoint.x/64.0f), rangeint(samplePoint.y/64.0f) );

				if (( region != uint32_t(-2) )&&( region != uint32_t(-1) )) {
					vert.r = (region%3)*0.5f;	vert.g = ((region/3)%3)*0.5f;	vert.b = ((region/9)%3)*0.5f;
					vert.a = 0.2f;
				}
				else {
					vert.r = .176f;	vert.g = .605f;	vert.b = .746f;
					vert.a = 0.0f;
				}
			}
		}
		// Draw the prim
		tempMat.bindPass(0);
		GLd.BeginPrimitive( GL_QUADS );
		for ( uint x = 0; x < 63; ++x )
		{
			for ( uint y = 0; y < 63; ++y )
			{
				CTerrainVertex vert;
				vert = verts[x+(y*64)];
				GLd.P_PushColor( vert.r, vert.g, vert.b, vert.a );
				GLd.P_AddVertex( vert.x, vert.y, vert.z );

				vert = verts[x+1+(y*64)];
				GLd.P_PushColor( vert.r, vert.g, vert.b, vert.a );
				GLd.P_AddVertex( vert.x, vert.y, vert.z );

				vert = verts[x+1+((y+1)*64)];
				GLd.P_PushColor( vert.r, vert.g, vert.b, vert.a );
				GLd.P_AddVertex( vert.x, vert.y, vert.z );

				vert = verts[x+((y+1)*64)];
				GLd.P_PushColor( vert.r, vert.g, vert.b, vert.a );
				GLd.P_AddVertex( vert.x, vert.y, vert.z );
			}
		}
		GLd.EndPrimitive();
		//tempMat.unbind();
	}

	/*const int casemap [32][5] = {
		{-1},
		{0,3,-1},		//1
		{1,0,-1},		//2
		{1,3,-1},		//3
		{3,2,-1},		//4
		{0,2,-1},		//5
		{3,2,1,0,-1},	//6
		{1,2,-1},		//7
		{2,1,-1},		//8, flip 7
		{2,1,0,3,-1},	//9, opposite 6
		{2,0,-1},		//10, flip 5
		{2,3,-1},		//11, flip 4
		{3,1,-1},		//12, flip 3
		{0,1,-1},		//13, flip 2
		{3,0,-1},		//14, flip 1
		{-1}, //Following are specific for borders
		{},	//0
		{},	//1
		{}, //2
		{0,2,1,4,-1},	//3
		{},	//4
		{1,3,2,4,-1},	//5
		{}, //6
		{}, //7
		{}, //8
		{}, //9
		{2,0,3,4,-1}, //10
		{}, //11
		{3,1,0,4,-1}, //12
		{}, //13
		{}, //14
		{0,2,1,3,-1} //15
	};*/

	const int casemap [16][5] = {
		{-1},
		{0,3,-1},		//1
		{1,0,-1},		//2
		{1,3,-1},		//3
		{3,2,-1},		//4
		{0,2,-1},		//5
		{3,2,1,0,-1},	//6
		{1,2,-1},		//7
		{2,1,-1},		//8, flip 7
		{2,1,0,3,-1},	//9, opposite 6
		{2,0,-1},		//10, flip 5
		{2,3,-1},		//11, flip 4
		{3,1,-1},		//12, flip 3
		{0,1,-1},		//13, flip 2
		{3,0,-1},		//14, flip 1
		{-1}
	};

	Vector2d samplePoint;
	Vector2d samplePointOffset ( (0.5f/32.0f)*r_halfsize.x, (0.5f/32.0f)*r_halfsize.y );
	//Vector2d samplePointOffset ( (1/32.0f)*r_halfsize.x, (1/32.0f)*r_halfsize.y );

	// First, render borders
	// Draw the prim
	tempMat.bindPass(0);
	GLd.SetLineWidth(3.0f);
	GLd.BeginPrimitive( GL_LINES );
	GLd.P_PushColor( 0.1f,0.1f,0.1f,1.0f );
	struct regioninfo_t {
		int count;
		int bordercount;
		Vector2d center;
		regioninfo_t ( void ) : count(0), bordercount(0), center(0,0) {}
	};
	vector<pair<uint32_t,regioninfo_t>> usedRegions;
	uint32_t regionInfo [4096];
	for ( int x = 0; x < 64; ++x )
	{
		for ( int y = 0; y < 64; ++y )
		{
			// Convert iteration into point
			samplePoint = Vector2d( r_pos.x + ((x-32)/32.0f)*r_halfsize.x, r_pos.y + ((y-32)/32.0f)*r_halfsize.y );
			uint32_t region = World::ProvinceManager->GetRegion( rangeint(samplePoint.x/64.0f), rangeint(samplePoint.y/64.0f) );
			regionInfo[x+y*64] = region;
			/*
			// Convert iteration into point
			samplePoint = Vector2d( r_pos.x + ((x-32)/32.0f)*r_halfsize.x, r_pos.y + ((y-32)/32.0f)*r_halfsize.y );

			uint32_t regionSamples [4];
			regionSamples[0] = World::ActiveRegionManager->GetRegion( rangeint((samplePoint.x-samplePointOffset.x)/64.0f), rangeint((samplePoint.y-samplePointOffset.y)/64.0f) );
			regionSamples[1] = World::ActiveRegionManager->GetRegion( rangeint((samplePoint.x+samplePointOffset.x)/64.0f), rangeint((samplePoint.y-samplePointOffset.y)/64.0f) );
			regionSamples[2] = World::ActiveRegionManager->GetRegion( rangeint((samplePoint.x-samplePointOffset.x)/64.0f), rangeint((samplePoint.y+samplePointOffset.y)/64.0f) );
			regionSamples[3] = World::ActiveRegionManager->GetRegion( rangeint((samplePoint.x+samplePointOffset.x)/64.0f), rangeint((samplePoint.y+samplePointOffset.y)/64.0f) );

			char caseType = 1;
			// default 2-volume cases
			if ( regionSamples[1] == regionSamples[0] ) caseType |= 2;
			if ( regionSamples[2] == regionSamples[0] ) caseType |= 4;
			if ( regionSamples[3] == regionSamples[0] ) caseType |= 8;
			// multiple volume cases (T's and single X)
			if ( caseType == 3 && regionSamples[2] != regionSamples[3] ) caseType |= 16;
			if ( caseType == 5 && regionSamples[1] != regionSamples[3] ) caseType |= 16;
			if ( caseType == 10 && regionSamples[0] != regionSamples[2] ) caseType |= 16;
			if ( caseType == 12 && regionSamples[0] != regionSamples[1] ) caseType |= 16;
			if ( caseType == 1 && regionSamples[1] != regionSamples[2] && regionSamples[2] != regionSamples[3] ) caseType = 31;

			Vector2d verts [5];
			verts[0] = Vector2d( (x+0.5f)/63.0f, y/63.0f );
			verts[1] = Vector2d( (x+1.0f)/63.0f, (y+0.5f)/63.0f );
			verts[2] = Vector2d( (x+0.5f)/63.0f, (y+1.0f)/63.0f );
			verts[3] = Vector2d( x/63.0f, (y+0.5f)/63.0f );
			verts[4] = Vector2d( (x+0.5f)/63.0f, (y+0.5f)/63.0f );

			int i = 0;
			while ( casemap[caseType][i] != -1 ) {
				glVertex2f( verts[casemap[caseType][i]].x * m_rt->GetWidth(), verts[casemap[caseType][i]].y * m_rt->GetHeight() );
				++i;
			}
			*/

			// Convert iteration into point
			/*samplePoint = Vector2d( r_pos.x + ((x-32)/32.0f)*r_halfsize.x, r_pos.y + ((y-32)/32.0f)*r_halfsize.y );

			uint32_t regionSamples [5];
			regionSamples[0] = World::ActiveRegionManager->GetRegion( rangeint((samplePoint.x)/64.0f), rangeint((samplePoint.y)/64.0f) );
			regionSamples[1] = World::ActiveRegionManager->GetRegion( rangeint((samplePoint.x+samplePointOffset.x)/64.0f), rangeint((samplePoint.y)/64.0f) );
			regionSamples[2] = World::ActiveRegionManager->GetRegion( rangeint((samplePoint.x)/64.0f), rangeint((samplePoint.y+samplePointOffset.y)/64.0f) );
			regionSamples[3] = World::ActiveRegionManager->GetRegion( rangeint((samplePoint.x-samplePointOffset.x)/64.0f), rangeint((samplePoint.y)/64.0f) );
			regionSamples[4] = World::ActiveRegionManager->GetRegion( rangeint((samplePoint.x)/64.0f), rangeint((samplePoint.y-samplePointOffset.y)/64.0f) );

			Vector2d verts [4];
			verts[0] = Vector2d( x/63.0f, y/63.0f );
			verts[1] = Vector2d( (x+1)/63.0f, y/63.0f );
			verts[2] = Vector2d( (x+1)/63.0f, (y+1)/63.0f );
			verts[3] = Vector2d( x/63.0f, (y+1)/63.0f );

			const char casemap [4][3] = {
				{1,2,-1},
				{2,3,-1},
				{3,0,-1},
				{0,1,-1}
			};

			for ( int k = 0; k < 4; ++k )
			{
				if ( regionSamples[0] != regionSamples[1+k] )
				{
					int i = 0;
					while ( casemap[k][i] != -1 ) {
						glVertex2f( verts[casemap[k][i]].x * m_rt->GetWidth(), verts[casemap[k][i]].y * m_rt->GetHeight() );
						++i;
					}
				}
			}*/
		}
	}
	for ( int x = 0; x < 64; ++x )
	{
		for ( int y = 0; y < 64; ++y )
		{
			uint32_t region = regionInfo[x+y*64];

			bool found = false;
			for ( uint i = 0; i < usedRegions.size(); ++i ) {
				if ( usedRegions[i].first == region ) {
					found = true;
					usedRegions[i].second.count++;
					usedRegions[i].second.center += Vector2d( x,y );
					if (( x > 0 && regionInfo[(x-1)+y*64] == region )||
						( y > 0 && regionInfo[x+(y-1)*64] == region )||
						( x < 63 && regionInfo[(x+1)+y*64] == region )||
						( x < 63 && regionInfo[x+(y+1)*64] == region ))
					{
						usedRegions[i].second.bordercount++;
						break;
					}
				}
			}
			if ( !found ) {
				usedRegions.push_back( pair<uint32_t,regioninfo_t>(region,regioninfo_t()) );
			}
		}
	}
	for ( uint i = 0; i < usedRegions.size(); ++i )
	{
		usedRegions[i].second.center /= usedRegions[i].second.count;
		if ( usedRegions[i].second.bordercount > 13 )
		{
			for ( int x = 0; x < 64; ++x )
			{
				for ( int y = 0; y < 64; ++y )
				{
					uint32_t regionSamples [4];
					regionSamples[0] = regionInfo[x+y*64];
					regionSamples[1] = regionInfo[(x+1)+y*64];
					regionSamples[2] = regionInfo[x+(y+1)*64];
					regionSamples[3] = regionInfo[(x+1)+(y+1)*64];

					char caseType = 0;
					// default 2-volume cases
					if ( regionSamples[0] == usedRegions[i].first ) caseType |= 1;
					if ( regionSamples[1] == usedRegions[i].first ) caseType |= 2;
					if ( regionSamples[2] == usedRegions[i].first ) caseType |= 4;
					if ( regionSamples[3] == usedRegions[i].first ) caseType |= 8;

					Vector2d verts [4];
					verts[0] = Vector2d( (x+0.5f)/64.0f, y/64.0f );
					verts[1] = Vector2d( (x+1.0f)/64.0f, (y+0.5f)/64.0f );
					verts[2] = Vector2d( (x+0.5f)/64.0f, (y+1.0f)/64.0f );
					verts[3] = Vector2d( x/64.0f, (y+0.5f)/64.0f );

					int i = 0;
					while ( casemap[caseType][i] != -1 ) {
						GLd.P_AddVertex( verts[casemap[caseType][i]].x * m_rt->GetWidth(), (1-verts[casemap[caseType][i]].y) * m_rt->GetHeight() );
						++i;
					}
				}
			}
		}
	}
	GLd.EndPrimitive();
	GLd.SetLineWidth();

	tempMat.bindPass(0);
	GLd.BeginPrimitive( GL_LINES );
	GLd.P_PushColor( 0.1f,0.1f,0.1f,1.0f );
	// Loop through regions and draw the cities
	/*for ( uint i = 0; i < usedRegions.size(); ++i )
	{
		if ( usedRegions[i].second.count > 10 )
		{*/
			// Get sectors for the region
			/*vector<World::sProvinceAreaEntry> regionSectors;
			World::ActiveRegionManager->GetProvinceArea( usedRegions[i].first, regionSectors );

			for ( uint r = 0; r < regionSectors.size(); ++r )
			{
				CTownManager::sTownInfo townInfo;
				if ( World::ActiveTownManager->GetTownAtSector( RangeVector(regionSectors[r].x,regionSectors[r].y,0), &townInfo ) )
				{
					//glVertex2f( (regionSectors[r].x - r_halfsize.x), (regionSectors[r].y - r_halfsize.y) );
					Vector2d midpoint ( (regionSectors[r].x - r_pos.x + r_halfsize.x), (regionSectors[r].y - r_pos.x + r_halfsize.y) ); 
					midpoint.x *= m_rt->GetWidth();
					midpoint.y *= m_rt->GetHeight();

					glVertex2f( midpoint.x - 4, midpoint.y );
					glVertex2f( midpoint.x + 4, midpoint.y );
					glVertex2f( midpoint.x, midpoint.y - 4 );
					glVertex2f( midpoint.x, midpoint.y + 4 );
				}
			}*/
			//vector<World::sProvinceAreaEntry> regionSectors;
			//World::ActiveRegionManager->GetProvinceTownAreas( usedRegions[i].first, regionSectors );
	/*for ( int x = 0; x < 64; ++x )
	{
		for ( int y = 0; y < 64; ++y )
		{
			// Convert iteration into point
			samplePoint = Vector2d( r_pos.x + ((x-32)/32.0f)*r_halfsize.x, r_pos.y + ((y-32)/32.0f)*r_halfsize.y );
			CTownManager::sTownInfo townInfo;
			if ( World::ActiveTownManager->GetTownAtSector( RangeVector(rangeint(samplePoint.x/64.0f), rangeint(samplePoint.y/64.0f), 0), &townInfo ) )
			{
				//Vector2d midpoint ( (regionSectors[r].x - r_pos.x + r_halfsize.x), (regionSectors[r].y - r_pos.x + r_halfsize.y) ); 
				Vector2d midpoint ( x/64.0f, 1-(y/64.0f) ); 
				midpoint.x *= m_rt->GetWidth();
				midpoint.y *= m_rt->GetHeight();

				GLd.P_AddVertex( midpoint.x - 4, midpoint.y );
				GLd.P_AddVertex( midpoint.x + 4, midpoint.y );
				GLd.P_AddVertex( midpoint.x, midpoint.y - 4 );
				GLd.P_AddVertex( midpoint.x, midpoint.y + 4 );
			}
		}
	}*/
	/*	}
	}*/
	GLd.EndPrimitive();

	fontMat.bindPass(0);
		GLd.P_PushColor( 1.0f, 1.0f, 1.0f, 1.0f );
		for ( uint i = 0; i < usedRegions.size(); ++i )
		{
			arstring<256> regionName;
			World::ProvinceManager->GetProvinceName(usedRegions[i].first, regionName );
			GLd.DrawTextP(
				(usedRegions[i].second.center.x/64.0f - 0.004f*regionName.length()) * m_rt->GetWidth(),
				(1-usedRegions[i].second.center.y/64.0f) * m_rt->GetHeight(),
				"%s", regionName.c_str()
				);
		}

	// End render to texture
	{
		GL.endOrtho();
		m_rt->UnbindBuffer();
		GL.setupViewport( 0,0,Screen::Info.width,Screen::Info.height );
	}
}


void CWorldCartographer::RenderVerts ( void )
{
	GL_ACCESS GLd_ACCESS

	// Create draw material
	glMaterial tempMat;
	{
		tempMat.setTexture( 0, new CTexture("null") );
		tempMat.passinfo.push_back( glPass() );
		tempMat.passinfo[0].m_blend_mode = Renderer::BM_NORMAL;
		tempMat.passinfo[0].m_lighting_mode = Renderer::LI_NONE;
		tempMat.passinfo[0].m_transparency_mode = Renderer::ALPHAMODE_TRANSLUCENT;
		tempMat.passinfo[0].m_face_mode = Renderer::FM_FRONTANDBACK;
		tempMat.passinfo[0].shader = new glShader( ".res/shaders/v2d/default.glsl" );
	}
	tempMat.removeReference();

	// Render to texture
	{
		m_rt->BindBuffer();
		GL.setupViewport( 0,0, m_rt->GetWidth(), m_rt->GetHeight() );
		GL.beginOrtho( 0,0, (ftype)m_rt->GetWidth(), (ftype)m_rt->GetHeight(), -45,45, false );
	}

	// Clear render target.
	GL.clearColor( Color( 0.3f, 0.3f, 0.5f, 0.0f ) );
	GL.ClearBuffer( GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT );

	// Draw the prim
	GL.pushModelMatrix( Matrix4x4() );
	tempMat.bindPass(0);
	GLd.BeginPrimitive( GL_QUADS );
	GLd.P_PushNormal( 0,0,1 );
	GLd.P_PushTexcoord( 0,0 );
	for ( uint x = 0; x < m_max_x-1; ++x )
	{
		for ( uint y = 0; y < m_max_y-1; ++y )
		{
			CTerrainVertex vert;
			vert = verts[x+(y*m_max_x)];
			GLd.P_PushColor( vert.r, vert.g, vert.b, vert.a );
			GLd.P_AddVertex( vert.x, vert.y, vert.z );

			vert = verts[x+1+(y*m_max_x)];
			GLd.P_PushColor( vert.r, vert.g, vert.b, vert.a );
			GLd.P_AddVertex( vert.x, vert.y, vert.z );

			vert = verts[x+1+((y+1)*m_max_x)];
			GLd.P_PushColor( vert.r, vert.g, vert.b, vert.a );
			GLd.P_AddVertex( vert.x, vert.y, vert.z );

			vert = verts[x+((y+1)*m_max_x)];
			GLd.P_PushColor( vert.r, vert.g, vert.b, vert.a );
			GLd.P_AddVertex( vert.x, vert.y, vert.z );
		}
	}
	GLd.EndPrimitive();
	GL.popModelMatrix();

	// End render to texture
	{
		GL.endOrtho();
		m_rt->UnbindBuffer();
		GL.setupViewport( 0,0,Screen::Info.width,Screen::Info.height );
	}
}
