
//#include "physical/liscensing.cxx" // Include liscense info
#include "core/legacy/linkerfix_legacy.h"

#include "engine/utils/CDeveloperConsole.h"
#include "engine-common/lua/LuaSys.h"
#include "engine-common/lua/Lua_Console.h"
#include "engine-common/engine-common.h"
#include "engine-common/engine-common-scenes.h"
#include "cloud-common.h"

#include "cloud/scenes/sceneCloudGame.h"
#include "after/scenes/tests/testSceneStaticMesh.h"
#include "after-editor/scenes/gmsceneParticleEditor.h"
#include "after-editor/scenes/gmsceneLipsyncEditor.h"
#include "after-editor/scenes/gmsceneVoxelEditor.h"

#include "after/physics/water/Water.h"
#include "after/physics/wind/WindMotion.h"

int Cloud_Gamestart ( const std::string& sceneName )
{
	return EngineCommon::LoadScene("game");
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
	// Engine
	EngineCommonInitialize();

	Engine::Console->AddConsoleFunc( "listen",	Cloud_Gamestart );

	// Scene registration
	EngineCommon::RegisterScene<sceneCloudGame>( "game" );
	//EngineCommon::RegisterScene<gmsceneCharacterViewer>( "test" );

	EngineCommon::RegisterScene<gmsceneParticleEditor>( "pce" );
	EngineCommon::RegisterScene<gmsceneLipsyncEditor>( "lse" );
	EngineCommon::RegisterScene<gmsceneVoxelEditor>( "vxe" );

	EngineCommon::RegisterScene<testSceneStaticMesh>( "ttsm" );

	//===============================================================================================//
	// PLACE YOUR CUSTOM ENGINE MODULES BELOW HERE
	//===============================================================================================//

	// create testers
	new CAfterWaterTester();
	new CWindMotion();

	return 0;
}