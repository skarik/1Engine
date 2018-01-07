#include "engine/utils/CDeveloperConsole.h"
#include "engine-common/lua/LuaSys.h"
#include "engine-common/lua/Lua_Console.h"
#include "engine-common/engine-common.h"
#include "engine-common/engine-common-scenes.h"
#include "m04-common.h"

#include "renderer/state/CRenderState.h"
#include "renderer/state/Options.h"

#include "physical/physics/PrPhysics.h"
#include "physical/physics/PrWorld.h"

#include "m04/scenes/sceneTilesetTest.h"
#include "m04/scenes/sceneGameLuvPpl.h"
#include "m04-editor/scenes/sceneEditorMain.h"
#include "m04-editor/scenes/sceneDeveloperMenu.h"
#include "m04-editor/scenes/sceneEditorCutscene.h"

namespace M04
{
	arstring<256>	m04NextLevelToLoad;
}

//	listen() - Called by game engine when all system setup is done.
static int listen ( std::string const& )
{
#ifdef _ENGINE_DEBUG
	//engine::Console->RunCommand( "scene game_luvppl" );
	//engine::Console->RunCommand( "scene m04devmenu" );
	engine::Console->RunCommand( "scene editorcts" );
#else
	engine::Console->RunCommand( "scene game_luvppl" );
#endif
	return 0;
}


//===============================================================================================//
//	GameInitialize
//
// After the main engine components have been started, this function is called.
// This calls EngineCommonInitialize first, then performs its own actions.
// It adds game specific bindings to the console, Lua system, and other systems.
//===============================================================================================//
int GameInitialize ( void )
{
	M04::m04NextLevelToLoad = "maps/test2.m04";

	// Engine. Must always be called else features are not initialized!
	EngineCommonInitialize();

	//===============================================================================================//
	// PLACE YOUR CUSTOM ENGINE MODULES BELOW HERE
	//===============================================================================================//

	// Create physics world
	{
		prWorldCreationParams params = {0};
		PrPhysics::Active()->AddWorld( new PrWorld(params) );
	}

	// Scene registration
	EngineCommon::RegisterScene<sceneTilesetTest>( "test0" );
	EngineCommon::RegisterScene<sceneEditorMain>( "editorm04" );
	EngineCommon::RegisterScene<sceneEditorCutscene>( "editorcts" );
	EngineCommon::RegisterScene<sceneDeveloperMenu>( "menu" );
	EngineCommon::RegisterScene<sceneDeveloperMenu>( "m04devmenu" );
	EngineCommon::RegisterScene<sceneGameLuvPpl>( "game_luvppl" );
	//EngineCommon::RegisterScene<gmsceneParticleEditor>( "pce" );
	//EngineCommon::RegisterScene<gmsceneLipsyncEditor>( "lse" );

	// Set renderer options:
	renderer::rrDeferredShaderSettings shaderSettings;
	shaderSettings.filenamePrimaryVertexShader = "shaders/def_alt/surface_default_2d.vert";
	shaderSettings.filenamePrimaryPixelShader  = "shaders/def_alt/surface_default_2d.frag";

	renderer::Options::DeferredShaders( shaderSettings );
	renderer::Options::TextureStreaming( false );
	CRenderState::Active->SetPipelineMode( renderer::kPipelineMode2DPaletted );

	// Register game start command
	engine::Console->AddConsoleFunc( "listen", listen );

	return 0;
}