#include "sceneEditorMaterial.h"

// Include game settings
#include "core/settings/CGameSettings.h"
// Include debug console
#include "core/debug/Console.h"
#include "engine/utils/CDeveloperConsole.h"
#include "engine-common/utils/CDeveloperConsoleUI.h"

void sceneEditorMaterial::LoadScene ( void )
{
	// Enable cursor
	ActiveCursor->SetVisible(true);

	// Create the sequence editor
	{
		//m04::editor::NoiseEditor* editor = new m04::editor::NoiseEditor();
		//editor->RemoveReference(); // So it can be destroyed when the game quits
	}
}