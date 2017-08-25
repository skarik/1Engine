
// Needed includes
#include "gmsceneSplashScreens.h"

// Include game settings
#include "core/settings/CGameSettings.h"

#include "core/debug/CDebugConsole.h"

// Used objects
#include "after/entities/world/environment/DayAndNightCycle.h"
#include "after/entities/world/environment/CloudSphere.h"
#include "after/entities/menu/splash/CSplashScreens.h"

//#include "after/entities/menu/CTerrainCollisionLoader.h"

#include "engine-common/entities/CRendererHolder.h"
#include "renderer/object/screenshader/effects/CScreenSpaceOutlineShader.h"
#include "renderer/object/screenshader/effects/CZoomBloomShader.h"
#include "renderer/object/screenshader/effects/CBloomShader.h"
#include "renderer/object/screenshader/effects/CColorFilterShader.h"
#include "renderer/object/screenshader/effects/CTestViewShader.h"

#include "after/renderer/objects/hud/Ploverlay_Icing.h"

void gmsceneSplashScreens::LoadScene ( void )
{
	Debug::Console->PrintMessage( "Loading scene (splash screens).\n" );
	
	// Set current save files (this is temporary until the menu is finished)
	/*{
		CGameSettings::Active()->SetPlayerSaveFile( "clara" );
		CGameSettings::Active()->SetWorldSaveFile( "lucra" );
		CGameSettings::Active()->SetTerrainSaveFile( "terra" );
	}*/

	// Particle Editor
	/*{
		CParticleEditor* aParticleEditor = new CParticleEditor;
		aParticleEditor->RemoveReference();
	}*/

	// Screen shader
	{
		CRendererHolder* shaderHolder = new CRendererHolder( new CScreenSpaceOutlineShader() );
		shaderHolder->RemoveReference();

		if ( CGameSettings::Active()->i_ro_RendererMode == RENDER_MODE_FORWARD ) {
			CRendererHolder* shaderHolder2 = new CRendererHolder( new CTestViewShader() );
			shaderHolder2->RemoveReference();
		}

		CRendererHolder* shaderHolder3 = new CRendererHolder( new CBloomShader() );
		shaderHolder3->RemoveReference();
	}

	// Terrain collision background loader to speed up getting into game
	/*{
		CTerrainCollisionLoader* loader = new CTerrainCollisionLoader();
		loader->RemoveReference();
	}*/

	/*{
		Ploverlay_Icing* icingOnTheCake = new Ploverlay_Icing();
		icingOnTheCake->RemoveReference();
	}*/


	// Sky Cycles
	{
		Daycycle* dayCycle = new Daycycle();
		//dayCycle->SetTimeOfDay( 60*60*3 ); // Set to nine in the morning
		dayCycle->SetTimeOfDay( 60*60*-1 ); // Set to five in the morning
		dayCycle->RemoveReference();

		CloudSphere* cloudCycle = new CloudSphere();
		cloudCycle->SetCloudDensity( 0.3f );
		cloudCycle->SetTimeOfDay( 60*60*-1 );
		cloudCycle->RemoveReference();
	}

	// Splash Screens
	{
		CSplashScreens* splash = new CSplashScreens();
		splash->RemoveReference();
	}

}