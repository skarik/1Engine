
// Needed includes
#include "gmsceneLipsyncEditor.h"

#include "core/debug/CDebugConsole.h"
#include "core/settings/CGameSettings.h"
#include "engine/utils/CDeveloperConsole.h"

#include "after/entities/menu/front/C_RMainMenu.h"

#include "engine-common/entities/CRendererHolder.h"
#include "renderer/object/screenshader/effects/CScreenSpaceOutlineShader.h"
#include "renderer/object/screenshader/effects/CBloomShader.h"
#include "renderer/object/screenshader/effects/CZoomBloomShader.h"
#include "renderer/object/screenshader/effects/CColorFilterShader.h"
#include "renderer/object/screenshader/effects/CTestViewShader.h"

#include "after/terrain/VoxelTerrain.h"
#include "after/entities/test/CTestPhysicsFloor.h"
#include "engine-common/entities/CPlayer.h"
#include "after/entities/character/CAfterPlayer.h"

#include "after/entities/gametype/CGameType.h"
#include "after/states/CWorldState.h"
#include "after/entities/world/environment/CEnvironmentEffects.h"
#include "after/entities/character/npc/zoned/CZonedCharacterController.h"
#include "after/entities/world/CNpcSpawner.h"

#include "after/states/CharacterStats.h"
#include "after/states/player/CPlayerStats.h"
#include "after/entities/character/npc/CNpcBase.h"
#include "after/entities/character/npc/zoned/CZonedCharacter.h"
#include "after/interfaces/io/CZonedCharacterIO.h"

#include "after-editor/standalone/CLipsyncEditor.h"

#include "renderer/logic/model/CModel.h"
#include "renderer/logic/model/CSkinnedModel.h"

#include "renderer/object/util/CLoadScreenInjector.h"
#include "renderer/material/glMaterial.h"

#include "renderer/object/shapes/CCubeRenderablePrimitive.h"

void gmsceneLipsyncEditor::LoadScene ( void )
{
	Debug::Console->PrintMessage( "Loading scene. (instance of gmsceneCharacterViewer)\n" );

	CLoadScreenInjector* loadscreen = new CLoadScreenInjector();
	loadscreen->StepScreen();

	// Hnnnnnnng
	{
		Engine::CDeveloperConsole* devconsole = new Engine::CDeveloperConsole();
		devconsole->RemoveReference();
	}

	// Go to _debugging for debugging
	{
		CGameSettings::Active()->SetWorldSaveFile( "_debugging" );
		CGameSettings::Active()->SetTerrainSaveFile( "terra" );
		CGameSettings::Active()->SetPlayerSaveFile( "clara" );
	}
	loadscreen->StepScreen();

	// Precache animation models
	{
		CSkinnedModel* precache;
		precache = new CSkinnedModel( ".res/models/character/clara.FBX" );
		delete precache;
		loadscreen->StepScreen();
		precache = new CSkinnedModel( ".res/models/character/clara_fluxxor.FBX" );
		delete precache;
		loadscreen->StepScreen();

		precache = new CSkinnedModel( ".res/models/character/clara/body.FBX" );
		delete precache;
		loadscreen->StepScreen();
		precache = new CSkinnedModel( ".res/models/character/clara/body_m.FBX" );
		delete precache;
		loadscreen->StepScreen();
		precache = new CSkinnedModel( ".res/models/character/clara/body_fluxxor.FBX" );
		delete precache;
		loadscreen->StepScreen();
	}
	loadscreen->StepScreen();

	// Screen shader
	{
		if ( CGameSettings::Active()->i_ro_RendererMode == RENDER_MODE_FORWARD )
		{
			CRendererHolder* shaderHolder = new CRendererHolder( new CScreenSpaceOutlineShader() );
			shaderHolder->name = "Screen space outline shader Holder";
			shaderHolder->RemoveReference();
		
			/*CRendererHolder* shaderHolder2 = new CRendererHolder( new CZoomBloomShader() );
			shaderHolder2->name = "Zoom Bloom Shader Holder";
			shaderHolder2->RemoveReference();*/

			CRendererHolder* shaderHolder3 = new CRendererHolder( new CBloomShader() );
			shaderHolder3->name = "Bloom Shader Holder";
			shaderHolder3->RemoveReference();

			CRendererHolder* shaderHolder4 = new CRendererHolder( new CColorFilterShader() );
			shaderHolder4->name = "Color Filter Shader Holder";
			shaderHolder4->RemoveReference();

			CRendererHolder* shaderHolder5 = new CRendererHolder( new CTestViewShader() );
			shaderHolder5->name = "Testview Shader Holder";
			shaderHolder5->RemoveReference();
		}
		else
		{
			CRendererHolder* shaderHolder = new CRendererHolder( new CScreenSpaceOutlineShader() );
			shaderHolder->name = "Screen space outline shader Holder";
			shaderHolder->RemoveReference();

			CRendererHolder* shaderHolder3 = new CRendererHolder( new CBloomShader() );
			shaderHolder3->name = "Bloom Shader Holder";
			shaderHolder3->RemoveReference();

			CRendererHolder* shaderHolder4 = new CRendererHolder( new CColorFilterShader() );
			shaderHolder4->name = "Color Filter Shader Holder";
			shaderHolder4->RemoveReference();
		}
	}
	loadscreen->StepScreen();

	// Main menu + gametype + world
	{
		CGameType* aGametype = new CGameType();
		aGametype->m_worldeffects	= new CEnvironmentEffects;
		aGametype->m_worldstate		= new CWorldState;
		aGametype->m_charactercontroller= new NPC::CZonedCharacterController;
		aGametype->m_characterspawner	= new NPC::CNpcSpawner( NULL, aGametype->m_charactercontroller );
		aGametype->m_charactercontroller->ReadyUp();

		// Disable everything in the gametype
		aGametype->m_worldeffects->active		= false;
		aGametype->m_charactercontroller->active= false;
		aGametype->m_characterspawner->active	= false;

		// Create the menu, giving it the gametype (it will create a new gametype)
		//C_RMainMenu* aMenu = new C_RMainMenu( aGametype );
		//aMenu->RemoveReference();
	}
	loadscreen->StepScreen();
	
	// Octree terrain
	/*{
		COctreeTerrain* aTerrain = new COctreeTerrain();
		aTerrain->SetSystemPaused( true );
		aTerrain->RemoveReference();
	}*/

	// Create the stage
	{
		CTestPhysicsFloor* stageFloor = new CTestPhysicsFloor;

		glMaterial* newMaterial = glMaterial::Default->copy();
		newMaterial->m_diffuse = Color( 0.6,0.6,0.6 );
		newMaterial->setTexture( 0, new CTexture( ".res/textures/rockdiffuse.jpg" ) );

		//newMaterial->deferredinfo.push_back( glPass_Deferred() );
		//newMaterial->deferredinfo[0].

		stageFloor->pRenderable->SetMaterial( newMaterial );
		newMaterial->removeReference();

		stageFloor->RemoveReference();
	}
	loadscreen->StepScreen();

	// Create a test model
	/*{
		CSkinnedModel* modelTest = new CSkinnedModel( ".res/models/robot/automaton.FBX" );
		modelTest->transform.position = Vector3d( -8,8,5 );
	}*/
	/*{
		CSkinnedModel* modelTest = new CSkinnedModel( ".res/models/fauna/drakes/komodo_lesser_drake.FBX" );
		modelTest->transform.position = Vector3d( -8,20,5 );
	}*/

	// Camera control
	/*{
		CPlayer* observer = new CMccPlayer(NULL);
		observer->transform.position = Vector3d( -4,-4,6 );
		observer->transform.SetDirty();
		observer->RemoveReference();
	}
	loadscreen->StepScreen();*/

	// Create stats for the character
	CPlayerStats* pl_stats;
	{
		pl_stats = new CPlayerStats();
		CRacialStats* rstats = pl_stats->race_stats;
		rstats->SetDefaults();

		rstats->iGender = CGEND_FEMALE;
		//rstats->iRace = CRACE_ELF;
		rstats->iRace = CRACE_FLUXXOR;

		rstats->RerollColors();
		rstats->cEyeColor = Color( 1.0, 0.3, 1.0 );
		rstats->cFocusColor = Color( 1.0, 0.7, 0.5 );
		rstats->cHairColor = Color( 0.8, 0.38, 0.33 );
		rstats->cSkinColor = Color( 0.3, 0.27, 0.37 );
		rstats->LimitColors();

		rstats->iHairstyle = 0;

		rstats->SetLuaDefaults();

		// Set facial tattoo
		/*CRacialStats::tattoo_t tattoo;
		tattoo.color = Color( 0.5,0.14,0.12,2.0 ) * 0.5f;
		tattoo.mirror = false;
		tattoo.type = TATT_CLANMARK;
		tattoo.pattern = "clan_skullblack";
		tattoo.projection_angle = 160.0f;
		tattoo.projection_dir = Vector3d( 0,1,0 );
		tattoo.projection_pos = Vector3d( 0,-15,28 ) / 12.0f;
		tattoo.projection_scale = Vector3d(1,1,1) * 0.83f;

		rstats->tattooList.push_back( tattoo );*/
		rstats->iTattooCount = rstats->tattooList.size();
	}
	loadscreen->StepScreen();

	// Create a character
	NPC::CNpcBase* m_playercharacter;
	{
		for ( uint i = 0; i < 1; ++i )
		{
			//uint64_t player_id = NPC::Manager->RequestNPC( NPC::npcid_SETPIECE, 0 );
			uint64_t player_id = NPC::Manager->RequestNPC( NPC::npcid_UNIQUE, 0 );
			{
				NPC::characterFile_t characterFile;
				characterFile.rstats = pl_stats->race_stats;
			
				NPC::sWorldState worldstate;
				characterFile.worldstate = &worldstate;
				worldstate.mFocus = NPC::AIFOCUS_None;
				worldstate.mFocusName = "";
				worldstate.worldPosition = Vector3d( 0,0,20 );
				worldstate.partyHost = 1024;

				worldstate.mFaction = NPC::FactionNone;

				NPC::sOpinions opinions;
				characterFile.opinions = &opinions;
				NPC::sPreferences prefs;
				characterFile.prefs = &prefs;

				// Save generated stats
				NPC::CZonedCharacterIO io;
				io.CreateCharacterFile( player_id, characterFile );
			}
			m_playercharacter = (NPC::CNpcBase*) NPC::Manager->SpawnNPC(player_id);
		}
	}
	loadscreen->StepScreen();
	
	// Spawn an automaton
	{
	//	NPC::Manager->SpawnFauna( "RobotAutomaton", Vector3d( -8,8,5 ) );
	}

	// Create the editor
	{
	//	CAiTester* tester = new CAiTester();
	//	tester->RemoveReference();
		CLipsyncEditor* editor = new CLipsyncEditor();
		editor->RemoveReference();
	}
	loadscreen->StepScreen();

	// Free the stats
	{
		delete pl_stats->stats;
		delete pl_stats->race_stats;
		delete pl_stats;
	}
	loadscreen->StepScreen();

	delete loadscreen;
}