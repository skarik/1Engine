
// Needed includes
#include "gmsceneVoxelEditor.h"

// Include game settings
#include "core/settings/CGameSettings.h"

// Used objects
#include "core/debug/CDebugConsole.h"

#include "after/developer/editor/CVoxelFileEditor.h"
//#include "CWeatherSimulator.h"

void gmsceneVoxelEditor::LoadScene ( void )
{
	Debug::Console->PrintMessage( "Loading scene (voxel editor).\n" );

	// Particle Editor
	{
		CVoxelFileEditor* aEditor = new CVoxelFileEditor;
		aEditor->RemoveReference();
	}

}