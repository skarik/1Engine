#include "sceneEditorSequence.h"

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

#include "m04-editor/standalone/seqeditor/SequenceEditor.h"

// Include resource system in case want to muck around with manual tileset loading
#include "core-ext/system/io/Resources.h"
// Include loading screen
#include "engine-common/entities/CLoadingScreen.h"

void sceneEditorSequence::LoadScene ( void )
{
	debug::Console->PrintMessage( "Loading scene: sequence editor.\n" );

	// Enable cursor
	ActiveCursor->SetVisible(true);

	CLoadingScreen* loadScreen = new CLoadingScreen();
	loadScreen->loadStep();

	// Create the sequence editor
	{
		m04::editor::SequenceEditor* editor = new m04::editor::SequenceEditor();
		editor->RemoveReference(); // So it can be destroyed when the game quits
	} loadScreen->loadStep();

	// Print a prompt
	debug::Console->PrintMessage( "You are running a debug build of M04.\n" );
	debug::Console->PrintMessage( "This scene provides a sequence editor and no other items.\n" );

	// Finish load screen.
	loadScreen->loadSetDone();
	loadScreen->RemoveReference();
}