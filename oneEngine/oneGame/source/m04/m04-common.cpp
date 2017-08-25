
#include "physical/liscensing.cxx" // Include liscense info

#include "engine/utils/CDeveloperConsole.h"
#include "engine-common/lua/LuaSys.h"
#include "engine-common/lua/Lua_Console.h"
#include "engine-common/engine-common.h"
#include "engine-common/engine-common-scenes.h"
#include "m04-common.h"
/*
#include "after/scenes/gmsceneCharacterViewer.h"
#include "after-editor/scenes/gmsceneParticleEditor.h"
#include "after-editor/scenes/gmsceneLipsyncEditor.h"

#include "after/physics/water/Water.h"
#include "after/physics/wind/WindMotion.h"
*/
#include "renderer/state/Options.h"

#include "physical/physics/PrPhysics.h"
#include "physical/physics/PrWorld.h"

#include "m04/scenes/sceneTilesetTest.h"
#include "m04/scenes/sceneGameLuvPpl.h"
#include "m04-editor/scenes/sceneEditorMain.h"
#include "m04-editor/scenes/sceneDeveloperMenu.h"

//===============================================================================================//
//	GameInitialize
//
// After the main engine components have been started, this function is called.
// This calls EngineCommonInitialize first, then performs its own actions.
// It adds game specific bindings to the console, Lua system, and other systems.
//===============================================================================================//
int GameInitialize ( void )
{
	// Engine
	EngineCommonInitialize();

	// Create physics world
	{
		prWorldCreationParams params = {0};
		PrPhysics::Active()->AddWorld( new PrWorld(params) );
	}

	// Scene registration
	EngineCommon::RegisterScene<sceneTilesetTest>( "test0" );
	EngineCommon::RegisterScene<sceneEditorMain>( "test1" );
	EngineCommon::RegisterScene<sceneDeveloperMenu>( "m04devmenu" );
	EngineCommon::RegisterScene<sceneGameLuvPpl>( "game_luvppl" );
	//EngineCommon::RegisterScene<gmsceneParticleEditor>( "pce" );
	//EngineCommon::RegisterScene<gmsceneLipsyncEditor>( "lse" );

	//===============================================================================================//
	// PLACE YOUR CUSTOM ENGINE MODULES BELOW HERE
	//===============================================================================================//

	// create testers
	//new CAfterWaterTester();
	//new CWindMotion();
	renderer::Options::TextureStreaming( false );

	return 0;
}