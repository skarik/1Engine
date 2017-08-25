
// Needed includes
#include "gmsceneTattooTester.h"

// Include game settings
#include "core/settings/CGameSettings.h"

// Used objects
#include "after/entities/world/environment/DayAndNightCycle.h"

//#include "CCamera.h"
#include "engine-common/entities/CPlayer.h"
#include "after/developer/editor/CTattooTester.h"

#include "core/debug/CDebugConsole.h"

void gmsceneTattooTester::LoadScene ( void )
{
	Debug::Console->PrintMessage( "Loading scene (tattoo tester).\n" );
	
	// Set current save files (this is temporary until the menu is finished)
	{
		/*CGameSettings::Active()->SetPlayerSaveFile( "clara" );
		CGameSettings::Active()->SetWorldSaveFile( "lucra" );
		CGameSettings::Active()->SetTerrainSaveFile( "terra" );*/
	}

	// Sky Cycles
	{
		Daycycle* dayCycle = new Daycycle();
		dayCycle->SetTimeOfDay( 60*60*3 ); // Set to morning
		dayCycle->RemoveReference();
	}

	// Camera
	{
		//CCamera* cam = new CCamera;
		//cam->RemoveReference();
		CPlayer* observer = new CPlayer();
		observer->RemoveReference();
	}

	// Tattoo Tester
	{
		CTattooTester* tester = new CTattooTester;
		tester->RemoveReference();
	}

}