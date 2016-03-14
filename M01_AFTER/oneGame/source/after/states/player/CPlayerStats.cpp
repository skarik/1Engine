
#include <sstream>

#include "core/math/random/Random.h"
#include "core/system/io/CBinaryFile.h"
#include "core-ext/system/io/serializer.h"
#include "core-ext/system/io/serializer/ISerialBinary.h"
#include "core-ext/system/io/serializer/OSerialBinary.h"

#include "after/states/world/CMCCRealm.h"
#include "after/terrain/generation/CWorldGenerator.h"

// Include stats, character, and game settings
#include "CPlayerStats.h"
#include "after/states/CharacterStats.h"
#include "after/entities/character/CCharacter.h"

#include "core/settings/CGameSettings.h"

// Include buffs to be added to the character
#include "after/states/debuffs/racial/BuffRacialDarkElf.h"

// Player stats initialization
CPlayerStats::CPlayerStats ( void )
{
	race_stats	= new CRacialStats();
	race_stats->stats	= new CharacterStats();
	stats		= race_stats->stats;
	pOwner		= NULL;
};

// Set Default stats
void CPlayerStats::SetDefaults ( void )
{
	// ====== GAMEPLAY STATS ======
	// Skill points
	skillpoints_race	= 1;
	skillpoints_skill	= 1;
	memset( skillpoints_discipline, 0, sizeof(uint16_t)*30 );

	// Record stats
	bHasPlayed		= false;
	fDeathCount		= 0;
	fFeetFallen		= 0;
	fFeetTravelled	= 0;

	// Racial stats
	race_stats->SetDefaults();

	// Game Saves
	vPlayerInitSpawnPoint = Vector3d(0,0,0);

	// ====== GAME MECHANICS STATS ======
	// Companion stats
	companion_sex_prefs		= SEXPREF_RANDOM;
	companion_love_prefs	= CLOVE_RANDOM;
	companion_gender		= CGEND_RANDOM;
	companion_race			= CRACE_SAME;
}

// Randomizes stats
void CPlayerStats::Randomize ( void )
{
	SetDefaults();

	// Choose a random race
	do {
		race_stats->iRace = (eCharacterRace)((Random.Next()%6)+1);
	} while ( race_stats->iRace == CRACE_DWARF || race_stats->iRace == CRACE_MERCHANT );
	race_stats->iGender = (eCharacterGender)(Random.Next()%2);
	race_stats->iMiscType = Random.Next()%3;
	do {
		race_stats->iMovestyle = Random.Next()%CMOVES_MAX;
	} while ( race_stats->iMovestyle == CMOVES_LARGE_HAM );

	race_stats->iRace = CRACE_FLUXXOR;
	//race_stats->iRace = CRACE_HUMAN;
	race_stats->iGender = CGEND_FEMALE;

	//race_stats->iRace = CRACE_KITTEN;
	//race_stats->iGender = CGEND_FEMALE;
	//race_stats->iMovestyle = CMOVES_JOLLY;
	//race_stats->iMovestyle = CMOVES_DEFAULT;

	// Randomize colors
	race_stats->RerollColors();
}

// Load stats from file
void CPlayerStats::LoadFromFile ( void )
{
	CBinaryFile file;
	std::stringstream tempStream;
	string sFilename;
	tempStream << CGameSettings::Active()->MakePlayerSaveDirectory() << "/stats";
	sFilename = tempStream.str();
	if ( file.Exists( sFilename.c_str() ) )
	{
		// == Open file for reading ==
		file.Open( sFilename.c_str(), CBinaryFile::IO_READ );

		// ====== VERSION INFO ======
		file.ReadUInt32();
		file.ReadUInt32();
		file.ReadUInt32();
		uint32_t _ver3 = file.ReadUInt32();

		// General Player Stats
		ISerialBinary deserializer ( &file, _ver3 );
		deserializer >> (*this);

		// Racial stats
		race_stats->LoadFromFile( file );

		// Game save info
		if ( _ver3 > 1 )
		{
			// Read spawnpoints
			uint32_t spawnpointCount = file.ReadUInt32();
			for ( uint32_t i = 0; i < spawnpointCount; ++i ) {
				string sterraname = file.ReadString();
				Vector3d_d spawnpoint;
				spawnpoint.x = file.ReadDouble();
				spawnpoint.y = file.ReadDouble();
				spawnpoint.z = file.ReadDouble();
				// Check for invalid spawnpoints
				/*if ( !(VALID_FLOAT(spawnpoint.x)&&VALID_FLOAT(spawnpoint.y)&&VALID_FLOAT(spawnpoint.z)) || (spawnpoint.magnitude() > 150000000) )
				{
					spawnpoint = Vector3d_d( 1,1,1 );
				}*/
				mPlayerSpawnpointMap[sterraname] = spawnpoint;
			}

			// Set current world spawnpoint to Vector3d::zero if cannot find it
			string sfilename = CGameSettings::Active()->GetRealmTargetName()+CGameSettings::Active()->GetWorldTargetName();
			if ( mPlayerSpawnpointMap.find( sfilename ) == mPlayerSpawnpointMap.end() ) {
				bHasPlayed = false;
				vPlayerInitSpawnPoint = Vector3d(0,0,0);
			}
			else {
				bHasPlayed = true;
				vPlayerInitSpawnPoint = Vector3d(
					(Real) mPlayerSpawnpointMap[sfilename].x,
					(Real) mPlayerSpawnpointMap[sfilename].y,
					(Real) mPlayerSpawnpointMap[sfilename].z );
			}
		}

		// == Close file ==
		file.Close();
	}
	else
	{
		// Set defaults
		SetDefaults();
		// Save stats
		SaveToFile();
	}
}

// Save stats to file
void CPlayerStats::SaveToFile ( void )
{
	CBinaryFile file;
	std::stringstream tempStream;
	string sFilename;
	tempStream << CGameSettings::Active()->MakePlayerSaveDirectory() << "/stats";
	sFilename = tempStream.str();
	
	// Open file for writing
	file.Open( sFilename.c_str(), CBinaryFile::IO_WRITE );

	if ( file.IsOpen() )
	{
		// ====== VERSION INFO ======
		file.WriteUInt32( 0 );
		file.WriteUInt32( 0 );
		file.WriteUInt32( 0 );
		uint32_t _ver3 = 4;
		file.WriteUInt32( _ver3 );

		// General Player Stats
		OSerialBinary serializer ( &file, _ver3 );
		serializer << (*this);

		// Racial stats
		race_stats->SaveToFile( file );

		// Write spawnpoints
		file.WriteUInt32( mPlayerSpawnpointMap.size() );
		for ( auto spawnInfo = mPlayerSpawnpointMap.begin(); spawnInfo != mPlayerSpawnpointMap.end(); ++spawnInfo )
		{
			file.WriteString( spawnInfo->first );
			file.WriteDouble( spawnInfo->second.x );
			file.WriteDouble( spawnInfo->second.y );
			file.WriteDouble( spawnInfo->second.z );
		}

		// Close file
		file.Close();
	}
}


void CPlayerStats::serialize ( Serializer & ser, const uint ver )
{	
	// ====== GAMEPLAY STATS ======
	// Game stats
	ser & vPlayerInitSpawnPoint;
	ser & vPlayerCalcSpawnPoint;

	// Record stats
	ser & bHasPlayed;
	ser & fDeathCount;
	ser & fFeetFallen;
	ser & fFeetTravelled;

	// Version 3 Lock
	if ( ver <= 2 ) return;
	
	// ====== GAME MECHANICS STATS ======
	// Companion stats
	ser & (uint8_t&)companion_sex_prefs;
	ser & (uint8_t&)companion_love_prefs;
	ser & (uint8_t&)companion_gender;
	ser & (uint8_t&)companion_race;

	// Version 4 Lock
	if ( ver <= 3 ) return;

	// Skill points
	ser & skillpoints_race;
	ser & skillpoints_skill;
	for ( uint i = 0; i < 32; ++i ) {
		ser & skillpoints_discipline[i];
	}
	for ( uint i = 0; i < 32; ++i ) {
		ser & skillpoints_experience[i];
	}
}

// Generate spawnpoint
Vector3d_d CPlayerStats::MakeSpawnpoint ( Terrain::CWorldGenerator* n_generator )
{
	//CVoxelTerrain*	activeTerrain = CVoxelTerrain::GetActive();
	//if ( activeTerrain )
	//{
	//	// Start at origin point
	//	vPlayerCalcSpawnPoint = vPlayerInitSpawnPoint;

	//	// Loop through blocks until hit the top
	//	BlockInfo currentBlock;
	//	do
	//	{
	//		//if ( activeTerrain->GetBlockInfoAtPosition( vPlayerCalcSpawnPoint, currentBlock ) == false )
	//		//	currentBlock.block.block = EB_NONE;
	//		activeTerrain->GetBlockInfoAtPosition( vPlayerCalcSpawnPoint, currentBlock );
	//		vPlayerCalcSpawnPoint.z += 2;
	//	} while (( currentBlock.block.block != EB_NONE )&&( vPlayerCalcSpawnPoint.z < 60 ));

	//	// Move spawn point up a little more.
	//	vPlayerCalcSpawnPoint.z += 4;
	//}
	CMCCRealm targetRealm;
	return n_generator->GetSpawnPoint( 1, 0, targetRealm.GetPersonCount() );
}

// Update stats per-step
void CPlayerStats::UpdateStats ( void )
{
	// Only thing really worth updating
	race_stats->UpdateStats();

}

// Level shit up
void CPlayerStats::LevelUp ( void )
{
	// Give player both a race point and a general tree point
	skillpoints_race	+= 1;
	skillpoints_skill	+= 1;
	// Level them up
	race_stats->LevelUp();

	// Save character
	SaveToFile();
}
