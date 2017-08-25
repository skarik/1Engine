
#include "CGameEventGenerator.h"
#include "engine-common/entities/CPlayer.h"
#include "core/time/time.h"
#include "core/settings/CGameSettings.h"
#include "core/system/io/CBinaryFile.h"
#include "core/math/random/Random.h"
#include "after/terrain/Zones.h"
#include "engine/state/CGameState.h"
#include "core/debug/CDebugConsole.h"
#include "engine-common/network/playerlist.h"

namespace World
{
	CGameEventGenerator*	EventGenerator = NULL;
}

World::CGameEventGenerator::CGameEventGenerator ( void )
	: CGameBehavior()
{
	World::EventGenerator = this;
	m_target_player = NULL;
	m_travel_distance = 0;
	m_event_timer = 0;
	m_start_position = Vector3d::zero;

	RegisterGameEvents();

	LoadState();
}

World::CGameEventGenerator::~CGameEventGenerator ( void )
{
	if ( World::EventGenerator == this ) {
		SaveState();
		World::EventGenerator = NULL;
	}
}

void World::CGameEventGenerator::AddGameEvent ( const string& eventName, CGameBehavior*(*func)(void) )
{
	gameEventInstantiator newInst;
	newInst.func = func;
	m_instance_list[eventName] = newInst;
}
void World::CGameEventGenerator::AddGameEvent ( const string& eventName, const string& luaName )
{
	gameEventInstantiator newInst;
	newInst.func = NULL;
	newInst.luaname = luaName;
	m_instance_list[eventName] = newInst;
}
CGameBehavior* World::gameEventInstantiator::operator()(void)
{
	if ( func != NULL ) {
		return func();
	}
	else {
		return NULL;
	}
}
bool World::gameEventInstantiator::valid ( void )
{
	if ( func != NULL ) {
		return true;
	}
	else {
		if ( !luaname.empty() ) {
			return true;
		}
	}
	return false;
}

// Adds an event to the event entry list
void World::CGameEventGenerator::AddEvent ( const string& eventName, const gameEventEntry& newEntry )
{
	if ( m_instance_list.find( eventName ) != m_instance_list.end() ) {
		m_event_list[eventName] = newEntry;
	}
	else {
		Debug::Console->PrintError( "Could not find event named " + eventName + " in registered event list!" );
	}
}
// Removes an event from the event entry list
void World::CGameEventGenerator::RemoveEvent ( const string& eventName )
{
	auto findResult = m_event_list.find( eventName );
	if ( findResult != m_event_list.end() ) {
		m_event_list.erase( findResult );
	}
	else {
		Debug::Console->PrintWarning( "Could not find event named " + eventName + " in active event list" );
	}
}

void World::CGameEventGenerator::SaveState ( void )
{
	string eventGeneratorFile = CGameSettings::Active()->GetTerrainSaveDir() + ".events";
	CBinaryFile bfile;
	if ( bfile.Open( eventGeneratorFile.c_str(), bfile.IO_WRITE ) )
	{
		bfile.WriteUShort( m_event_list.size() );
		for ( auto eventEntry = m_event_list.begin(); eventEntry != m_event_list.end(); ++eventEntry )
		{
			bfile.WriteString( eventEntry->first );
			bfile.WriteData( (char*)&(eventEntry->second), sizeof(gameEventEntry) );
		}
	}
}
void World::CGameEventGenerator::LoadState ( void )
{
	string eventGeneratorFile = CGameSettings::Active()->GetTerrainSaveDir() + ".events";
	CBinaryFile bfile;
	string			newName;
	gameEventEntry	newEntry;
	if ( bfile.Open( eventGeneratorFile.c_str(), bfile.IO_READ ) )
	{
		m_event_list.clear();
		ushort readCount = bfile.ReadUShort();
		for ( ushort i = 0; i < readCount; ++i )
		{
			newName = bfile.ReadString();
			bfile.ReadData( (char*)&newEntry, sizeof(gameEventEntry) );
			newEntry.active = 0; // override active
			AddEvent( newName, newEntry );
		}
	}
	else
	{	// Add default events to the list
		{	// Null event
			newName = "";
			newEntry.baseChance = 1.0f;
			AddEvent( newName, newEntry );
		}
		{	// Bandit event
			newName = "bandits small";
			newEntry.baseChance = 0.5f;
			AddEvent( newName, newEntry );
			// Remove bandits for now
		}
		{	// Komodoes event
			/*newName = "fauna komodo pack";
			newEntry.baseChance = 0.3f;
			AddEvent( newName, newEntry );*/
		}
		{	// Desert storm
			newName = "desert storm 1";
			newEntry.baseChance = 0.5f;
			AddEvent( newName, newEntry );
		}
		{	// Desert automatons
			newName = "desert automatons 1";
			newEntry.baseChance = 0.4f;
			AddEvent( newName, newEntry );
		}
	}
}
void World::CGameEventGenerator::Update ( void )
{
	// Get a player to use for spawning events
	if ( m_target_player == NULL )
	{
		m_target_player = Network::GetPlayerActors()[0].actor;//CPlayer::GetActivePlayer();
		if ( m_target_player )
		{
			m_start_position = m_target_player->transform.position;
		}
		return;
	}
	if ( !Zones.IsActiveArea( m_target_player->transform.position ) )
	{
		return;
	}
	
	// Go through the list and look up the id's when active
	for ( auto curEvent = m_event_list.begin(); curEvent != m_event_list.end(); ++curEvent )
	{
		if ( curEvent->second.active ) {
			if ( CGameState::Active()->GetBehavior( curEvent->second.target ) == NULL ) // guaranteed to be NULL the frame after destruction
			{
				curEvent->second.active = 0; // event no longer active, can create again
			}
		}
	}

	// Count distance and time for events
	m_event_timer += Time::deltaTime;
	ftype sqrTravelDistance = ( m_target_player->transform.position - m_start_position ).sqrMagnitude();
	if ( sqrTravelDistance > 2500.0f )
	{
		m_start_position = m_target_player->transform.position;
		m_travel_distance += sqrt(sqrTravelDistance);
	}

	// Every 2 minutes or 300 feet, create an event
	bool makeEvent = false;
	//if ( m_event_timer > 10 ) {
	if ( m_event_timer > 120 ) {
		makeEvent = true;
		m_event_timer = 0;
	}
	if ( m_travel_distance > 300 ) {
		makeEvent = true;
		m_travel_distance = 0;
	}

	if ( makeEvent )
	{
		// Loop through the current event entries to get total chance
		ftype totalChance = 0;
		for ( auto curEvent = m_event_list.begin(); curEvent != m_event_list.end(); ++curEvent )
		{
			if ( !curEvent->second.active ) {
				totalChance += curEvent->second.baseChance;
			}
		}

		// Chose one of the entries to create
		ftype randomChoice = Random.Range( 0, totalChance );
		auto chosenEvent = m_event_list.begin();
		totalChance -= chosenEvent->second.baseChance;
		while ( totalChance > 0 ) {
			chosenEvent++;
			if ( chosenEvent == m_event_list.end() ) {
				throw std::out_of_range( "Random sampler hit end" );
			}
			if ( !chosenEvent->second.active ) {
				totalChance -= chosenEvent->second.baseChance;
			}
		}

		// Now, create event
		std::cout << "New event \"" << chosenEvent->first << "\"" << std::endl;
		if ( m_instance_list[chosenEvent->first].valid() )
		{
			CGameBehavior* newEvent = m_instance_list[chosenEvent->first]();
			chosenEvent->second.active = 1;
			chosenEvent->second.target = newEvent->GetId();
		}

	}
}