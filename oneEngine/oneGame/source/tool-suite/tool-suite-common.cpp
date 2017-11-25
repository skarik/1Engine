#include "core/settings/CGameSettings.h"
#include "core/utils/StringUtils.h"

#include "engine/utils/CDeveloperConsole.h"
#include "engine-common/lua/LuaSys.h"
#include "engine-common/lua/Lua_Console.h"
#include "engine-common/engine-common.h"
#include "engine-common/engine-common-scenes.h"
#include "tool-suite-common.h"

#include "renderer/state/CRenderState.h"
#include "renderer/state/Options.h"

#include "physical/physics/PrPhysics.h"
#include "physical/physics/PrWorld.h"

#include "tool-suite/scenes/toolsceneModelViewer.h"


//	listen() - Called by game engine when all system setup is done.
static int listen ( std::string const& )
{
	// Check first argument to find a matching file extension.
	auto& cmd_list = CGameSettings::Active()->m_cmd;
	if ( !cmd_list.empty() )
	{
		string extension = StringUtils::ToLower(StringUtils::GetFileExtension(cmd_list[0]));
		if ( extension == "fbx" )
		{
			engine::Console->RunCommand( "scene modelviewer" );
		}
	}
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
	EngineCommon::RegisterScene<toolsceneModelViewer>( "modelviewer" );
	//EngineCommon::RegisterScene<gmsceneParticleEditor>( "pce" );
	//EngineCommon::RegisterScene<gmsceneLipsyncEditor>( "lse" );

	// Set renderer options:
	renderer::Options::TextureStreaming( false );
	CRenderState::Active->SetPipelineMode( renderer::kPipelineModeNormal );

	// Register game start command
	engine::Console->AddConsoleFunc( "listen", listen );

	return 0;
}