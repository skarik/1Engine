#include "sceneDeveloperMenu.h"

// Include game settings
#include "core/settings/CGameSettings.h"
// Include debug console
#include "core/debug/Console.h"
#include "engine/utils/CDeveloperConsole.h"
#include "engine-common/utils/CDeveloperConsoleUI.h"
// Include camera
#include "renderer/camera/RrCamera.h"
#include "engine-common/entities/CRenderCameraHolder.h"
// Include developer menu
#include "m04-editor/standalone/DeveloperMenu.h"
// Include loading screen
#include "engine-common/entities/CLoadingScreen.h"

void sceneDeveloperMenu::LoadScene ( void )
{
	debug::Console->PrintMessage( "Loading M04 developer menu...\n" );

	CLoadingScreen* loadScreen = new CLoadingScreen();
	loadScreen->loadStep();

	// Create the menu
	{
		M04::DeveloperMenu* menu = new M04::DeveloperMenu();
		menu->RemoveReference();
	} loadScreen->loadStep();
	

	// Create camera to render
	{
		CRenderCameraHolder* holder = new CRenderCameraHolder(new RrCamera(false));
		holder->RemoveReference();
	} loadScreen->loadStep();

	// Print a prompt
	debug::Console->PrintMessage( "You are running a debug build of M04.\n" );

	// Finish load screen.
	loadScreen->loadSetDone();
	loadScreen->RemoveReference();
}