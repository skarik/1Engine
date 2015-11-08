
#include "CNpcSpawner.h"
#include "core/time/time.h"
#include "core/math/random/Random.h"
#include "after/terrain/Zones.h"

#include "after/terrain/VoxelTerrain.h"

#include "engine-common/entities/CPlayer.h"
#include "engine-common/network/playerlist.h"

// NPC Include list
#include "after/entities/character/npc/CCharTest.h"
#include "after/entities/character/npc/CNpcBase.h"

#include "after/entities/character/npc/zoned/fauna/FaunaDesertBird.h"
#include "after/entities/character/npc/zoned/fauna/FaunaKomodo.h"

#include "after/types/terrain/TerrainTypes.h"
#include "after/types/terrain/BiomeTypes.h"
#include "after/types/terrain/BlockType.h"

#include "renderer/camera/CCamera.h"

using namespace std;
using namespace NPC;

CNpcSpawner* NPC::Spawner = NULL;

CNpcSpawner::CNpcSpawner( CVoxelTerrain * targetTerra, CZonedCharacterController* targetController )
	: CGameBehavior(), pTerra( targetTerra ), pController( targetController )
{
	// Warning if invalid terrain or world
	if ( pTerra == NULL )
	{
		cout << "Somebody forgot to make the terrain before making a spawner! Tell them they're an idiot!" << endl;
		cout << "Also, if you're a girl, take your dick and thrust it into the back of their neck!" << endl;
	}
	if ( ActiveGameWorld == NULL )
	{
		cout << "Somebody forgot to make the world before making a spawner! Seriously, what kind of idiot does that?" << endl;
	}
	// Set up global pointer
	if ( Spawner == NULL ) {
		Spawner = this;
	}

	// Reset timers
	fSpawnUpdateTimer = 0;
	iSpawnUpdateFramecount = 0;

	// Get spawner state
	spawnerState = &(ActiveGameWorld->spawnerState);
}

CNpcSpawner::~CNpcSpawner ( void )
{
	// Set up global pointer
	if ( Spawner == this ) {
		Spawner = NULL;
	}
}


void CNpcSpawner::Update ( void )
{
	auto playerList = Network::GetPlayerActors();

	for ( uint i = 0; i < playerList.size(); ++i )
	{
		// Get player reference
		CActor* currentPlayer = playerList[i].actor;
		if ( !currentPlayer || currentPlayer->ActorType() != ACTOR_TYPE_PLAYER ) {
			continue;
		}

		// Work spawn timers
		fSpawnUpdateTimer += Time::smoothDeltaTime;
		if ( fSpawnUpdateTimer > 1.0f )
		{
			iSpawnUpdateFramecount += 1;
			if ( iSpawnUpdateFramecount > 3 )
			{
				// Update sample areas
				iCurrentBiome	= (Terrain::EBiomeType)Zones.GetTerrainBiomeAt( currentPlayer->transform.position );
				iCurrentTerrain	= (Terrain::ETerrainType)Zones.GetTerrainTypeAt ( currentPlayer->transform.position );
				CZonedCharacter::biomeType = iCurrentBiome;
				CZonedCharacter::terraType = iCurrentTerrain;

				// Run spawning algorithm
				SpawnNpcs( currentPlayer->transform.position );

				// Reset timers
				fSpawnUpdateTimer = 0;
				iSpawnUpdateFramecount = 0;
			}
		}
	}
}

struct spawnChoice_t {
	spawnChoice_t( const char* s, ftype w = 1.0f ) {
		name = s;
		weight = w;
	}
	string	name;
	ftype	weight;
};
void CNpcSpawner::SpawnNpcs ( const Vector3d n_sourcePlayerPosition )
{
	ftype	spawnsPerHour = 600.0f;
	ftype	spawnChance = Random.Range( 0.0f, 1.0f );
	ftype	spawnChoice = Random.Range( 0.0f, 1.0f );
	CZonedCharacter*		newNpc;
	vector<spawnChoice_t>		choiceList;

	// ===============================================
	// Generate spawn list
	//choiceList.push_back( spawnChoice_t( "CCharTest" ) );
	choiceList.push_back( spawnChoice_t( "CNpcBase" ) );
	switch ( iCurrentTerrain ) {
	case Terrain::TER_DESERT:
		choiceList.push_back( spawnChoice_t( "FaunaDesertBird" ) );
		break;
	case Terrain::TER_FLATLANDS:
	case Terrain::TER_HILLLANDS:
	case Terrain::TER_DEFAULT:
		choiceList.push_back( spawnChoice_t( "FaunaKomodo" ) );
		break;
	}

	// ===============================================
	// Choose From Spawn List
	int spawnChoiceIndex = -1;
	if ( spawnChance < spawnsPerHour * Time::deltaTime * 0.01667f ) {
		// Choose one from spawn list
		ftype maxWeight = 0;
		for ( uint i = 0; i < choiceList.size(); ++i ) {
			maxWeight += choiceList[i].weight;
		}
		spawnChoice *= maxWeight;
		for ( uint i = 0; i < choiceList.size(); ++i ) {
			if( spawnChoice < choiceList[i].weight ) {
				spawnChoiceIndex = i;
				break;
			}
			spawnChoice -= choiceList[i].weight;
		}
	}

	// ===============================================
	// Spawn when a valid object chosen
	if ( spawnChoiceIndex != -1 ) {
		string typeName = choiceList[spawnChoiceIndex].name;
		int currentCount = pController->GetCharacterCount( typeName );
		/*int currentCount;
		currentCount = pController->GetCharacterCount( typeName );
		if ( currentCount < 1 ) {
			//newNpc = pController->SpawnCharacter( typeName, currentPlayer->transform.position + Vector3d( 10,10,10 ) );
		}*/
		if ( typeName == "FaunaDesertBird" ) {
			if ( currentCount < 5 ) {
				newNpc = pController->SpawnFauna( typeName, n_sourcePlayerPosition + Vector3d( random_range(-64,64),random_range(-64,64),random_range(32,80) ) );
			}
		}
		/*else if ( typeName == "CCharTest" ) {
			if ( currentCount < 1 ) {
				newNpc = pController->SpawnCharacter( typeName, currentPlayer->transform.position + Vector3d( 10,10,10 ) );
			}
		}
		else if ( typeName == "CNpcBase" ) {
			if ( currentCount < 1 ) {
				newNpc = pController->SpawnCharacter( typeName, currentPlayer->transform.position + Vector3d( 10,10,10 ) );
			}
		}*/
		else if ( typeName == "FaunaKomodo" ) {
			if ( currentCount < 2 ) {
				//newNpc = pController->SpawnCharacter( typeName, currentPlayer->transform.position + Vector3d( Random.PointOnUnitCircle()*10,4 ) );
				/*bool check;
				Vector3d spawnpos;
				spawnpos = GetSpawnPosition( n_sourcePlayerPosition, check );
				if ( check ) {
					newNpc = pController->SpawnFauna( typeName, spawnpos );
				}*/
			}
		}
	}
}

Vector3d CNpcSpawner::GetSpawnPosition ( const Vector3d n_sourcePlayerPosition, bool& out_success )
{
	// Stack overflow guard
	/*if ( n_recurse > 20 ) {
		out_success = false;	// Mark as failed
		return Vector3d::zero;
	}

	Vector3d resultPos = currentPlayer->transform.position + Vector3d( Random.PointOnUnitCircle()*Random.Range(15,40),-4 );
	// check position for ground
	//ushort block;
	//= CVoxelTerrain::GetActive()->GetBlockAtPosition(resultPos).block;
	BlockInfo block;
	if ( CVoxelTerrain::GetActive()->GetBlockInfoAtPosition( resultPos, block ) )
	{
		short bi;//sexual lol
		do 
		{
			resultPos.z += 2;
			if ( CVoxelTerrain::GetActive()->GetBlockInfoAtPosition( resultPos, block ) ) {
				bi = block.block.block;
			}
			else {
				bi = Terrain::EB_NONE;
			}
		}
		while (( bi != Terrain::EB_NONE )&&( bi != Terrain::EB_WATerrain::TER ));
	}

	// Make sure the point is not in view
	if ( CCamera::activeCamera->SphereIsVisible( resultPos, 2.0f ) ) {
		// instead return a new attempt
		return GetSpawnPosition(out_success,n_recurse+1); // TODO stack overflow
	}
	// Mark as success if here
	out_success = true;
	// But we're here, so return the new place
	return resultPos;*/
	Vector3d resultPos;
	bool	good_spawnpoint	= false;
	int		check_count		= 0;

	while ( (!good_spawnpoint) && (check_count++ < 100) )
	{
		resultPos = n_sourcePlayerPosition + Vector3d( Random.PointOnUnitCircle()*Random.Range(15,60),-4 );
		// check position for ground
		//BlockInfo block;
		Terrain::terra_b block;
		//if ( CVoxelTerrain::GetActive()->GetBlockInfoAtPosition( resultPos, block ) )
		block = Zones.GetBlockAtPosition( resultPos );
		if ( Zones.IsActiveArea( resultPos ) )
		{
			short bi;//sexual lol
			do 
			{
				resultPos.z += 2;
				//if ( CVoxelTerrain::GetActive()->GetBlockInfoAtPosition( resultPos, block ) ) {
				if ( Zones.IsActiveArea( resultPos ) ) {
					block = Zones.GetBlockAtPosition( resultPos );
					bi = block.block;
				}
				else {
					bi = Terrain::EB_NONE;
				}
			}
			while (( bi != Terrain::EB_NONE )&&( bi != Terrain::EB_WATER ));
		}
		else
		{
			continue; // Position out of terrain range, continue
		}

		// Make sure the point is not in view
		if ( CCamera::activeCamera->SphereIsVisible( resultPos, 2.0f ) ) {
			continue; // if in view, continue
		}
		
		// Mark good spawnpoint if here
		good_spawnpoint = true;
	}

	if ( good_spawnpoint ) {
		out_success = true;
		return resultPos;
	}
	else {
		out_success = false;
		return Vector3d::zero;
	}
}