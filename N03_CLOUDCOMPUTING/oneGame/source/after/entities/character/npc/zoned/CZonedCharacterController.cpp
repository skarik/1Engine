#include "CZonedCharacterController.h"
#include "CZonedCharacter.h"

//#include "unused/CVoxelTerrain.h"

#include "core/settings/CGameSettings.h"
#include "core/system/io/CBinaryFile.h"

#include "after/interfaces/io/CZonedCharacterIO.h"
#include "after/terrain/Zones.h"
#include "after/entities/character/npc/CNpcBase.h"
#include "after/states/CRacialStats.h"

#include "after/terrain/VoxelTerrain.h"
#include "after/terrain/data/Node.h"
#include "after/terrain/data/GameState.h"
//#include "after/terrain/generation/regions/CRegionGenerator.h"
#include "after/types/world/Regions.h"
#include "after/states/world/ProvinceManager.h"

using std::cout;
using std::endl;

using NPC::CZonedCharacterController;
using NPC::CZonedCharacter;

CZonedCharacterController* NPC::Manager = NULL;

CZonedCharacterController::BaseFactory::map_type* CZonedCharacterController::BaseFactory::map;

CZonedCharacterController::CZonedCharacterController ( void )
	: CGameBehavior ()
{
	Manager = this;

	// Setup vars
	{
		updateTimer = 0;
		npcsimTimer = 0;
		systemReady = false;
	}
}

void CZonedCharacterController::ReadyUp ( void )
{
	// Open state file
	{
		CBinaryFile		statefile;
		arstring<256>	s_statefile_fn;

		sprintf( s_statefile_fn, "%s/npc_freelist", CGameSettings::Active()->MakeRealmSaveDirectory().c_str() );
		statefile.Open( s_statefile_fn, CBinaryFile::IO_READ );
		// Check if state file exists
		if ( !statefile.IsOpen() )
		{
			// Doesn't exist, set default values and write a new value
			nextFreeNPC = 2048;
			freeIDcount = 0;
			statefile.Open( s_statefile_fn, CBinaryFile::IO_WRITE );
			if ( statefile.IsOpen() )
			{
				statefile.WriteUInt64( nextFreeNPC );
				statefile.WriteUInt64( freeIDcount );
			}
			else
			{
				throw Core::NullReferenceException();
			}
		}
		else
		{
			// Load state file data
			nextFreeNPC = statefile.ReadUInt64();
			freeIDcount = statefile.ReadUInt64();
		}
	}

	systemReady = true; 
}

CZonedCharacterController::~CZonedCharacterController ( void )
{
	Manager = NULL;
}

void CZonedCharacterController::AddCharacter ( NPC::zcc_characterinfo_t &ncharatoadd )
{
	ncharatoadd.start_li_position = Zones.PositionToRV( ncharatoadd.pointer->transform.position );
	characterList.push_back( ncharatoadd );
}
void CZonedCharacterController::RemoveCharacter ( CZonedCharacter * ncharactoremove )
{
	for ( auto it = characterList.begin(); it != characterList.end(); ++it ) {
		if ( it->pointer == ncharactoremove ) {
			characterList.erase( it );
			return;
		}
	}
	cout << "WARNING!!! :: Unable to remove character " << ncharactoremove->ZCC_GetCharacterId() << endl;
}
//	SaveCharacterOnUnload()
// Called when character goes out of range of the game state.
void CZonedCharacterController::SaveCharacterOnUnload ( CZonedCharacter* ncharatosave )
{
	CZonedCharacterIO io;
	RangeVector start_li_position;

	// Set world state position
	if ( ncharatosave->characterFile.worldstate ) {
		ncharatosave->characterFile.worldstate->worldPosition = ncharatosave->transform.position;
	}

	// First save the character
	io.SaveCharacterFile( ncharatosave->characterId, ncharatosave->characterFile );

	// Unload the character from the previous position
	for ( auto it = characterList.begin(); it != characterList.end(); ++it ) {
		if ( it->pointer == ncharatosave ) {
			start_li_position = it->start_li_position;
		}
	}
	io.SectorRemoveNPC( start_li_position, ncharatosave->characterId );
	// Now save it to the range area index
	RangeVector targetArea = Zones.PositionToRV( ncharatosave->transform.position );
	io.SectorAddNPC( targetArea, ncharatosave->characterId );
}
//	SaveCharacterLoad()
// Called when character is created.
void CZonedCharacterController::SaveCharacterLoad ( CZonedCharacter* ncharatosave )
{
	CZonedCharacterIO io;
	RangeVector start_li_position;

	// Set world state position
	if ( ncharatosave->characterFile.worldstate ) {
		ncharatosave->characterFile.worldstate->worldPosition = ncharatosave->transform.position;
	}

	// First save the character
	io.SaveCharacterFile( ncharatosave->characterId, ncharatosave->characterFile );

	// Unload the character from the previous position
	for ( auto it = characterList.begin(); it != characterList.end(); ++it ) {
		if ( it->pointer == ncharatosave ) {
			start_li_position = it->start_li_position;
		}
	}
	//cout << " from (" << start_li_position.x << ", " << start_li_position.y << ", " << start_li_position.z << ")" << endl;
	io.SectorRemoveNPC( start_li_position, ncharatosave->characterId );
	// Now save it to the range area index
	RangeVector targetArea = Zones.PositionToRV( ncharatosave->transform.position );
	//cout << ncharatosave->transform.position << " to (" << targetArea.x << ", " << targetArea.y << ", " << targetArea.z << ")" << endl;
	io.SectorAddNPC( targetArea, ncharatosave->characterId );
}

void CZonedCharacterController::Update ( void )
{
	if ( !(systemReady && CVoxelTerrain::GetActive()) ) {
		return;
	}

	// Every second, loop through the terrain and look for non-NPC sectors.
	updateTimer += Time::deltaTime;
	if ( updateTimer > 1.0f )
	{
		updateTimer -= 1.0f;

		// these sort of gamestates should be separate from the terrain. Terrain should maintain a list of areas that get a forced load/unload on them.
		//std::vector<Terrain::Node*>& terraList = CVoxelTerrain::GetActive()->GetGamestateListCopy();
		std::vector<Terrain::AreaGameState> terraList;
		CVoxelTerrain::GetActive()->GetAreaGamestateListCopy( terraList );
		for ( uint i = 0; i < terraList.size(); ++i ) 
		{
			if ( terraList[i].has_block_data && !terraList[i].npcs_loaded ) {
				terraList[i].npcs_loaded = true;
				// Load in NPC file
				LoadSector( terraList[i].world_vector );
			}
		}
		//CVoxelTerrain::GetActive()->ReleaseLockTerraList();
	}

	// And yes, for unloading, we WILL be using the unload callback
}

void CZonedCharacterController::LateUpdate ( void )
{
	if ( !(systemReady && CVoxelTerrain::GetActive()) ) {
		return;
	}

	// Every ten seconds, simulate NPC behavior.
	npcsimTimer += Time::deltaTime;
	if ( npcsimTimer > 10.0f )
	{
		npcsimTimer -= 10.0f;

		CZonedCharacterIO IO;
		RangeVector center = CVoxelTerrain::GetActive()->GetCenterSector(); //get terrain center
		std::vector<uint64_t> NPC_list;

		// Do sim on external NPCS
		// Need to load in all NPCs in the area.
		RangeVector li_target_position;
		for ( int i = -10; i < 10; ++i ) {
			for ( int j = -10; j < 10; ++j ) {
				for ( int k = -10; k < 10; ++k ) {

					li_target_position.x = center.x + i;
					li_target_position.y = center.y + i;
					li_target_position.z = center.z + i;

					if ( std::find( externalCharacterSectors.begin(), externalCharacterSectors.end(), li_target_position ) == externalCharacterSectors.end() )
					{
						// load it (aka, use the IO, and add info to externalCharacterList)
						IO.SectorGetNPCList( li_target_position, NPC_list );
					}
				}
			}
		}
		// First check if NPCs in list are valid (in range)
		for ( auto i_npc = externalCharacterList.begin(); i_npc != externalCharacterList.end(); )
		{
			if (!(abs(i_npc->li_position.x) < center.x + 10) || !(abs(i_npc->li_position.y) < center.y + 10) || !(abs(i_npc->li_position.z) < center.z + 10))
			{
				//i_npc->id = -1; // Assuming you want to delete this, modified the loop to do this
				i_npc = externalCharacterList.erase( i_npc ); // erase returns next step in loop
			}
			else {
				i_npc++; // Go to next step in loop
			}
		}
		// Now iterate through these external NPCs and simulate their behavior
		characterFile_t charinfo;
		charinfo.opinions = NULL;
		charinfo.prefs = NULL;
		charinfo.rstats = NULL;
		sWorldState mworldState;
		charinfo.worldstate = &mworldState;
		ftype limit = 30;
		
		for ( auto i_npc = externalCharacterList.begin(); i_npc != externalCharacterList.end(); ++i_npc )
		{	
			// load in NPC data (using i_npc->id)
			
			IO.LoadCharacterFile( i_npc->id, charinfo );
				
			// Do simulation
			charinfo.worldstate->travelDirection.x += (Random.Next() % 20) * (-1 * (Random.Next() % 2));
			charinfo.worldstate->travelDirection.y += (Random.Next() % 20) * (-1 * (Random.Next() % 2));

			if ( charinfo.worldstate->travelDirection.magnitude() > limit) {
				charinfo.worldstate->travelDirection = charinfo.worldstate->travelDirection.normal() * limit;
			}
			
			i_npc->position.x += charinfo.worldstate->travelDirection.x;
			i_npc->position.y += charinfo.worldstate->travelDirection.y;
			//i_npc->position.z +=;

			// Check the LI position (you can compare new w/ old using the stored value in the i_npc)
			RangeVector moved = Zones.PositionToRV( i_npc->position ) ; //converts world coordinates to LI coordinates (most functions here are documented)
			if (moved != i_npc->li_position)
			{
				// if changed, use SectorAddNPC and SectorRemoveNPC
				IO.SectorRemoveNPC ( i_npc->li_position, i_npc->id);
				IO.SectorAddNPC (moved, i_npc->id);
				//Update li_position
				i_npc->li_position.x = moved.x;
				i_npc->li_position.y = moved.y;
				i_npc->li_position.z = moved.z;
			}
		
			// then save back the char info
			IO.SaveCharacterFile (i_npc->id, charinfo);
		}
	}
}
void CZonedCharacterController::PostUpdate ( void )
{
	
}

// Loads in a sector for loading
void CZonedCharacterController::LoadSector ( const RangeVector& sector_index )
{
	/*arstring<256> s_sector_fn;	// Get sector file
	sprintf( s_sector_fn, "%s/chars/%d_%d_%d", CGameSettings::Active()->MakeRealmSaveDirectory().c_str(), sector_index.x, sector_index.y, sector_index.z );

	CBinaryFile sector;
	sector.Open( s_sector_fn, CBinaryFile::IO_READ );
	if ( sector.IsOpen() ) {
		// Read in number of NPCs
		uint64_t numNPCs = sector.ReadUInt64();
		// Load in NPCs and spawn
		for ( uint64_t i = 0; i < numNPCs; ++i ) {
			SpawnNPC( sector.ReadUInt64() );
		}
	}*/

	std::vector<uint64_t> npclist;
	CZonedCharacterIO io;

	if ( io.SectorGetNPCList( sector_index, npclist ) != -1 ) {

		if ( npclist.size() > 0 ) {
			cout << "found " << npclist.size() << " npcs in sector (" << sector_index.x << "," << sector_index.y << "," << sector_index.z << ")" << endl;
		}
		for ( uint i = 0; i < npclist.size(); ++i ) {
			SpawnNPC( npclist[i] );
		}

	}
	// File closed automatically
}


int CZonedCharacterController::GetCharacterCount ( const char* charstring )
{
	int count = 0;
	for ( auto it = characterList.begin(); it != characterList.end(); ++it )
	{
		if ( it->name == charstring )
			++count;
	}
	return count;
}


//	GetCharactersInParty()
// Populates the list with all the NPCs in the given party
void		CZonedCharacterController::GetCharactersInParty ( std::vector<uint64_t>& oid_list, uint64_t ntarget_party )
{
	oid_list.clear();
	for ( auto it = characterList.begin(); it != characterList.end(); ++it ) {
		if ( it->id >= 1024 ) {
			if ( ((CNpcBase*)(it->pointer))->GetPartyHost() == ntarget_party ) {
				oid_list.push_back( it->pointer->characterId );
			}
		}
	}
}

//	CheckNPC()
// Returns true if an NPC of the type already exists
bool				CZonedCharacterController::CheckNPC ( uint64_t ntarget_id )
{
	if ( ntarget_id < 1024 && ntarget_id != 0 ) {
		return false;
	}
	else {
		CZonedCharacter* newNPC = GetNPC( ntarget_id );
		if ( newNPC != NULL ) {
			return true;
		}

		// Check that the NPC info exists.
		CZonedCharacterIO io;
		// Read in spawn position at the same time as checking
		characterFile_t nextFile;
		sWorldState worldstate;
		nextFile.worldstate = &worldstate;

		if ( io.LoadCharacterFile( ntarget_id, nextFile ) == -1 ) {
			return false; // No file, return false
		}
		// File found, return true
		return true;
	}
}

//  RequestNPC()
// Request an NPC id to use. Marks NPC id as used. (Only unused IDs are saved)
uint64_t			CZonedCharacterController::RequestNPC ( NPC::eNPC_ID_TYPE nid_type, uint64_t ntarget_id )
{
	if ( nid_type == npcid_FAUNA ) {
		// Loop through current NPCs to find first unused fauna
		bool validated = false;
		while ( !validated )
		{
			// Increment ID to use
			nextFaunaNPC = (nextFaunaNPC+1)%1024; // Initial value doesn't matter
			// Make sure the id is not in use
			validated = true;
			if ( nextFaunaNPC == 0 ) {
				continue; // Cannot use fauna ID zero. Usually is reserved for system.
			}
			for ( auto it = characterList.begin(); it != characterList.end(); ++it ) {
				if ( nextFaunaNPC == it->id ) { 
					validated = false;
					it = characterList.end();
				}
			}
		}
		// Return that fauna ID
		return nextFaunaNPC;
	}
	else if ( nid_type == npcid_SETPIECE ) {
		// Create entry for character with given id, but check for an id between 1024 and 2047
		if ( ntarget_id < 1024 || ntarget_id > 2047 ) {
			return 0;
		}
		else {
			return ntarget_id;
		}
	}
	else if ( nid_type == npcid_UNIQUE ) {
		// Grab an open entry
		if ( freeIDcount == 0 ) {
			nextFreeNPC += 1; // Increment ID count
			return nextFreeNPC-1;
		}
		else {
			// Open file and grab last entry.
			freeIDcount -= 1;
			CBinaryFile		statefile;
			arstring<256>	s_statefile_fn;

			sprintf( s_statefile_fn, "%s/npc_freelist", CGameSettings::Active()->MakeRealmSaveDirectory().c_str() );
			statefile.Open( s_statefile_fn, CBinaryFile::IO_READ|CBinaryFile::IO_WRITE );

			fseek( statefile.GetFILE(), 8, SEEK_SET );
			statefile.WriteUInt64( freeIDcount );
			fseek( statefile.GetFILE(), (long)(16+freeIDcount*8), SEEK_SET );

			return statefile.ReadUInt64();
		}
	}
	return 0;
}
//  SpawnNPC()
// Using the given ID, spawns an NPC. If the NPC file does not exist, will generate the id.
CZonedCharacter*	CZonedCharacterController::SpawnNPC ( uint64_t nid )
{
	// Only spawn ID's above 2047
	//if ( nid < 2048 ) {
	if ( nid < 1024 && nid != 0 ) {
		//cout << "You can not spawn an NPC with fauna or setpiece ID." << endl;
		cout << "You can not spawn an NPC with fauna ID. (" << nid << ")" << endl;
	}
	else {
		// Provide warning message for ID zero
		if ( nid == 0 ) {
			Debug::Console->PrintWarning( "NPC System spawning ID 0 - 0 is reserved for system\n" );
		}

		// Check that NPC isn't already loaded.
		CZonedCharacter* newNPC = GetNPC( nid );
		if ( newNPC != NULL ) {
			return newNPC;
		}

		// Check that the NPC info exists.
		CZonedCharacterIO io;

		// Read in spawn position at the same time as checking
		characterFile_t nextFile;
		sWorldState worldstate;
		nextFile.worldstate = &worldstate;

		if ( io.LoadCharacterFile( nid, nextFile ) == -1 ) {
			if ( nid < 2048 ) { 
				cout << "You can not spawn a setpiece ID without previously setting its properties." << endl;
				return NULL;
			}
			// If it doesn't exist, send a request to the region manager for a new NPC in the town.
		}
		
		// Create the NPC
		newNPC = SpawnCharacter( "CNpcBase", worldstate.worldPosition, nid, Quaternion::CreateRotationTo( Vector3d::forward, worldstate.travelDirection.normal() ) );
		// Load in data to the NPC
		io.LoadCharacterFile( nid, newNPC->characterFile );
		// Call post load
		newNPC->PostLoad();

		// Save the NPC data
		SaveCharacterLoad( newNPC );
		
		// Return freshly created NPC
		return newNPC;
	}

	return NULL;
}
//  FreeNPCID()
// Frees the given ID. This deletes the NPC off the disk and removes shit and yeah!
void				CZonedCharacterController::FreeNPCID ( uint64_t nid )
{
	if ( nextFreeNPC == nid+1 ) {
		nextFreeNPC -= 1;
	}
	else {
		// Add it to the free list
		CBinaryFile		statefile;
		arstring<256>	s_statefile_fn;

		sprintf( s_statefile_fn, "%s/npc_freelist", CGameSettings::Active()->MakeRealmSaveDirectory().c_str() );
		statefile.Open( s_statefile_fn, CBinaryFile::IO_READ|CBinaryFile::IO_WRITE );
		// Update count of free ID's
		fseek( statefile.GetFILE(), 8, SEEK_SET );
		freeIDcount += 1;
		statefile.WriteUInt64( freeIDcount );
		// Write new ID to end of the list
		fseek( statefile.GetFILE(), (long)(8+8*freeIDcount), SEEK_SET );
		statefile.WriteUInt64( nid );
	}
	// Delete NPC file
	arstring<256> s_npcfile_fn;
	sprintf( s_npcfile_fn, "%s/chars/%lld.entry", CGameSettings::Active()->MakeRealmSaveDirectory().c_str(), nid );
	remove( s_npcfile_fn );
}
//  GetNPCList()
// Returns a list of NPC ids that are currently spawned.
void				CZonedCharacterController::GetNPCList ( std::vector<uint64_t>& oid_list )
{
	oid_list.clear();
	for ( auto it = characterList.begin(); it != characterList.end(); ++it ) {
		oid_list.push_back( it->pointer->characterId );
	}
}
//  GetNPC()
// Returns a character pointer to the character that matches with the ID. ID's 1024 and above are unique.
CZonedCharacter*	CZonedCharacterController::GetNPC ( uint64_t nid )
{
	for ( auto it = characterList.begin(); it != characterList.end(); ++it ) {
		if ( it->id == nid ) {
			return it->pointer;
		}
	}
	return NULL;
}

// == SPAWNERS == 
CZonedCharacter* CZonedCharacterController::SpawnCharacter ( const char* s , const Vector3d & inPosition, const uint64_t & inID, const Rotator & inRotation )
{
	cout << "Attempting to spawn " << inID << ", type (character): " << s << endl;
	return BaseFactory::createInstance( s, inPosition, inID, inRotation );
}
CZonedCharacter* CZonedCharacterController::SpawnFauna ( const char* s , const Vector3d & inPosition, const Rotator & inRotation )
{
	cout << "Attempting to spawn type (fauna): " << s;
	uint64_t nextId = RequestNPC( npcid_FAUNA );
	cout << " with fauna_id " << nextId;
	CZonedCharacter* newFauna = BaseFactory::createInstance( s, inPosition, nextId, inRotation );
	cout << endl;
	return newFauna;
}


// == Generation ==
//#include "unused/CRegionManager.h"
//  GenerateNPC
// Generates an NPC given the NPC ID and the region to associate the NPC's start area with.
void CZonedCharacterController::GenerateNPC ( uint64_t nid, const rangeint& x, const rangeint& y )
{
	GenerateNPC( nid, World::ProvinceManager->GetRegion(x,y) );
}
void CZonedCharacterController::GenerateNPC ( uint64_t nid, uint32_t nregion )
{
	CZonedCharacterIO IO;

	// Get the region properties
	World::province_properties_t provinceProperties;
	World::ProvinceManager->GetProvinceInfo( nregion, &provinceProperties );

	// Create the values
	characterFile_t charinfo;

	sOpinions opinion;
	{
		opinion.animosity = 0;
		opinion.loyalty = 0;
		opinion.opinion = 0;
	}
	charinfo.opinions = &opinion;

	sPreferences prefs;
	{
		prefs.friendly = true;
		prefs.judgemental = false;
		prefs.requiresGlasses = false;
	}
	charinfo.prefs = &prefs;

	sWorldState worldstate;
	{
		worldstate.mFocus	= AIFOCUS_Wanderer;
	}
	charinfo.worldstate = &worldstate;

	CRacialStats rs;
	{
		
	}
	charinfo.rstats = &rs;

	IO.CreateCharacterFile( nid, charinfo );
}