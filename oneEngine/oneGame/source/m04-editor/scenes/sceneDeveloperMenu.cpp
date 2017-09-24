#include "sceneDeveloperMenu.h"

// Include game settings
#include "core/settings/CGameSettings.h"
// Include debug console
#include "core/debug/CDebugConsole.h"
#include "engine/utils/CDeveloperConsole.h"
#include "engine-common/utils/CDeveloperConsoleUI.h"
// Include camera
#include "renderer/camera/CCamera.h"
#include "engine-common/entities/CRenderCameraHolder.h"
// Include developer menu
#include "m04-editor/standalone/DeveloperMenu.h"

void sceneDeveloperMenu::LoadScene ( void )
{
	debug::Console->PrintMessage( "Loading M04 developer menu...\n" );

	// Create the menu
	{
		M04::DeveloperMenu* menu = new M04::DeveloperMenu();
		menu->RemoveReference();
	}

	// Create camera to render
	{
		CRenderCameraHolder* holder = new CRenderCameraHolder(new CCamera());
		holder->RemoveReference();
	}

	// Print a prompt
	debug::Console->PrintMessage( "You are running a debug build of M04.\n" );
}