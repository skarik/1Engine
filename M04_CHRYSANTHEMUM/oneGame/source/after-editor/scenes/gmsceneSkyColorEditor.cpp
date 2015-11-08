
// Needed includes
#include "gmsceneSkyColorEditor.h"

// Include game settings
#include "core/settings/CGameSettings.h"

// Used objects
#include "core/debug/CDebugConsole.h"

//#include "CVoxelFileEditor.h"
//#include "CWeatherSimulator.h"
#include "after-editor/standalone/CSkyColorEditor.h"

#include "after/entities/character/CMccCharacterModel.h"
#include "after/states/CRacialStats.h"

void gmsceneSkyColorEditor::LoadScene ( void )
{
	Debug::Console->PrintMessage( "Loading scene (sky color editor).\n" );

	// Particle Editor
	{
		//CVoxelFileEditor* aEditor = new CVoxelFileEditor;
		CSkyColorEditor* aEditor = new CSkyColorEditor;
		aEditor->RemoveReference();
	}

	// Model in center to test lighting
	{
		//pl_stats = new CPlayerStats();
		//pl_stats->LoadFromFile();
		CRacialStats* race_stats = new CRacialStats();
		race_stats->stats = new CharacterStats();
		race_stats->SetDefaults();
		race_stats->SetLuaDefaults();
		race_stats->iRace = CRACE_KITTEN;
		race_stats->RerollColors();

		CMccCharacterModel* model = new CMccCharacterModel ( NULL );
		model->LoadBase( "clara" );
		model->SetVisualsFromStats( race_stats );
	}
}