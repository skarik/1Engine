
#include "EventBanditSpawnSmall.h"

#include "after/entities/character/npc/zoned/CZonedCharacterController.h"
#include "after/entities/world/CNpcSpawner.h"
#include "after/types/character/NPC_WorldState.h"
#include "after/interfaces/io/CZonedCharacterIO.h"
#include "after/entities/character/NPC/CNpcBase.h"

#include "core/math/random/Random.h"
#include "engine/state/CGameState.h"

using namespace GameEvents;

EventBanditSpawnSmall::EventBanditSpawnSmall ( void ) : CEventBase()
{
	for ( uint i = 0; i < 3; ++i )
	{
		NPC::npcid_t banditId = NPC::Manager->RequestNPC( NPC::npcid_UNIQUE );
		{
			NPC::characterFile_t characterFile;

			CRacialStats rstats;
			rstats.stats = new CharacterStats;
			rstats.SetDefaults();
			characterFile.rstats = &rstats;
			{
				// Set race
				//rstats.iRace = CRACE_ELF;
				switch ( Random.Next()%4 ) {
				case 0:
					rstats.iRace = CRACE_ELF; break;
					//rstats.iRace = CRACE_FLUXXOR; break;
				case 1:
					rstats.iRace = CRACE_ELF; break;
				case 2:
					rstats.iRace = CRACE_KITTEN; break;
				case 3:
					rstats.iRace = CRACE_HUMAN; break;
				}
				// Set gender
				rstats.iGender = eCharacterGender(Random.Next()%2);
				// Randomize hairstyle
				rstats.iHairstyle = Random.Next()%3;
				// Set companion colors
				rstats.RerollColors();
			
				// Set facial tattoo
				if ( Random.Next()%3 == 0 )
				{
					CRacialStats::tattoo_t tattoo;
					tattoo.color = Color( Random.Range(0.1f,0.5f),Random.Range(0.1f,0.5f),Random.Range(0.1f,0.5f),1.0f );
					tattoo.mirror = false;
					tattoo.type = TATT_CLANMARK;
					tattoo.pattern = "clan_skullblack";
					if ( Random.Next()%2 == 0 ) {
						tattoo.pattern = "clan_mask2";
					}
					tattoo.projection_angle = Random.Range( 150.0f, 180.0f );
					tattoo.projection_dir = Vector3d( 0,1,0 );
					tattoo.projection_pos = Vector3d( 0,-15,28 ) / 12.0f;
					tattoo.projection_scale = Vector3d(1,1,1) * Random.Range( 0.77f, 0.92f );
					if ( Random.Chance(0.5f) ) tattoo.projection_scale.x *= -1;
					rstats.tattooList.push_back( tattoo );
				}
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
				bool success = false;
				while ( !success ) {
					worldstate.worldPosition = NPC::Spawner->GetSpawnPosition( m_focus_player_actor->transform.position, success );//= pPlayerStats->vPlayerInitSpawnPoint;
				}
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



		// Assign main bandit id to check when should call event over
		mainBanditId = bandit->GetId();
	}
}


EventBanditSpawnSmall::~EventBanditSpawnSmall ( void )
{

}


void EventBanditSpawnSmall::Update ( void )
{
	if ( CGameState::Active()->GetBehavior( mainBanditId ) == NULL ) {
		DeleteObject( this );
	}
}