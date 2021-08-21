#include "sceneEditorNoise.h"

// Include game settings
#include "core/settings/CGameSettings.h"
// Include debug console
#include "core/debug/Console.h"
#include "engine/utils/CDeveloperConsole.h"
#include "engine-common/utils/CDeveloperConsoleUI.h"
// Include 2d camera
#include "render2d/camera/COrthoCamera.h"
// Include cutscene editor editor
#include "m04-editor/standalone/CutsceneEditor.h"

#include "m04-editor/standalone/noiseeditor/NoiseEditor.h"

// Include resource system in case want to muck around with manual tileset loading
#include "core-ext/system/io/Resources.h"
// Include loading screen
#include "engine-common/entities/CLoadingScreen.h"

void sceneEditorNoise::LoadScene ( void )
{
	debug::Console->PrintMessage( "Loading scene: noise editor.\n" );

	// Enable cursor
	ActiveCursor->SetVisible(true);

	// Create the sequence editor
	{
		m04::editor::NoiseEditor* editor = new m04::editor::NoiseEditor();
		editor->RemoveReference(); // So it can be destroyed when the game quits
	}
}