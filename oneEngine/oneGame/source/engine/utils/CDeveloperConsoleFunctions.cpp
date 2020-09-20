/*
#include "Console.h"
#include "CDeveloperConsole.h"
#include "CGameSettings.h"

// Lua
#include "CLuaController.h"

int con_execLua ( string const& cmd )
{
	Lua::Controller->RunLua( cmd );
	return 0;
}
int con_execLuaFile ( string const& file )
{
	Lua::Controller->RunLuaFile( file );
	return 0;
}
int con_luaReload ( string const& )
{
	Lua::Controller->ReloadLuaFiles();
	return 0;
}

// Materials and textures
#include "RrMaterial.h"
#include "RrTextureMaster.h"

int reload_textures ( string const& )
{
	TextureMaster.ReloadAll();
	return 0;
}
int reload_materials ( string const& )
{
	std::cout << "reload_materials() does nothing! like my goggles!" << std::endl;
	return 0;
}

// Game set
#include "CWorldState.h"

int set_time ( string const& arg )
{
	double value = atof( arg.c_str() );
	if ( ActiveGameWorld )
		ActiveGameWorld->fCurrentTime = value;
	return 0;
}

// Signals
#include "GameMessages.h"
#include "CGameState.h"

int send_rebuild_skilltree_message ( string const& arg )
{
	GameState->messenger.SendGlobal( Game::MSG_GAME_SKILLTREE_RELOAD );
	return 0;
}

// Editors + Scene skips
#include "gmsceneParticleEditor.h"
int use_particleeditor ( string const& arg )
{
	CGameScene* pNewScene = CGameScene::NewScene<gmsceneParticleEditor>();
	CGameScene::SceneGoto( pNewScene );
	return 0;
}
#include "gmsceneMenu.h"
int use_mainmenu ( const string& arg )
{
	CGameScene* pNewScene = CGameScene::NewScene<gmsceneMenu>();
	CGameScene::SceneGoto( pNewScene );
	return 0;
}
#include "gmsceneTattooTester.h"
int use_tattootester ( const string& arg )
{
	CGameScene* pNewScene = CGameScene::NewScene<gmsceneTattooTester>();
	CGameScene::SceneGoto( pNewScene );
	return 0;
}
#include "gmsceneVoxelEditor.h"
int use_voxeleditor ( const string& arg )
{
	CGameScene* pNewScene = CGameScene::NewScene<gmsceneVoxelEditor>();
	CGameScene::SceneGoto( pNewScene );
	return 0;
}
#include "gmsceneSkyColorEditor.h"
int use_skycoloreditor ( const string& arg )
{
	CGameScene* pNewScene = CGameScene::NewScene<gmsceneSkyColorEditor>();
	CGameScene::SceneGoto( pNewScene );
	return 0;
}
#include "gmsceneCharacterViewer.h"
int use_characterview ( const string& arg )
{
	CGameScene* pNewScene = CGameScene::NewScene<gmsceneCharacterViewer>();
	CGameScene::SceneGoto( pNewScene );
	return 0;
}
#include "gmsceneLipsyncEditor.h"
int use_lipsyncer ( const string& arg )
{
	CGameScene* pNewScene = CGameScene::NewScene<gmsceneLipsyncEditor>();
	CGameScene::SceneGoto( pNewScene );
	return 0;
}


// Util
#include "CGameState.h"
int gm_quit ( string const& arg )
{
	CGameState::pActive->EndGame();
	return -1;
}

// Moar util
#include "CMCCRealm.h"
#include "CMCCPlanet.h"
#include "boost/filesystem.hpp"
int gm_clear_realm ( string const& arg )
{
	string savedir = CGameSettings::Active()->GetWorldSaveDir( arg );
	savedir += "terra";
	//remove( (savedir+".
	boost::filesystem::remove_all( savedir );
	boost::filesystem::remove_all( savedir+".dungeons" );
	boost::filesystem::remove_all( savedir+".loot" );
	boost::filesystem::remove_all( savedir+".regions" );
	boost::filesystem::remove_all( savedir+".towns" );
	boost::filesystem::remove( savedir+".info" );
	boost::filesystem::remove( savedir+".settings" );
	boost::filesystem::remove( savedir+".state" );
	return 0;
}

#include "CHKAnimation.h"

// Developer console function list
void CDeveloperConsole::GenerateDefaultFunctionList ( void )
{
	// ============
	// ENGINE
	// ============
	// Lua
	AddConsoleFunc( "lua", con_execLua );
	AddConsoleFunc( "lua_file", con_execLuaFile );
	AddConsoleFunc( "lua_reload", con_luaReload );

	// Editors
	AddConsoleFunc( "particleeditor", use_particleeditor );
	AddConsoleFunc( "particleditor", use_particleeditor );
	AddConsoleFunc( "mainmenu", use_mainmenu );
	AddConsoleFunc( "tattootester", use_tattootester );
	AddConsoleFunc( "voxeleditor", use_voxeleditor );
	AddConsoleFunc( "skycoloredit", use_skycoloreditor );
	AddConsoleFunc( "gm_charview", use_characterview );
	AddConsoleFunc( "lipsyncer", use_lipsyncer );

	// ============
	// GAME
	// ============
	// Time of Day
	AddConsoleFunc( "gm_settime", set_time );

	// Settings
	AddConsoleVariable( "cl_faststart", &(CGameSettings::Active()->b_cl_FastStart) );
	AddConsoleVariable( "cl_defaultseed", &(CGameSettings::Active()->i_cl_DefaultSeed) );
	AddConsoleVariable( "cl_keyboardstyle", &(CGameSettings::Active()->i_cl_KeyboardStyle) );
	AddConsoleVariable( "cl_minimal_terrain_threads", &(CGameSettings::Active()->b_cl_MinimizeTerrainThreads) );
	AddConsoleVariable( "cl_ter_sotc_style", &(CGameSettings::Active()->b_cl_ter_ShadowOfTheCollussusRenderStyle) );

	// Util
	AddConsoleFunc( "quit", gm_quit );
	AddConsoleFunc( "clear_realm", gm_clear_realm );

	// Physics
	AddConsoleVariable( "physjoint_x", &(CHKAnimation::defaultJointVector.x) );
	AddConsoleVariable( "physjoint_y", &(CHKAnimation::defaultJointVector.y) );
	AddConsoleVariable( "physjoint_z", &(CHKAnimation::defaultJointVector.z) );

	// Messages
	AddConsoleFunc( "reload_skilltrees", send_rebuild_skilltree_message );

	// ============
	// RENDERER
	// ============
	// Renderer Options
	AddConsoleVariable( "ro_shadowmapresolution", &(CGameSettings::Active()->i_ro_ShadowMapResolution) );
	AddConsoleVariable( "ro_enableshadows", &(CGameSettings::Active()->b_ro_EnableShadows) );
	// Materials and textures
	AddConsoleFunc( "reload_textures", reload_textures );
	AddConsoleFunc( "reload_materials", reload_materials );
}
*/