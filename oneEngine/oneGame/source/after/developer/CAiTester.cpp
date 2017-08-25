
#include "CAiTester.h"

#include "core/input/CInput.h"

#include "core-ext/input/CInputControl.h"

#include "engine/utils/CDeveloperConsole.h"
#include "engine/physics/raycast/Raycaster.h"

#include "engine-common/network/playerlist.h"
#include "engine-common/entities/CPlayer.h"

#include "renderer/light/CLight.h"
#include "renderer/material/glMaterial.h"

#include "after/entities/character/CAfterPlayer.h"
#include "after/entities/character/npc/zoned/CZonedCharacterController.h"
#include "after/entities/character/npc/CNpcBase.h"
#include "after/entities/world/CNpcSpawner.h"
#include "after/interfaces/io/CZonedCharacterIO.h"
#include "after/entities/test/CTestPhysicsCrate.h"

//Josh fix your shit

CAiTester::CAiTester ( void )
	: CGameBehavior()
{

}

void CAiTester::Update ( void )
{
	CInputControl* input = CInputControl::GetActive();
	if ( input->GetUser() == Debug::Console ) {
		return;
	}

	// Grab player list
	auto playerList = Network::GetPlayerActors();
	if ( playerList.empty() ) {
		return;
	}

	//if ( playerList[0].actor->GetCombatState() != NULL ) {
	if ( playerList[0].actor->ActorType() == ACTOR_TYPE_PLAYER ) {
		return;
	}

	// Swap vision modes
	if ( CInput::Keydown( 'V' ) ) {
		if ( glMaterial::special_mode == Renderer::SP_MODE_NORMAL ) {
			glMaterial::special_mode = Renderer::SP_MODE_ECHO;
		}
		else {
			glMaterial::special_mode = Renderer::SP_MODE_NORMAL;
		}
	}

	// Add a player
	if ( CInput::Keydown( 'P' ) ) {
		Vector3d spawn_position = playerList[0].actor->transform.position + Vector3d( 0,0,2 );
		DeleteObject( playerList[0].actor );
		CCharacter* observer = new CAfterPlayer(NULL);
		observer->transform.position = spawn_position;
		observer->transform.SetDirty();
		observer->RemoveReference();
	}

	// Find an NPC and give it a command
	if ( input->axes.secondary.pressed() )
	{
		// Raytrace from player view
		Ray ray = playerList[0].actor->GetEyeRay();
		RaycastHit hitInfo;
		Raycaster.Raycast( ray, 2000, &hitInfo, Physics::GetCollisionFilter(Layers::PHYS_WORLD_TRACE) );
		if ( hitInfo.hit )
		{
			// Find NPC
			NPC::CNpcBase* mNpc = (NPC::CNpcBase*)NPC::Manager->GetNPC(0);
			if ( mNpc )
			{
				NPC::AIAccessor ai_l( mNpc->GetAI() );
				ai_l.MoveTo( hitInfo.hitPos, true );
			}
		}
	}
	
	// Create a bandit
	if ( input->axes.primary.pressed() )
	{
		// Raytrace from player view
		Ray ray = playerList[0].actor->GetEyeRay();
		RaycastHit hitInfo;
		Raycaster.Raycast( ray, 2000, &hitInfo, Physics::GetCollisionFilter(Layers::PHYS_WORLD_TRACE) );

		if ( hitInfo.hit )
		{
			CTestPhysicsCrate* newCrate = new CTestPhysicsCrate();
			newCrate->transform.position = hitInfo.hitPos + hitInfo.hitNormal*5;
			newCrate->transform.rotation = Vector3d( Random.Range(-180,180), Random.Range(-180,180), Random.Range(-180,180) );
			newCrate->transform.SetDirty();

			CLight* newLight = new CLight;
			//newLight->transform.position = newCrate->transform.position;
			newLight->transform.SetParent( &(newCrate->transform) );
			
			newLight->diffuseColor = Color( Random.Range(0.2f,1.2f),Random.Range(0.2f,1.2f),Random.Range(0.2f,1.2f) );
			newLight->range = 12;

			/*NPC::npcid_t banditId = NPC::Manager->RequestNPC( NPC::npcid_UNIQUE );
			{
				NPC::characterFile_t characterFile;

				CRacialStats rstats;
				rstats.stats = new CharacterStats;
				rstats.SetDefaults();
				characterFile.rstats = &rstats;
				{
					// Set race
					rstats.iRace = CRACE_ELF;
					// Set gender
					rstats.iGender = eCharacterGender(Random.Next()%2);
					// Randomize hairstyle
					rstats.iHairstyle = Random.Next()%3;
					// Set companion colors
					rstats.RerollColors();
			
					// Set facial tattoo
					CRacialStats::tattoo_t tattoo;
					tattoo.color = Color( Random.Range(0.1f,0.5f),Random.Range(0.1f,0.5f),Random.Range(0.1f,0.5f),1.0f );
					tattoo.mirror = false;
					tattoo.type = TATT_CLANMARK;
					tattoo.pattern = "clan_skullblack";
					tattoo.projection_angle = Random.Range( 150.0f, 180.0f );
					tattoo.projection_dir = Vector3d( 0,1,0 );
					tattoo.projection_pos = Vector3d( 0,-15,28 ) / 12.0f;
					tattoo.projection_scale = Vector3d(1,1,1) * Random.Range( 0.77f, 0.92f );
					if ( Random.Chance(0.5f) ) tattoo.projection_scale.x *= -1;

					rstats.tattooList.push_back( tattoo );
					rstats.iTattooCount = rstats.tattooList.size();

					// Set name
					rstats.sPlayerName = "Bandit";
					rstats.sLastName = "";
				}

				NPC::sWorldState worldstate;
				{
					characterFile.worldstate = &worldstate;
					worldstate.mFocus = NPC::AIFOCUS_Wanderer; // should be bandit focus, so can turn to BanditRunner faction when 
					worldstate.travelDirection = Vector3d( 1,0,0 );
					worldstate.worldPosition = hitInfo.hitPos + Vector3d( 0,0,5 );
					worldstate.partyHost = banditId;

					worldstate.mFaction = NPC::FactionBandit;
				}

				NPC::sOpinions opinions;
				characterFile.opinions = &opinions;

				NPC::sPreferences prefs;
				characterFile.prefs = &prefs;

				// Save generated stats
				NPC::CZonedCharacterIO io;
				io.CreateCharacterFile( banditId, characterFile );
				// Clean up
				delete rstats.stats;
			}

			// Create bandit :D
			NPC::CNpcBase* bandit = (NPC::CNpcBase*) NPC::Manager->SpawnNPC( banditId );

			// Now, change bandit's equipment 
			*/
		}
	}
}