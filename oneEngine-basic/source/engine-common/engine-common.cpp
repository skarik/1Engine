
//#include "physical/liscensing.cxx" // Include liscense info

#include "engine/utils/CDeveloperConsole.h"
#include "renderer/renderer-console.h"

#include "engine-common/lua/LuaSys.h"
#include "engine-common/lua/Lua_Console.h"
#include "engine-common.h"
#include "engine-common-console.h"

#include "engine-common-scenes.h"
#include "engine-common/scenes/gmsceneFromFile.h"
#include "engine-common/scenes/gmsceneSystemLoader.h"
#include "engine-common/scenes/gmsceneSystemBuilder.h"
#include "engine-common/scenes/benchmark/benchmarkSemaphores.h"
#include "engine-common/scenes/benchmark/benchmarkMatrices.h"

//===============================================================================================//
//	EngineCommonInitialize
//
// After the main engine components have been started, this function is called.
// It adds the default engine bindings to the console, Lua system, and other systems.
//===============================================================================================//
int EngineCommonInitialize ( void )
{
	// Engine
	Engine::Console->AddConsoleFunc( "scene",	EngineCommon::LoadScene );
	EngineCommon::RegisterScene<gmsceneSystemLoader>( "default" );
	EngineCommon::RegisterScene<gmsceneSystemBuilder>( "sysbuild" );
	EngineCommon::RegisterScene<benchmarkSemaphores>( "benchmark_semaphore" );
	EngineCommon::RegisterScene<benchmarkMatrices>( "benchmark_matrices" );

	// Lua
	Engine::Console->AddConsoleFunc( "lua",	Lua::con_execLua );
	Engine::Console->AddConsoleFunc( "lua_file",	Lua::con_execLuaFile );
	Engine::Console->AddConsoleFunc( "lua_reload",	Lua::con_luaReload );

	// Register Lua info
	Lua::Register_GameState( *Lua::Controller );
	Lua::Register_Renderer( *Lua::Controller );
	//Lua::Register_GameObjects( *Lua::Controller );
	Lua::Controller->RunLuaFile( "sys_engine.lua" );

	// Register Renderer info
	Engine::Console->AddConsoleFunc( "recompile_shader",	Renderer::con_recompile_shader );
	Engine::Console->AddConsoleFunc( "recompile_shaders",	Renderer::con_recompile_shaders );

	// Debug tools
	Engine::Console->AddConsoleFunc( "showtimes", EngineCommon::DebugToggleTimeProfilerUI );

	//===============================================================================================//
	// PLACE YOUR CUSTOM ENGINE MODULES BELOW HERE
	//===============================================================================================//

	// Return success
	return 0;
}


#include "engine-common-scenes.h"

// List of scene instantiators here
std::vector<EngineCommon::_sceneEntry_t> EngineCommon::_sceneListing;

//	LoadScene(scene name)
// Given an input, loads the given scene. Returns 0 on success.
int EngineCommon::LoadScene ( const std::string& sceneName )
{
	// Loop through the scene listing to find a matching scene
	for ( uint i = 0; i < _sceneListing.size(); ++i )
	{
		if ( _sceneListing[i].first == sceneName )
		{
			_sceneListing[i].second();
			return 0;
		}
	}

	// Try to find a map file that matches if still here
	{
		// gmsceneFromFile will be used. For now, no file found.
	}

	Debug::Console->PrintError( "Could not find map \""+sceneName+"\"\n" );
	return -1;
}