
// ==Includes==
// Main class
#include "CPlayerHud.h"

// Other classes needed
#include "core/input/CInput.h"
#include "core/time/time.h"

#include "engine/state/CGameState.h"

#include "renderer/light/CLight.h"

#include "after/entities/character/CAfterPlayer.h"

#include "after/terrain/Zones.h"
#include "after/terrain/edit/CTerrainAccessor.h"
#include "after/terrain/VoxelTerrain.h"
#include "after/physics/wind/WindMotion.h"
#include "after/types/terrain/TerrainTypes.h"
#include "after/types/terrain/BiomeTypes.h"
#include "after/types/terrain/WeatherTypes.h"

//#include "unused/CVoxelTerrain.h"
//#include "unused/COctreeTerrain.h"

#include "after/renderer/objects/hud/CBlockCursor.h"

//#include "CRTCamera.h"
#include "engine-common/entities/CWaypoint.h"

// Include screen properties
#include "core/system/Screen.h"

//#include "CTimeProfilerUI.h"

// UI's needed
#include "CPlayerInventoryGUI.h"
#include "CPlayerLogbook.h"
//#include "CPlayerQuestlog.h"
//#include "CPlayerSkilltree.h"

#include "renderer/material/glMaterial.h"
#include "renderer/system/glMainSystem.h"
#include "renderer/system/glDrawing.h"

#include "after/interfaces/CCharacterMotion.h"

#include "engine/physics/raycast/Raycaster.h"
#include "engine/physics/motion/CRigidbody.h"

#include "after/terrain/system/MemoryManager.h"

// ==Constructor==
CPlayerHud::CPlayerHud ( CAfterPlayer* p_player, CInventory* p_inventory )
	: CGameBehavior(), CRenderableObject()
{
	pPlayer		= p_player;
	pInventory	= p_inventory;

	renderSettings.renderHints = RL_WORLD;
	renderType = Renderer::V2D;

	fntDebug	= new CBitmapFont ( "monofonto.ttf", 18, FW_BOLD );
	fntGamehud	= new CBitmapFont ( "HVD_Comic_Serif_Pro.otf", 16, FW_NORMAL );
	fntGamehudSm= new CBitmapFont ( "HVD_Comic_Serif_Pro.otf", 12, FW_BOLD );

	matFntDebug = new glMaterial;
	matFntDebug->m_diffuse = Color( 0.0f,0,0 );
	matFntDebug->setTexture( 0, fntDebug );
	matFntDebug->passinfo.push_back( glPass() );
	matFntDebug->passinfo[0].m_lighting_mode	= Renderer::LI_NONE;
	matFntDebug->passinfo[0].m_transparency_mode = Renderer::ALPHAMODE_TRANSLUCENT;
	matFntDebug->passinfo[0].shader = new glShader( ".res/shaders/v2d/default.glsl" );

	matDrawDebug = new glMaterial;
	matDrawDebug->m_diffuse = Color( 0.0f,0,0 );
	matDrawDebug->setTexture( 0, new CTexture( "null" ) );
	matDrawDebug->passinfo.push_back( glPass() );
	matDrawDebug->passinfo[0].m_lighting_mode	= Renderer::LI_NONE;
	matDrawDebug->passinfo[0].m_transparency_mode = Renderer::ALPHAMODE_TRANSLUCENT;
	matDrawDebug->passinfo[0].shader = new glShader( ".res/shaders/v2d/default.glsl" );

	matDrawHudFnt = new glMaterial;
	matDrawHudFnt->m_diffuse = Color( 0.0f,0,0 );
	matDrawHudFnt->setTexture( 0, fntGamehud );
	matDrawHudFnt->passinfo.push_back( glPass() );
	matDrawHudFnt->passinfo[0].m_lighting_mode	= Renderer::LI_NONE;
	matDrawHudFnt->passinfo[0].m_transparency_mode = Renderer::ALPHAMODE_TRANSLUCENT;
	matDrawHudFnt->passinfo[0].shader = new glShader( ".res/shaders/v2d/default.glsl" );

	matDrawHud	= new glMaterial;
	matDrawHud->m_diffuse = Color( 1.0f,1,1 );
	matDrawHud->setTexture( 0, new CTexture( "null" ) );
	matDrawHud->passinfo.push_back( glPass() );
	matDrawHud->passinfo[0].m_lighting_mode	= Renderer::LI_NONE;
	matDrawHud->passinfo[0].shader = new glShader( ".res/shaders/v2d/default.glsl" );

	//texReticle = new CTexture( ".res\\textures\\hud\\crosshair.tga" );
	//texReticle = new CTexture( ".res/textures/black.jpg" );

	matDrawReticle = new glMaterial;
	matDrawReticle->setTexture( 0, new CTexture( ".res/textures/hud/crosshairs.png" ) );
	matDrawReticle->passinfo.push_back( glPass() );
	matDrawReticle->passinfo[0].m_lighting_mode = Renderer::LI_NONE;
	matDrawReticle->passinfo[0].m_transparency_mode = Renderer::ALPHAMODE_TRANSLUCENT;
	matDrawReticle->passinfo[0].shader = new glShader( ".res/shaders/v2d/default.glsl" );

	SetMaterial( matDrawHud );

	Initialize();

	// Testing RT's
	//pRTTex = new CRenderTexture ( RGBA8, 1024,1024, Clamp,Clamp, Texture2D, Depth16,true,true );
	//pRTTex = new CRenderTexture ( );
	//pRTCam = new CRTCamera( pRTTex, 30.0f, true );
	//pRTCam->orthographic = true;
	//pRTCam->ortho_size = Vector2d( 64,64 );
	/*matRT = new glMaterial;
	matRT->useLighting = false;
	matRT->useColors = false;
	matRT->diffuse = Color( 1.0f,1,1,1.00f );*/
	//matRT->setTexture( pRTTex );


	bDrawRenderTargets = false;
}

// ==Destructor==
CPlayerHud::~CPlayerHud ( void )
{
	// Free up anything we're using
	delete fntDebug;
	delete fntGamehud;
	delete fntGamehudSm;

	matFntDebug->removeReference();
	delete matFntDebug;
	matDrawDebug->removeReference();
	delete matDrawDebug;

	matDrawReticle->removeReference();
	delete matDrawReticle;

	matDrawHudFnt->removeReference();
	delete matDrawHudFnt;

	// Delete memory monitor
	delete pMemMonitor;
	// Delete time profiler
	//delete timeProfiler;

	// Delete cursor
	delete blockCursor;
}

// == State Updates ==
// Update for hud values
void CPlayerHud::Update ( void )
{
	// Testing RT's
	//pRTCam->transform.Get( CCamera::activeCamera->transform );

	// Update your gay ass shit here
	pMemMonitor->Update();

	// Block Cursor
	UpdateBlockCursor();

	// Toggle position drawn if F3 is pressed
	if ( Input::Keydown( Keys.F1 ) )//if ( CInput::keydown[VK_F1] )
		bDrawPlayerPosition = !bDrawPlayerPosition;
	if ( Input::Keydown( Keys.F2 ) )//if ( CInput::keydown[VK_F2] )
		bDrawMemoryMonitor = !bDrawMemoryMonitor;
	if ( Input::Keydown( Keys.F3 ) )//if ( CInput::keydown[VK_F3] )
		bDrawPlayerVelocities = !bDrawPlayerVelocities;
	//if ( Input::Keydown( Keys.F6 ) )//if ( CInput::keydown[VK_F6] )
	//	timeProfiler->visible = !timeProfiler->visible;
	//if ( Input::Keydown( Keys.F7 ) )
	//	bDrawRenderTargets = !bDrawRenderTargets;
}

// Render for drawing
bool CPlayerHud::Render ( const char pass )
{
	if ( pass != 0 )
		return false;
	GL_ACCESS;
	GLd_ACCESS;
	GL.beginOrtho();
	GLd.DrawSet2DScaleMode();

	// Put your drawing shit here
	DrawPlayerPosition();

	//DrawPlayerHealth();


	DrawMemoryStats();

	// Testing RT's
	//if ( pRTCam != CCamera::activeCamera )
	/*if ( CCamera::activeCamera->GetTypeName() == "CameraBase" )
	{
		matRT->bind();
		//pRTTex->BindDepth();
		vector<CLight*>* vl = CLight::GetLightList();
		if ( (*vl)[0]->GetShadowTexture() )
			(*vl)[0]->GetShadowTexture()->Bind();
			GL.DrawSet2DMode( GL.D2D_FLAT );
			GL.DrawRectangleA( 0.3f,0.01f,0.2f,0.2f );
		matRT->unbind();
	}*/
	if ( bDrawRenderTargets )
	{
		int camIndex = 0;
		for ( std::vector<CCamera*>::iterator cam = CCamera::vCameraList.begin(); cam != CCamera::vCameraList.end(); ++cam )
		{
			if ( (*cam)->IsRTCamera() )
			{
				CRTCamera* camDraw = (CRTCamera*)(*cam);
				CRenderTexture* texRT = camDraw->GetRenderTexture();
				
				matFntDebug->setTexture( 0, texRT );
				matFntDebug->bindPass(0);
					GLd.P_PushColor( 1.0f,1.0f,1.0f );
					GLd.DrawSet2DScaleMode( GLd.SCALE_HEIGHT );
					GLd.DrawSet2DMode( GLd.D2D_FLAT );
					GLd.DrawRectangleA( 0.01f,0.01f + 0.2f*camIndex,0.2f,0.2f );

				/*texRT->BindDepth();
					glColor3f( 1.0f,1.0f,1.0f );
					GL.DrawSet2DScaleMode( GL.SCALE_HEIGHT );
					GL.DrawSet2DMode( GL.D2D_FLAT );
					GL.DrawRectangleA( 0.01f + 0.2f,0.01f + 0.2f*camIndex,0.2f,0.2f );
					
				matFntDebug->unbind();*/ // TODO

				++camIndex;
			}
		}
	}
	

	DrawAimingReticle();
	DrawWaypoints();
	DrawPlayerVelocity();

	if ( pPlayer->GetCurrentLookedAt() )
	{
		if ( (pPlayer->GetCurrentLookedAt()->layer & Layers::WeaponItem) != 0 )
		{
			CWeaponItem* item = (CWeaponItem*)pPlayer->GetCurrentLookedAt();
			Vector3d itemPos = CCamera::activeCamera->WorldToScreenPos( item->transform.position );

			if ( item->GetTypeName() == "RandomBaseMelee" )
			{
				CWeaponItem::WTooltipStyle style;
				style.fontTexture	= fntGamehudSm;
				style.matFont		= matDrawHudFnt;
				style.matBg			= matDrawHud;
				style.matBg->m_diffuse = Color (0.1f, 0.1f, 0.1f,0.8f);

				Vector2d position;
				position.x = itemPos.x*(Real)Screen::Info.aspect - 0.2f;
				position.y = itemPos.y - 0.2f;
				GLd.DrawSet2DScaleMode(GLd.SCALE_HEIGHT);
				item->DrawTooltip( position, style );
				GLd.DrawSet2DScaleMode();
			}

			matDrawHudFnt->setTexture( 0, fntGamehud );
			matDrawHudFnt->m_diffuse = Color( 0.0f,0.0f,0.0f );
			matDrawHudFnt->bindPass(0);
				for ( uint i = 0; i < 3; ++i ) {
					GLd.DrawAutoTextCentered(
						itemPos.x+0.003f*sinf(Time::currentTime+(Real)PI*i*0.667f),itemPos.y+0.003f*cosf(Time::currentTime+(Real)PI*i*0.667f),
						"%s", item->GetItemName() );
				}
			matDrawHudFnt->m_diffuse = Color( 1.0f,1.0f,1.0f );
			matDrawHudFnt->bindPass(0);
				GLd.DrawAutoTextCentered( itemPos.x,itemPos.y, "%s", item->GetItemName() );
		}
		else if ( (pPlayer->GetCurrentLookedAt()->layer & Layers::Character) != 0 )
		{
			CCharacter* character = (CCharacter*)pPlayer->GetCurrentLookedAt();
			Vector3d charPos = CCamera::activeCamera->WorldToScreenPos( character->GetEyeRay().pos + Vector3d( 0,0,0.5f ) );

			matDrawHudFnt->setTexture( 0, fntGamehud );
			matDrawHudFnt->m_diffuse = Color( 0.0f,0.0f,0.0f );
			matDrawHudFnt->bindPass(0);
				for ( uint i = 0; i < 3; ++i ) {
					GLd.DrawAutoTextCentered(
						charPos.x+0.003f*sinf(Time::currentTime+(Real)PI*i*0.667f),charPos.y+0.003f*cosf(Time::currentTime+(Real)PI*i*0.667f),
						"%s", character->GetName() );
				}
			matDrawHudFnt->m_diffuse = Color( 1.0f,1.0f,1.0f );
			matDrawHudFnt->bindPass(0);
				GLd.DrawAutoTextCentered( charPos.x,charPos.y, "%s", character->GetName() );

			charPos = CCamera::activeCamera->WorldToScreenPos( character->GetEyeRay().pos - Vector3d( 0,0,0.7f ) );
			matDrawHudFnt->m_diffuse = Color( 0.0f,0.0f,0.0f );
			matDrawHudFnt->bindPass(0);
				for ( uint i = 0; i < 3; ++i ) {
					GLd.DrawAutoTextCentered(
						charPos.x+0.003f*sinf(Time::currentTime+(Real)PI*i*0.667f),charPos.y+0.003f*cosf(Time::currentTime+(Real)PI*i*0.667f),
						"%s", "Press [E] to Greet" );
				}
			matDrawHudFnt->m_diffuse = Color( 1.0f,1.0f,1.0f );
			matDrawHudFnt->bindPass(0);
				GLd.DrawAutoTextCentered( charPos.x,charPos.y, "%s", "Press [E] to Greet" );
		}	
	}

	GL.endOrtho();
	// Return success
	return true;
}

// == Private Routines ==
// Initialize all your retarded shit here
void CPlayerHud::Initialize ( void )
{
	bDrawPlayerPosition = false;

	blockCursor = new CBlockCursor ();
	bDrawBlockCursor = true;

	// Create memory monitor
	bDrawMemoryMonitor = false;
	pMemMonitor = new CMemoryMonitor();

	bDrawPlayerVelocities = false;

	// Create time profiler output
	//timeProfiler = new CTimeProfilerUI();
}

// === Non-rendering Routines ===
// Update block cursor
void CPlayerHud::UpdateBlockCursor ( void )
{
	if ( bDrawBlockCursor )
	{
		// raytrace from point
		Ray		aimDir;
		aimDir = pPlayer->GetEyeRay();

		RaycastHit		result;
		BlockTrackInfo	block;
		//pPlayer->fMaxUseDistance
		if ( Raycaster.Raycast( aimDir, 7.0f, &result, 1|2|4 ) )
		{
			TerrainAccess.GetBlockAtPosition( result, block );
			if ( result.pHitBehavior == CVoxelTerrain::GetActive() )
			{
				Vector3d newPos = result.hitPos-result.hitNormal*0.3f;
				if ( newPos.x < 0 )
					newPos.x -= 2;
				if ( newPos.y < 0 )
					newPos.y -= 2;
				if ( newPos.z < 0 )
					newPos.z -= 2;
				newPos.x -= fmod( newPos.x, 2.0f );
				newPos.y -= fmod( newPos.y, 2.0f );
				newPos.z -= fmod( newPos.z, 2.0f );
				blockCursor->transform.position = newPos + Vector3d( 1,1,1 );

				blockCursor->SetVisible( true );
			}
		}
		else
		{
			blockCursor->SetVisible( false );
		}
	}
	else
	{
		blockCursor->SetVisible( false );
	}
}

// === Rendering Routines ===
// If wanted, draws the player position on screen.
void CPlayerHud::DrawPlayerPosition ( void )
{
	if ( bDrawPlayerPosition )
	{
		// Get the terrain string at current position
		char str_ter [256] = "er...";
		char terrainType = Zones.GetTerrainTypeAt( pPlayer->transform.position );
		switch ( terrainType )
		{
			case Terrain::TER_DEFAULT:	strcpy( str_ter, "Default" );		break;
			case Terrain::TER_DESERT:	strcpy( str_ter, "Desert" );		break;
			case Terrain::TER_BADLANDS:	strcpy( str_ter, "Badlands" );		break;
			case Terrain::TER_THE_EDGE:	strcpy( str_ter, "The Edge" );		break;
			case Terrain::TER_OUTLANDS:	strcpy( str_ter, "Outlands" );		break;
			case Terrain::TER_SPIRES:	strcpy( str_ter, "Spires" );		break;
			case Terrain::TER_OCEAN:		strcpy( str_ter, "Ocean" );			break;
			case Terrain::TER_ISLANDS:	strcpy( str_ter, "Islands" );		break;
			case Terrain::TER_FLATLANDS:	strcpy( str_ter, "Flatlands" );		break;
			case Terrain::TER_HILLLANDS:	strcpy( str_ter, "Hilllands" );		break;
			case Terrain::TER_MOUNTAINS:	strcpy( str_ter, "Mountains" );		break;
		}
		// Get the biome string at the current position
		char str_bio [256] = "er...";
		char biomeType = Zones.GetTerrainBiomeAt( pPlayer->transform.position );
		switch ( biomeType )
		{
			case Terrain::BIO_DEFAULT:		strcpy( str_bio, "Default" );		break;
			case Terrain::BIO_EVERGREEN:		strcpy( str_bio, "a-Evergreen" );	break;
			case Terrain::BIO_SAVANNA:		strcpy( str_bio, "a-Savanna" );		break;
			case Terrain::BIO_THE_EDGE:		strcpy( str_bio, "The Edge" );		break;
			case Terrain::BIO_OUTLANDS:		strcpy( str_bio, "Outlands" );		break;
			case Terrain::BIO_WET_OUTLANDS:	strcpy( str_bio, "Wet Outlands" );	break;
			case Terrain::BIO_RAINFOREST:	strcpy( str_bio, "d-Rainforest" );	break;
			case Terrain::BIO_TROPICAL:		strcpy( str_bio, "d-Tropical" );	break;
			case Terrain::BIO_SWAMP:			strcpy( str_bio, "e-Swamp" );		break;
			case Terrain::BIO_MOIST:			strcpy( str_bio, "e-Moist" );		break;
			case Terrain::BIO_TEMPERATE:		strcpy( str_bio, "b-Temperate" );	break;
			case Terrain::BIO_GRASSLAND:		strcpy( str_bio, "b-Grassland" );	break;
			case Terrain::BIO_TAIGA:			strcpy( str_bio, "c-Taiga" );		break;
			case Terrain::BIO_TUNDRA:		strcpy( str_bio, "c-Tundra" );		break;
			case Terrain::BIO_S_TUNDRA:		strcpy( str_bio, "Super Tundra" );	break;
			case Terrain::BIO_COLD_DESERT:	strcpy( str_bio, "Cold Desert" );	break;
			case Terrain::BIO_DESERT:		strcpy( str_bio, "Desert" );		break;
			case Terrain::BIO_GRASS_DESERT:	strcpy( str_bio, "Grassy Desert" );	break;
		}
		// Get the weather string at the current position
		char str_wth [256] = "er...";
		uchar weatherType = WindMotion.GetWeather( pPlayer->transform.position );
		switch ( weatherType )
		{
			case Terrain::WTH_NONE:				strcpy( str_wth, "None" );			break;
			case Terrain::WTH_LIGHT_RAIN:		strcpy( str_wth, "Rain - Light" );	break;
			case Terrain::WTH_HEAVY_RAIN:		strcpy( str_wth, "Rain - Heavy" );	break;
			case Terrain::WTH_LIGHT_RAIN_THUNDER:strcpy( str_wth, "Thundershower - Light" );		break;
			case Terrain::WTH_HEAVY_RAIN_THUNDER:strcpy( str_wth, "Thundershower - Heavy" );		break;
			case Terrain::WTH_LIGHT_SNOW:		strcpy( str_wth, "Snow - Light" );	break;
			case Terrain::WTH_HEAVY_SNOW:		strcpy( str_wth, "Snow - Heavy" );	break;
			case Terrain::WTH_STORM_SNOW:		strcpy( str_wth, "Snow - Storm" );	break;
			case Terrain::WTH_FOG_LIGHT:			strcpy( str_wth, "Fog - Light" );	break;
		}
		// Get the temperature string at the current position
		char str_tmp [256] = "er...";
		ftype temperatureValue = WindMotion.GetTemperatureFast( pPlayer->transform.position ) * 70.0f;
		sprintf( str_tmp, "%.1lf F", temperatureValue );

		// Get the terrain's memory
		static Real f_terMemory = 0;
		static Real f_terMemory_refreshTimer = 0;
		f_terMemory_refreshTimer += Time::deltaTime;
		if ( f_terMemory_refreshTimer > 2 ) {
			f_terMemory_refreshTimer -= 2;
			if ( CVoxelTerrain::GetActive() ) {
				f_terMemory = CVoxelTerrain::GetActive()->Memory->GetMemoryUsage()*100;
			}
		}

		// Begin Drawing
		GL_ACCESS GLd_ACCESS;
		GLd.DrawSet2DMode( GLd.D2D_WIRE );

		matFntDebug->setTexture( 0, fntDebug );
		matFntDebug->m_diffuse = Color( 0,0,0,1 );
		matFntDebug->bindPass(0);
			/*GL.DrawAutoText( 0.1f,0.1f, "Player Position:\n  x: %lf\n  y: %lf\n  z: %lf",
				pPlayer->transform.position.x,pPlayer->transform.position.y,pPlayer->transform.position.z );*/
			GLd.DrawAutoText( 0.01f,0.04f, "(F3) Player Pos:" );
			GLd.DrawAutoText( 0.01f,0.08f, "  x: %.3lf", pPlayer->transform.position.x );
			GLd.DrawAutoText( 0.01f,0.11f, "  y: %.3lf", pPlayer->transform.position.y );
			GLd.DrawAutoText( 0.01f,0.14f, "  z: %.3lf", pPlayer->transform.position.z );
			GLd.DrawAutoText( 0.01f,0.19f, "  ter: %s", str_ter );
			GLd.DrawAutoText( 0.01f,0.22f, "  bio: %s", str_bio );
			GLd.DrawAutoText( 0.01f,0.25f, "  wth: %s", str_wth );
			GLd.DrawAutoText( 0.01f,0.28f, "  tmp: %s", str_tmp );

			GLd.DrawAutoText( 0.01f,0.34f, "  mem: %.3lf", f_terMemory );

			const char cs_movetypes [][24] = {
				"MOVEMENT_DEFAULT",
				"MOVEMENT_FALLING",
				"MOVEMENT_WATerrain::TER",
				"MOVEMENT_SLIDING",
				
				"MOVEMENT_STUN_LIGHT",
				"MOVEMENT_STUN_HEAVY",
				
				"MOVEMENT_MINIDASH",

				"MOVEMENT_UNKNOWN"
			};
			GLd.DrawAutoText( 0.01f,0.40f, "  Movement: %s", cs_movetypes[pPlayer->GetMotionState()->GetMovementMode()] );
			/*if ( CVoxelTerrain::GetActive()->iploadstate == 0 )
				GL.DrawAutoText( 0.02f,0.8f, "waiting" );
			else if ( CVoxelTerrain::GetActive()->iploadstate == 1 )
				GL.DrawAutoText( 0.02f,0.8f, "loading" );
			else if ( CVoxelTerrain::GetActive()->iploadstate == 2 )
				GL.DrawAutoText( 0.02f,0.8f, "sim lock wait" );
			else if ( CVoxelTerrain::GetActive()->iploadstate == 3 )
				GL.DrawAutoText( 0.02f,0.8f, "simulation" );*/
			if ( CVoxelTerrain::GetActive() )
			{
				//if ( CVoxelTerrain::GetActive()->iploadstate == 0 )
				//	GL.DrawAutoText( 0.02f,0.8f, "waiting" );
				//else if ( CVoxelTerrain::GetActive()->iploadstate == 1 )
				//	GL.DrawAutoText( 0.02f,0.8f, "generation" );
				//GL.DrawAutoText( 0.02f,0.85f, "loadlistsize %d", CVoxelTerrain::GetActive()->GetLoadingListSize() );
			}
			/*if ( COctreeTerrain::GetActive() ) 
			{
				Real_d currentTime = CTime::GetCurrentCPUTime();
				int count = 0;
				const std::vector<Real_d>& threadTimes = COctreeTerrain::GetActive()->GetThreadTimes();
				for ( auto time = threadTimes.begin(); time != threadTimes.end(); ++time )
				{
					Real_d timeDif = currentTime - *time;
					matFntDebug->m_diffuse = Color( std::max<Real_d>( std::min<Real_d>((timeDif-300)/1000,1), 0 ),std::max<Real_d>( (300-timeDif)/600, 0 ),0,1 );
					matFntDebug->bindPass(0);
					GLd.DrawAutoText( 0.01f, 0.50f + count*0.05f, "Thread %d: %lfms", count, timeDif );
					count++;
				}
			}*/

		matDrawDebug->bindPass(0);
			GLd.DrawRectangleA( 0.01f,0.01f,0.16f,0.15f );
	}
}

// Draws the player health
/*void CPlayerHud::DrawPlayerHealth ( void )
{
	matFntDebug->bind();
	fntDebug->Set();
		GL.DrawAutoText( 0.02f,0.92f, "%3.0lf/%3.0lf", ftype(pPlayer->GetHealth()),pPlayer->GetHealth().GetMax() );
	fntDebug->Unbind();
	matFntDebug->unbind();
}*/

// Draws the aiming reticle
void CPlayerHud::DrawAimingReticle ( void )
{
	/*matDrawHud->setTexture( texReticle );
	matDrawHud->bind();
		GL.DrawSet2DMode( GL.D2D_FLAT );
		GL.DrawRectangle( Screen::Info.width/2-2,Screen::Info.height/2-2, 4,4 );
	matDrawHud->unbind();*/
	bool reticleDrawn = false;
	
	GLd_ACCESS;
	
	if ( pPlayer->GetInventory() )
	{
		//matDrawHud->setTexture( 0, texReticle );
		//pPlayer->GetInventory()->GetCurrentEquipped(pPlayer->GetInventory()->GetCurrent())->OnDrawHUD( matDrawHud );
		CPlayerInventory* pl_inventory = (CPlayerInventory*)pPlayer->GetInventory();
		for ( short i = 0; i < pl_inventory->GetHandCount(); ++i )
		{
			CWeaponItem* equippedItem = pl_inventory->GetCurrentEquipped(i);
			if ( equippedItem && (equippedItem->GetTypeName() != "ItemBlockPuncher") )
			{
				CWeaponItem::WReticleStyle reticleStyle;
				reticleStyle.matGeneralHud	= matDrawHud;
				reticleStyle.matReticle		= matDrawReticle;
				reticleStyle.hand_id		= (uchar) i;
				pl_inventory->GetCurrentEquipped(i)->OnDrawHUD( reticleStyle );

				reticleDrawn = true;
			}
		}
	}

	if ( !reticleDrawn )
	{
		// Draw the running reticle
		CGameBehavior* lookedAt = pPlayer->GetCurrentLookedAt();
		bool drawHandIcon = false;
		if ( lookedAt && CGameState::Active()->ObjectExists( lookedAt ) ) {
			if ( lookedAt->layer & Layers::WeaponItem ) {
				drawHandIcon = true;
			}
			else if ( lookedAt->layer & Layers::Actor ) {
				if ( ((CActor*)lookedAt)->Interactable() ) {
					drawHandIcon = true;
				}
			}
		}

		if ( drawHandIcon ) {
			matDrawReticle->m_diffuse = Color( 1,1,1,1.0f );
		}
		else {
			matDrawReticle->m_diffuse = Color( 1,1,1,0.5f );
		}
		matDrawReticle->bindPass(0);
		Vector2d centerPosition ( Screen::Info.width/2.0f,Screen::Info.height/2.0f );
		Vector2d reticleHalfsize ( 16,16 );

		if ( !drawHandIcon )
		{
			reticleHalfsize = Vector2d ( 24,24 );
			GLd.BeginPrimitive( GL_TRIANGLE_STRIP );
				GLd.P_PushColor( Color(1,1,1,1) );
				GLd.P_PushTexcoord( Vector2d(0,0)/8 );
				GLd.P_AddVertex( centerPosition + Vector2d(-reticleHalfsize.x,-reticleHalfsize.y) );
				GLd.P_PushTexcoord( Vector2d(0,1)/8 );
				GLd.P_AddVertex( centerPosition + Vector2d(-reticleHalfsize.x,reticleHalfsize.y) );
				GLd.P_PushTexcoord( Vector2d(1,0)/8 );
				GLd.P_AddVertex( centerPosition + Vector2d(reticleHalfsize.x,-reticleHalfsize.y) );
				GLd.P_PushTexcoord( Vector2d(1,1)/8 );
				GLd.P_AddVertex( centerPosition + Vector2d(reticleHalfsize.x,reticleHalfsize.y) );
			GLd.EndPrimitive();
		}
		else
		{
			GLd.BeginPrimitive( GL_TRIANGLE_STRIP );
				GLd.P_PushColor( Color(1,1,1,1) );
				GLd.P_PushTexcoord( Vector2d(2,0)/8 );
				GLd.P_AddVertex( centerPosition + Vector2d(-reticleHalfsize.x,-reticleHalfsize.y) );
				GLd.P_PushTexcoord( Vector2d(2,1)/8 );
				GLd.P_AddVertex( centerPosition + Vector2d(-reticleHalfsize.x,reticleHalfsize.y) );
				GLd.P_PushTexcoord( Vector2d(3,0)/8 );
				GLd.P_AddVertex( centerPosition + Vector2d(reticleHalfsize.x,-reticleHalfsize.y) );
				GLd.P_PushTexcoord( Vector2d(3,1)/8 );
				GLd.P_AddVertex( centerPosition + Vector2d(reticleHalfsize.x,reticleHalfsize.y) );
			GLd.EndPrimitive();
		}
	}
}

// Draws memory states
void CPlayerHud::DrawMemoryStats ( void )
{
	if ( bDrawMemoryMonitor )
	{
		unsigned long*	vals	= pMemMonitor->GetUsage();
		unsigned long	valMax	= pMemMonitor->GetPeakUsage();
		unsigned int	valNum	= pMemMonitor->GetUsageSize();
		valMax = std::max<unsigned long>( valMax, 1024 );

		GLd_ACCESS;
		// Draw the memory usage
		matDrawDebug->m_diffuse = Color( 0.0f,0,0, 0.1f );
		matDrawDebug->bindPass(0);
		GLd.DrawLineA( 0.02f,0.5f,0.02f+0.3f,0.5f );
		for ( unsigned int i = 0; i < valNum; i += 1 )
		{
			ftype xPos = i/ftype(valNum)*0.3f + 0.02f;
			ftype yOff = vals[i]/ftype(valMax) * 0.2f;
			GLd.DrawLineA( xPos, 0.5f, xPos, 0.5f-yOff );
		}
		matDrawDebug->m_diffuse = Color( 0.0f,0,0 );
	}
}

// Update player debug velocities
void CPlayerHud::DrawPlayerVelocity ( void )
{
	if ( bDrawPlayerVelocities )
	{
		if ( pPlayer->GetMotionState()->m_rigidbody )
		{
			GLd_ACCESS;
			Vector3d currentVelo = pPlayer->GetMotionState()->m_rigidbody->GetVelocity();
			matFntDebug->setTexture( 0, fntDebug );
			matFntDebug->bindPass(0);
				GLd.DrawAutoText( 0.50f,0.05f, "vx: %f", currentVelo.x );
				GLd.DrawAutoText( 0.50f,0.08f, "vy: %f", currentVelo.y );
				GLd.DrawAutoText( 0.50f,0.11f, "vz: %f", currentVelo.z );
		}
	}
}

void CPlayerHud::DrawWaypoints (void)
{
	if (!CWaypoint::WaypointList.empty())
	{
		GLd_ACCESS;
		for (auto temp = CWaypoint::WaypointList.begin(); temp != CWaypoint::WaypointList.end(); temp++)
		{
			Vector3d screenPos = CCamera::activeCamera->WorldToScreenPos( (*temp)->m_position);
			screenPos.x *= Screen::Info.width;
			screenPos.y *= Screen::Info.height;

			Vector2d reticleHalfsize = Vector2d ( 128,128 );
			GLd.BeginPrimitive( GL_TRIANGLE_STRIP );
				GLd.P_PushColor( Color(1,1,1,1) );
				GLd.P_PushTexcoord( Vector2d(0,0)/8 );
				GLd.P_AddVertex( screenPos + Vector2d(-reticleHalfsize.x,-reticleHalfsize.y) );
				GLd.P_PushTexcoord( Vector2d(0,1)/8 );
				GLd.P_AddVertex( screenPos + Vector2d(-reticleHalfsize.x,reticleHalfsize.y) );
				GLd.P_PushTexcoord( Vector2d(1,0)/8 );
				GLd.P_AddVertex( screenPos + Vector2d(reticleHalfsize.x,-reticleHalfsize.y) );
				GLd.P_PushTexcoord( Vector2d(1,1)/8 );
				GLd.P_AddVertex( screenPos + Vector2d(reticleHalfsize.x,reticleHalfsize.y) );
			GLd.EndPrimitive();
		}
	}
}