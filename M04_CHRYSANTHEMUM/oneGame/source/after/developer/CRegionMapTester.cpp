
#include "CRegionMapTester.h"

//#include "unused/CRegionManager.h"

#include "core/input/CInput.h"

#include "renderer/material/glMaterial.h"
#include "renderer/system/glMainSystem.h"
#include "renderer/system/glDrawing.h"

#include "engine-common/network/playerlist.h"
//#include "engine-common/entities/CPlayer.h"
#include "engine-common/entities/CActor.h"

#include "after/interfaces/world/CWorldCartographer.h"
#include "after/states/world/ProvinceManager.h"

CRegionMapTester::CRegionMapTester ( void )
	: CGameBehavior(), CRenderableObject()
{
	visible = false;

	// Create render options
	myMaterial = new glMaterial;
	//myMaterial->useColors = true;
	//myMaterial->useLighting = false;
	myMaterial->m_diffuse = Color( 1.0f,1,1 );
	SetMaterial( myMaterial );

	myMaterial2 = new glMaterial;
	//myMaterial2->useColors = true;
	//myMaterial2->useLighting = false;
	myMaterial2->m_diffuse = Color( 1.0f,1,1 );
	//myMaterial2->useTexture = false;

	// Set to top layer drawing
	renderType = Renderer::V2D;

	bNeedUpdate = true;

	mRT = new CRenderTexture ( RGBA8, 512,512 );

	myMaterial->setTexture( 0, mRT );
}


CRegionMapTester::~CRegionMapTester ( void )
{
	delete mRT;
	mRT = NULL;
}

void CRegionMapTester::Update ( void )
{
	if ( Input::Keydown( Keys.F8 ) ) {
		visible = !visible;
	}

	if ( visible ) {
		// Update the map
		if ( bNeedUpdate ) 
		{
			bNeedUpdate = false;

			// Update the map.
			for ( int32_t x = -32; x < 32; ++x ) {
				for ( int32_t y = -32; y < 32; ++y ) {
					Map(x,y) = World::ProvinceManager->GetRegion( x,y );
				}
			}
			// Create map image
			CWorldCartographer cartographer( "terra", mRT );
			//cartographer.RenderRegionMap();
			cartographer.RenderAreaMap();
		}
	}
}

bool CRegionMapTester::Render ( const char pass )
{
	if ( pass != 0 ) {
		return false;
	}
	GL_ACCESS GLd_ACCESS

	GL.beginOrtho();
	GLd.DrawSet2DMode( GLd.D2D_FLAT );

	/*for ( int32_t x = -32; x < 32; ++x ) {
	for ( int32_t y = -32; y < 32; ++y ) {
		uint32_t region = Map(x,y);
		if (( region != uint32_t(-2) )&&( region != uint32_t(-1) )) {
			glColor4f( (region%3)*0.5f, ((region/3)%3)*0.5f, ((region/9)%3)*0.5f, 0.5f );
		}
		else {
			glColor4f( 0.2f,0.6f,1.0f,0.0f );
		}

		GLd.DrawRectangle( 100+(x+32)*8,100+(y+32)*8,8,8 );
	}
	}*/

	myMaterial->bindPass(0);
	GLd.DrawRectangle( 100,100,mRT->GetWidth(),mRT->GetHeight() );
	//myMaterial->unbind();

	myMaterial2->bindPass(0);
	
	auto playerList = Network::GetPlayerActors();
	//Vector2d player ( CPlayer::GetActivePlayer()->transform.position.x, CPlayer::GetActivePlayer()->transform.position.y );
	Vector2d player ( playerList[0].actor->transform.position.x, playerList[0].actor->transform.position.y );
	
	//player /= 64.0f;
			//Vector2d samplePoint ( r_pos.x + ((x/32.0f) - 1)*r_halfsize.x, r_pos.y + ((y/32.0f) - 1)*r_halfsize.y );
	Vector2d r_halfsize	= Vector2d(2048,2048);

	player += r_halfsize;
	player.x /= r_halfsize.x*2;
	player.y /= r_halfsize.y*2;
	player.y = 1-player.y;

	player.x *= mRT->GetWidth();
	player.y *= mRT->GetHeight();

	GLd.DrawCircle( (int)(100+player.x),(int)(100+player.y), 3 );

	GL.endOrtho();

	return true;
}