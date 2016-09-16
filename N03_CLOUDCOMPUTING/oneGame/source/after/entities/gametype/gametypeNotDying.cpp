
#include "gametypeNotDying.h"

#include "after/entities/character/CAfterPlayer.h"
#include "renderer/camera/CCamera.h"
//#include "unused/CVoxelTerrain.h"
//#include "unused/COctreeTerrain.h"
#include "renderer/object/screen/CScreenFade.h"
#include "after/entities/world/environment/DayAndNightCycle.h"
#include "after/entities/world/environment/CloudSphere.h"

#include "after/entities/menu/CTerrainLoadscreen.h"


#include "after/entities/character/npc/zoned/CZonedCharacterController.h"
#include "after/entities/world/CNpcSpawner.h"
#include "after/entities/world/CGameEventGenerator.h"

#include "after/entities/world/environment/CEnvironmentEffects.h"

#include "core/settings/CGameSettings.h"

#include "after/terrain/Zones.h"	// for utility

#include "core/math/noise/Perlin.h"

// Constructor
gametypeNotDying::gametypeNotDying ( CGameType* n_previous_gametype, const Vector3d& n_spawnpoint )
	: CGameType(n_previous_gametype)
{
	currentState = GSTATE_INITIALIZE;
	bSinglePlayer = true;
	//bHasSpawnPoint = false;

	player_spawnpoint = n_spawnpoint;

	//pWorldEffects = NULL;
}

// Destructor
gametypeNotDying::~gametypeNotDying ( void )
{
	if ( World::EventGenerator ) {
		World::EventGenerator->SaveState();
	}
	//worldState.Save();
	m_worldstate->Save(); // Isn't ever called.
}

// Update
void gametypeNotDying::Update ( void )
{
	switch ( currentState )
	{
	case GSTATE_INITIALIZE:
		GsInitialize();
		break;
	case GSTATE_LOADING:
		GsLoading();
		break;
	case GSTATE_SPAWNING:
		GsSpawning();
		break;
	case GSTATE_SPAWNFALL:
		GsSpawnFall();
		break;
	case GSTATE_GAMEPLAY:
		GsGameplay();
		break;
	}
}

// == Gametype States ==

// Initializing state
void gametypeNotDying::GsInitialize ( void )
{
	// Load the world state
	//worldState.Load();

	//// Create loading screen
	//pTerrainLoadscreen	= new CTerrainLoadscreen();
	
	//// Create default camera
	//pDefaultCamera = new CCamera();

	//// Create the player stats
	//pPlayerStats = new CPlayerStats();
	//pPlayerStats->SetDefaults();
	//pPlayerStats->LoadFromFile();
	//pPlayerStats->race_stats->SetLuaDefaults();

	//// Create NPC governor
	//pCharacterController = new NPC::CZonedCharacterController ();
	////pCharacterController->RemoveReference();

	//// Now go to loading state
	//currentState = GSTATE_LOADING;

	// Go to gameplay state
	//currentState = GSTATE_GAMEPLAY;

	// Create a player with the stats
	//CPlayer* newPlayer = new CPlayer;
	//newPlayer->RemoveReference();

	//Daycycle* succker = new Daycycle;
	//succker->RemoveReference();

	m_charactercontroller->active = true;
	m_characterspawner->active = true;
	m_worldeffects->active = true;

	// Create the player stats
	CPlayerStats* pPlayerStats = new CPlayerStats();
	pPlayerStats->SetDefaults();
	pPlayerStats->LoadFromFile();
	pPlayerStats->race_stats->SetLuaDefaults();
	pPlayerStats->vPlayerCalcSpawnPoint = player_spawnpoint;

	// Create the player
	CCharacter* newPlayer = new CAfterPlayer ( pPlayerStats );
	newPlayer->RemoveReference();

	// Now go to loading state
	currentState = GSTATE_GAMEPLAY;
}

// Loading terrain screen
void gametypeNotDying::GsLoading ( void )
{
	////static bool validStart = true;
	//// Check for the terrain to be done loading
	//CVoxelTerrain*	activeTerrain = CVoxelTerrain::GetActive();
	//if ((( activeTerrain )&&( activeTerrain->GetDoneLoading() ))||( CGameSettings::Active()->b_cl_FastStart ))
	//{
	//	// Now go to either the spawning or the spawnfall state
	//	if ( pPlayerStats->bHasPlayed == false )
	//	//if ( !validStart )
	//	{
	//		// If hasn't played, go to spawn fall
	//		currentState = GSTATE_SPAWNFALL;
	//	}
	//	else
	//	{
	//		// If has played, just go to spawn
	//		currentState = GSTATE_SPAWNING;
	//	}
	//}
	//else if ( activeTerrain )
	//{
	//	/*if ( activeTerrain->GetLoadingPercent() < 0.5f ) 
	//	{
	//		bHasSpawnPoint = false;
	//	}
	//	//if ( pPlayerStats->bHasPlayed == false ) // NEEDS TO BE PER WORLD?
	//	else */if ( bHasSpawnPoint == false )
	//	{
	//		Perlin noise ( 2,2.3f,1, CGameSettings::Active()->i_cl_DefaultSeed );
	//		// TODO: put this BEFORE terrain finished loading
	//		// Make spawnpoint
	//		//pPlayerStats->vPlayerInitSpawnPoint = Vector3d( 0,0,0 );
	//		// However, need to look around in the area to find the spawnpoint
	//		bool invalidSpawnpoint = (pPlayerStats->vPlayerInitSpawnPoint.magnitude() < 2.1f);
	//		/*if ( invalidSpawnpoint ) {
	//			validStart = false;
	//		}*/
	//		while ( invalidSpawnpoint )
	//		{
	//			uchar terrain = Zones.GetTerrainTypeAt( pPlayerStats->vPlayerInitSpawnPoint );
	//			uchar biome = Zones.GetTerrainBiomeAt( pPlayerStats->vPlayerInitSpawnPoint );
	//			if ( (terrain == TER_OCEAN) || (terrain == TER_OUTLANDS) || (terrain == TER_THE_EDGE) || (biome == BIO_S_TUNDRA) )
	//			{
	//				// Toss the spawnpoint around.
	//				//pPlayerStats->bHasPlayed = false;
	//				//pPlayerStats->vPlayerInitSpawnPoint += Vector3d( Random.PointOnUnitCircle()*30 );
	//				pPlayerStats->vPlayerInitSpawnPoint *= 0.9f;
	//				pPlayerStats->vPlayerInitSpawnPoint += Vector3d(
	//					noise.Unnormalize( noise.Get(pPlayerStats->vPlayerInitSpawnPoint.y*2.3f+1.2f,pPlayerStats->vPlayerInitSpawnPoint.x*3.2f+2.1f) ),
	//					noise.Unnormalize( noise.Get(pPlayerStats->vPlayerInitSpawnPoint.x*4.1f+2.3f,pPlayerStats->vPlayerInitSpawnPoint.y*1.4f+3.2f) ),
	//					0.0f ).normal() * 70.0f;
	//			}
	//			else {
	//				// If we're here, then the spawnpoint is sexy.
	//				invalidSpawnpoint = false;
	//			}
	//		}
	//		cout << "split" << endl;
	//		cout << "SPAWNPOINT: " << pPlayerStats->vPlayerInitSpawnPoint << endl;
	//		// Generate spawn position with the player stats
	//		pPlayerStats->MakeSpawnpoint();

	//		// Save the player stats
	//		pPlayerStats->SaveToFile();

	//		// Move player camera
	//		pDefaultCamera->transform.position = pPlayerStats->vPlayerInitSpawnPoint + Vector3d(0,0,32);

	//		// Now have spawn point
	//		bHasSpawnPoint = true;

	//		// Set terrain as active
	//		//activeTerrain->SetRootUpdate( true );
	//		// Set target loading area
	//		activeTerrain->SetRootTarget( pPlayerStats->vPlayerInitSpawnPoint );
	//	}
	//}

	//// If new state is not the loading state
	//if ( currentState != GSTATE_LOADING )
	//{
	//	// Destroy load screen
	//	delete pTerrainLoadscreen;
	//	pTerrainLoadscreen = NULL;
	//	
	//	// Create fade in
	//	CScreenFade* fadeIn = new CScreenFade ( true, 1.0f, 1.3f, Color( 0.0f,0,0 ) );
	//}

	currentState = GSTATE_SPAWNING;
}

// Spawn fall animation on the first play
#include "after/types/character/NPC_WorldState.h"
#include "after/interfaces/io/CZonedCharacterIO.h"
void gametypeNotDying::GsSpawnFall ( void )
{
	/*static ftype height = 240;
	static ftype speed = 20;
	
	// do falling effect
	speed += 13 * Time::deltaTime;

	pDefaultCamera->transform.position = pPlayerStats->vPlayerInitSpawnPoint + Vector3d(0,0,20+height);
	Vector3d startAngle = pDefaultCamera->transform.rotation.getEulerAngles();
	startAngle.z += Time::deltaTime * speed * 0.04f;
	startAngle.y = -90 + Random.Range(-1,1)*0.02f*speed;
	//startAngle.x = speed*0.3f + Random.Range( -1,1 )*0.1f*speed;
	pDefaultCamera->transform.rotation = startAngle;

	height -= speed * Time::deltaTime;

	if ( height <= 0 ) {
		// create companion (after this, the NPC manager will be taking care of it)
		if ( !NPC::Manager->CheckNPC( 1024 ) ) // Only create NPC if not already created in this realm
		{
			uint64_t companion_id = NPC::Manager->RequestNPC( NPC::npcid_SETPIECE, 1024 );
			{
				NPC::characterFile_t characterFile;

				CRacialStats rstats;
				rstats.stats = new CharacterStats;
				rstats.SetDefaults();
				characterFile.rstats = &rstats;
				{
					// Set race
					rstats.iRace = pPlayerStats->companion_race;
					if ( rstats.iRace == CRACE_RANDOM ) {
						rstats.iRace = eCharacterRace(Random.Next()%6+1);
					}
					else if ( rstats.iRace == CRACE_SAME ) {
						rstats.iRace = pPlayerStats->race_stats->iRace;
					}
					// Set gender
					rstats.iGender = pPlayerStats->companion_gender;
					if ( rstats.iGender == CGEND_OPPOSITE ) {
						if ( pPlayerStats->race_stats->iGender == CGEND_MALE ) {
							rstats.iGender = CGEND_FEMALE;
						}
						else {
							rstats.iGender = CGEND_MALE;
						}
					}
					else if ( rstats.iGender == CGEND_SAME ) {
						rstats.iGender = pPlayerStats->race_stats->iGender;
					}
					else if ( rstats.iGender == CGEND_RANDOM ) {
						rstats.iGender = eCharacterGender(Random.Next()%2);
					}
					// Set companion colors
					rstats.RerollColors();
				}

				NPC::sWorldState worldstate;
				characterFile.worldstate = &worldstate;
				worldstate.mFocus = NPC::AIFOCUS_Companion;
				worldstate.travelDirection = Vector3d( 1,0,0 );
				worldstate.worldPosition = pPlayerStats->vPlayerInitSpawnPoint;
				worldstate.partyHost = companion_id;

				NPC::sOpinions opinions;
				characterFile.opinions = &opinions;

				NPC::sPreferences prefs;
				characterFile.prefs = &prefs;

				// Save generated stats
				NPC::CZonedCharacterIO io;
				io.CreateCharacterFile( companion_id, characterFile );
				// Clean up
				delete rstats.stats;
			}
			NPC::Manager->SpawnNPC( companion_id );
		}

		// Do spawning
		currentState = GSTATE_SPAWNING;

		// Create fade in
		CScreenFade* fadeIn = new CScreenFade ( true, 1.0f, 1.3f, Color( 0.0f,0,0 ) );
	}

	// Create environment manager
	if ( !pWorldEffects ) {
		pWorldEffects = new CEnvironmentEffects;
		pWorldEffects->RemoveReference();
	}

	// For now, just do regular spawning
	//GsSpawning(); // TODO here
	*/
}

// Spawning the player
void gametypeNotDying::GsSpawning ( void )
{
	//// Destroy the camera
	//delete pDefaultCamera;
	//pDefaultCamera = NULL;

	//// Create a player with the stats
	//CPlayer* newPlayer = new CMccPlayer ( pPlayerStats );
	//newPlayer->RemoveReference();

	//// Move terrain mode to follow camera
	//CVoxelTerrain*	activeTerrain = CVoxelTerrain::GetActive();
	//if ( activeTerrain ) // Active mode
	//	activeTerrain->SetRootUpdate( true );

	//// Create daycycle
	////pWorldDaycycle = new Daycycle();
	//// Create environment manager
	//if ( !pWorldEffects ) {
	//	pWorldEffects = new CEnvironmentEffects;
	//	pWorldEffects->RemoveReference();
	//}

	//// Create NPC spawner
	//pNpcSpawner = new NPC::CNpcSpawner( CVoxelTerrain::GetActive(), pCharacterController );
	//pNpcSpawner->RemoveReference();
	//// Ready up system
	//pCharacterController->ReadyUp();
	//// Create game event system
	//new World::CGameEventGenerator();
	//World::EventGenerator->RemoveReference();

	//// Go to gameplay state
	//currentState = GSTATE_GAMEPLAY;
}

// Gameplay state
void gametypeNotDying::GsGameplay ( void )
{
	m_worldstate->fTimeSpeed = 60;
	m_worldstate->Tick();

	m_worldeffects->pWorldDaycycle->cAmbientOffset = m_worldstate->cBaseAmbient;

	//pWorldDaycycle->SetTimeOfDay( worldState.fCurrentTime );
	m_worldeffects->pWorldDaycycle->SetTimeOfDay( (ftype)m_worldstate->fCurrentTime );
	m_worldeffects->pWorldMooncycle->SetTimeOfCycle( (ftype)m_worldstate->fCurrentTime );
	m_worldeffects->pWorldCloudsphere->SetTimeOfDay( (ftype)m_worldstate->fCurrentTime );
	m_worldeffects->pWorldDaycycle->SetSpaceEffect( 0 ); // For now, turn off space

	// If no event system, make one
	if ( World::EventGenerator == NULL ) {
		// Create game event system
		new World::CGameEventGenerator();
		World::EventGenerator->RemoveReference();
	}

	/// Have the terrain follow player as well
	/*static int ticker = 0;
	if ( COctreeTerrain::GetActive() && CCharacter::GetActivePlayer() && (((++ticker)%10)==0) )
	{
		COctreeTerrain::GetActive()->SetStateFollowTarget( CCharacter::GetActivePlayer()->transform.position );
	}*/
}