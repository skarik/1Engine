
// Needed includes
#include "gmsceneSystemLoader.h"

// Include game state
#include "engine/state/CGameState.h"
// Include game settings
#include "core/settings/CGameSettings.h"
// Include debug console
#include "core/debug/CDebugConsole.h"
#include "engine/utils/CDeveloperConsole.h"

// Include camera for rendering
#include "engine-common/entities/CRendererHolder.h"
#include "renderer/Camera/RrCamera.h"

// Include different scenes
/*#include "after/scenes/gmsceneMainGame.h"
#include "after/scenes/gmsceneMenu.h"
#include "after/scenes/gmsceneSplashScreens.h"
#include "after/scenes/gmsceneParticleEditor.h"
#include "after/scenes/gmsceneTattooTester.h"*/

#include "engine-common/utils/CDeveloperConsoleUI.h"

void gmsceneSystemLoader::LoadScene ( void )
{
	debug::Console->PrintMessage( "Loading scene (System Loader).\n" );
	
	// Set current save files (this is temporary until the menu is finished)
	{
		//CGameSettings::Active()->SetPlayerSaveFile( "clara" );
		//CGameSettings::Active()->SetWorldSaveFile( "lucra" );
		//CGameSettings::Active()->SetTerrainSaveFile( "terra" );
	}

	// Create the console (basically our script link)
	//engine::CDeveloperConsole* devconsole = new engine::CDeveloperConsole();
	//devconsole->RemoveReference();
	CDeveloperConsoleUI* devConsole = new CDeveloperConsoleUI();
	CDeveloperCursor* devCursor = new CDeveloperCursor();

	// Check the command line now
	debug::Console->PrintMessage( "Checking command line options...\n" );
	{
		// Parse out each word and execute it in the console.
		uint i = 0;
		string currentcmd = "";
		while ( i < CGameSettings::Active()->s_cmd.length() )
		{
			if ( CGameSettings::Active()->s_cmd[i] == '-' ) {
				engine::Console->RunCommand( currentcmd );
				currentcmd = "";
			}
			else {
				currentcmd += CGameSettings::Active()->s_cmd[i];
			}
			i++;
		}
		engine::Console->RunCommand( currentcmd ); // Run the last command as well.
	}
	// Check the final external values configuration. This can be used to override settings.
	//{
	//	if ( !CGameSettings::Active()->sysprop_default_cmd.empty() ) {
	//		engine::Console->RunCommand( CGameSettings::Active()->sysprop_default_cmd );
	//	}
	//} // Disabling for now. Making it hard-coded in the game's *-common.

	// Check that they don't want to stay in the console
	bool wantsConsole = false;
	if ( CGameSettings::Active()->s_cmd.empty() )
	{
		if ( CGameSettings::Active()->s_cmd.find( "-console" ) != string::npos )
		{
			wantsConsole = true;
		}
	}

	if ( !wantsConsole )
	{
		// At this point, check if this is still the active scene. If it is, then we move to the splash screens.
		/*if ( CGameState::Active()->GetNextScene() == NULL ) {
			CGameScene* pNewScene = CGameScene::NewScene<gmsceneSplashScreens>();
			//CGameScene* pNewScene = CGameScene::NewScene<gmsceneMainGame>();
			CGameScene::SceneGoto( pNewScene );
		}*/
		engine::Console->RunCommand( "listen" );
	}
	else
	{
		// If they want to stay in the console, a camera needs to be added so the output can be rendered
		RrCamera* aCamera = new RrCamera;
		CRenderCameraHolder* holder = new CRenderCameraHolder( aCamera );
		holder->RemoveReference();
		//aCamera->RemoveReference();

#ifdef _ENGINE_DEBUG
		debug::Console->PrintMessage( "You are running a debug build of oneEngine.\n" );
		debug::Console->PrintMessage( "Press the tilde (`) to open up the command line.\n" );
		debug::Console->PrintMessage( "The following scene commands are available:\n" );
		debug::Console->PrintMessage( "\tscene test\tloads up engine debugging testbed\n" );
		debug::Console->PrintMessage( "\tscene after\tloads AFTER main game scene (may not be active)\n" );
		debug::Console->PrintMessage( "\tscene pce\tloads AFTER-Editor particle editor\n" );
		debug::Console->PrintMessage( "\tscene lse\tloads AFTER-Editor lipsync editor\n" );
#endif
	}
}