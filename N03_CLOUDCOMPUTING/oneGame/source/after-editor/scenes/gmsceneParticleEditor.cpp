
// Needed includes
#include "gmsceneParticleEditor.h"

// Include game settings
#include "core/settings/CGameSettings.h"

// Used objects
//#include "DayAndNightCycle.h"

#include "after-editor/standalone/CParticleEditor.h"
//#include "CWeatherSimulator.h"

#include "core/debug/CDebugConsole.h"

void gmsceneParticleEditor::LoadScene ( void )
{
	Debug::Console->PrintMessage( "Loading scene (particle editor).\n" );
	
	// Set current save files (this is temporary until the menu is finished)
	{
		/*CGameSettings::Active()->SetPlayerSaveTarget( "clara" );
		CGameSettings::Active()->SetRealmSaveTarget( "lucra" );
		CGameSettings::Active()->SetWorldSaveTarget( "terra" );*/
	}

	// Particle Editor
	{
		CParticleEditor* aParticleEditor = new CParticleEditor;
		aParticleEditor->RemoveReference();
	}

	// Sky Cycles
	/*{
		Daycycle* dayCycle = new Daycycle();
		dayCycle->SetTimeOfDay( 60*60*18 ); // Set to midnight
		dayCycle->RemoveReference();
	}*/

}