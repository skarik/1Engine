
// Needed includes
#include "gmsceneGame_6.h"

#include "core/debug/CDebugConsole.h"
#include "core/settings/CGameSettings.h"
#include "engine/utils/CDeveloperConsole.h"

#include "engine-common/entities/CPlayer.h"

#include "after/entities/menu/front/C_RMainMenu.h"

#include "engine-common/entities/CRendererHolder.h"
#include "renderer/object/screenshader/effects/CScreenSpaceOutlineShader.h"
#include "renderer/object/screenshader/effects/CBloomShader.h"
#include "renderer/object/screenshader/effects/CZoomBloomShader.h"
#include "renderer/object/screenshader/effects/CTestViewShader.h"
#include "renderer/object/screenshader/effects/CColorFilterShader.h"
#include "renderer/object/screenshader/effects/CDepthSplitShader.h"

#include "after/terrain/VoxelTerrain.h"

#include "after/entities/gametype/gametypeNotDying.h"
#include "after/entities/gametype/gametypeCharview.h"
#include "after/states/CWorldState.h"
#include "after/entities/world/environment/CEnvironmentEffects.h"
#include "after/entities/character/npc/zoned/CZonedCharacterController.h"
#include "after/entities/world/CNpcSpawner.h"
#include "after/terrain/VoxelTerrain.h"
#include "after/terrain/generation/CWorldGen_Terran.h"

#include "renderer/logic/model/CModel.h"
#include "renderer/logic/model/CSkinnedModel.h"

#include "renderer/object/util/CLoadScreenInjector.h"


void gmsceneGame_6::LoadScene ( void )
{
	Debug::Console->PrintMessage( "Loading scene. (instance of gmsceneGame_6)\n" );

	CLoadScreenInjector* loadscreen = new CLoadScreenInjector();
	loadscreen->StepScreen();

	// Hnnnnnnng
	{
		Engine::CDeveloperConsole* devconsole = new Engine::CDeveloperConsole();
		devconsole->RemoveReference();
	}

	// Precache animation models
	{
		CSkinnedModel* precache;
		precache = new CSkinnedModel( "models/character/clara.FBX" );
		delete precache;
		loadscreen->StepScreen();
		precache = new CSkinnedModel( "models/character/clara_fluxxor.FBX" );
		delete precache;
		loadscreen->StepScreen();

		precache = new CSkinnedModel( "models/character/clara/body.FBX" );
		delete precache;
		loadscreen->StepScreen();
		precache = new CSkinnedModel( "models/character/clara/body_m.FBX" );
		delete precache;
		loadscreen->StepScreen();
		precache = new CSkinnedModel( "models/character/clara/body_fluxxor.FBX" );
		delete precache;
		loadscreen->StepScreen();
	}
	loadscreen->StepScreen();

	// Screen shader
	{
		CRendererHolder* shaderHolder = new CRendererHolder( new CScreenSpaceOutlineShader() );
		shaderHolder->name = "Screen space outline shader Holder";
		shaderHolder->RemoveReference();

		if ( CGameSettings::Active()->i_ro_RendererMode == RENDER_MODE_FORWARD ) {
			CRendererHolder* shaderHolder2 = new CRendererHolder( new CTestViewShader() );
			shaderHolder2->name = "Zoom Bloom Shader Holder";
			shaderHolder2->RemoveReference();
		}

		CRendererHolder* shaderHolder3 = new CRendererHolder( new CBloomShader() );
		shaderHolder3->name = "Bloom Shader Holder";
		shaderHolder3->RemoveReference();

		CRendererHolder* shaderHolder4 = new CRendererHolder( new CColorFilterShader() );
		shaderHolder4->name = "Color Filter Shader Holder";
		shaderHolder4->RemoveReference();
	}
	loadscreen->StepScreen();
	
	// Terrain system
	VoxelTerrain* aTerrain;
	{
		aTerrain = new VoxelTerrain();
		aTerrain->SetSystemPaused( true );
		aTerrain->RemoveReference();
	}
	loadscreen->StepScreen();

	// Main menu + gametype + world
	{
		//CGameType* aGametype = new gametypeNotDying( NULL, Vector3d(0,0,0) );
		CGameType* aGametype = new gametypeCharview();
		aGametype->m_worldeffects	= new CEnvironmentEffects;
		aGametype->m_worldstate		= new CWorldState;
		aGametype->m_charactercontroller= new NPC::CZonedCharacterController;
		aGametype->m_characterspawner	= new NPC::CNpcSpawner( NULL, aGametype->m_charactercontroller );
		aGametype->m_charactercontroller->ReadyUp();

		// Disable everything in the gametype
		aGametype->m_worldeffects->active		= false;
		aGametype->m_charactercontroller->active= false;
		aGametype->m_characterspawner->active	= false;

		aGametype->m_worldstate->fTimeSpeed = 0;
		aGametype->m_worldstate->fCurrentTime = 60*60*3;

		aGametype->RemoveReference();
		// Gametype doesn't take ownership of the pointers, so release them to the world
		//aGametype->m_worldeffects->RemoveReference();
		//aGametype->m_charactercontroller->RemoveReference();
		//aGametype->m_characterspawner->RemoveReference();

		// Create the menu, giving it the gametype (it will create a new gametype)
		//C_RMainMenu* aMenu = new C_RMainMenu( aGametype );
		//aMenu->RemoveReference();
	}
	loadscreen->StepScreen();

	// Create default player
	{
		CPlayer* aPlayer = new CPlayer();
		aPlayer->RemoveReference();
	}

	// At the end, unpause the terrain
	{
		aTerrain->SetGenerator( new Terrain::CWorldGen_Terran(0) );
		aTerrain->SetSystemPaused( false ); // This will start up the terrain system
	}

	delete loadscreen;
}