
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
#include "renderer/Camera/CCamera.h"

// Include different scenes
/*#include "after/scenes/gmsceneMainGame.h"
#include "after/scenes/gmsceneMenu.h"
#include "after/scenes/gmsceneSplashScreens.h"
#include "after/scenes/gmsceneParticleEditor.h"
#include "after/scenes/gmsceneTattooTester.h"*/

#include "engine-common/utils/CDeveloperConsoleUI.h"

void gmsceneSystemLoader::LoadScene ( void )
{
	Debug::Console->PrintMessage( "Loading scene (System Loader).\n" );
	
	// Set current save files (this is temporary until the menu is finished)
	{
		//CGameSettings::Active()->SetPlayerSaveFile( "clara" );
		//CGameSettings::Active()->SetWorldSaveFile( "lucra" );
		//CGameSettings::Active()->SetTerrainSaveFile( "terra" );
	}

	// Create the console (basically our script link)
	//Engine::CDeveloperConsole* devconsole = new Engine::CDeveloperConsole();
	//devconsole->RemoveReference();
	CDeveloperConsoleUI* devConsole = new CDeveloperConsoleUI();
	CDeveloperCursor* devCursor = new CDeveloperCursor();

	// Check the command line now
	Debug::Console->PrintMessage( "Checking command line options...\n" );
	{
		// Parse out each word and execute it in the console.
		uint i = 0;
		string currentcmd = "";
		while ( i < CGameSettings::Active()->s_cmd.length() )
		{
			if ( CGameSettings::Active()->s_cmd[i] == '-' ) {
				Engine::Console->RunCommand( currentcmd );
				currentcmd = "";
			}
			else {
				currentcmd += CGameSettings::Active()->s_cmd[i];
			}
			i++;
		}
		Engine::Console->RunCommand( currentcmd ); // Run the last command as well.
	}

	// Check that they don't want to stay in the console
	if ( !CGameSettings::Active()->s_cmd.empty() && CGameSettings::Active()->s_cmd.find( "-console" ) == string::npos )
	{
		// At this point, check if this is still the active scene. If it is, then we move to the splash screens.
		/*if ( CGameState::Active()->GetNextScene() == NULL ) {
			CGameScene* pNewScene = CGameScene::NewScene<gmsceneSplashScreens>();
			//CGameScene* pNewScene = CGameScene::NewScene<gmsceneMainGame>();
			CGameScene::SceneGoto( pNewScene );
		}*/
		Engine::Console->RunCommand( "listen" );
	}
	else
	{
		// If they want to stay in the console, a camera needs to be added so the output can be rendered
		CCamera* aCamera = new CCamera;
		//aCamera->RemoveReference();

#ifdef _ENGINE_DEBUG
		Debug::Console->PrintMessage( "You are running a debug build of oneEngine.\n" );
		Debug::Console->PrintMessage( "Press the tilde (`) to open up the command line.\n" );
		Debug::Console->PrintMessage( "The following scene commands are available:\n" );
		Debug::Console->PrintMessage( "\tscene test\tloads up engine debugging testbed\n" );
		Debug::Console->PrintMessage( "\tscene after\tloads AFTER main game scene (may not be active)\n" );
		Debug::Console->PrintMessage( "\tscene pce\tloads AFTER-Editor particle editor\n" );
		Debug::Console->PrintMessage( "\tscene lse\tloads AFTER-Editor lipsync editor\n" );
#endif
	}
}