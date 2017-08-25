
#include "CMCCPlanet.h"
#include "core/settings/CGameSettings.h"
#include "core/system/io/CBinaryFile.h"
#include "core/system/io/FileUtils.h"
#include "core/exceptions/exceptions.h"

#include "after/terrain/Zones.h"
#include "after/types/WorldVector.h"
#include "after/terrain/io/COctreeIO.h"

arstring<256>	CMCCPlanet::m_currentPlanet ( "" );

CMCCPlanet::CMCCPlanet ( const char* targetPlanet )
	: m_targetPlanet( targetPlanet ), b_activePlanet( false )
{
	CheckActivePlanet();
	if ( m_targetPlanet.length() == 0 ) {
		m_targetPlanet = CGameSettings::Active()->GetTerrainSaveFile().c_str();
		b_activePlanet = true;
	}
	Load();
}

CMCCPlanet::~CMCCPlanet ( void )
{
	Save();
}


void CMCCPlanet::CheckActivePlanet ( void )
{
	m_currentPlanet = CGameSettings::Active()->GetTerrainSaveFile().c_str();
	if ( m_currentPlanet == m_targetPlanet ) {
		b_activePlanet = true;
	}
	else {
		b_activePlanet = false;
	}
}

// Sets the seed for the realm.
// If this is the active realm, will set the default seed
void CMCCPlanet::SetSeed ( const int32_t n_seed )
{
	CheckActivePlanet();
	settings.seed = n_seed;
	/*if ( b_activeRealm ) {
		CGameSettings::Active()->i_cl_DefaultSeed = settings.seed;
		UploadSettings();
	}*/
}
// Set seed via string hash w/ special cases
void CMCCPlanet::SetSeed ( const char* str )
{
	// Copy seed name over
	strcpy( settings.seed_name, str );

	// Perform special cases first
	if ( strcmp( str, "ZER0" ) == 0 ) {
		SetSeed( 0 );
	}
	else
	{
		// use djb2 hash
		unsigned long hash = 5381;
	    int c;
		while ( c = (unsigned)(*str++) ) { // While not null char
			hash = ((hash << 5) + hash) + c; /* hash * 33 + c */ // Add to hash
		}
		SetSeed( (int32_t)hash );
	}
}
// Returns the seed for the realm.
// If this is the active realm, will set the default seed
int32_t CMCCPlanet::GetSeed ( void )
{
	CheckActivePlanet();
	/*if ( b_activeRealm ) {
		CGameSettings::Active()->i_cl_DefaultSeed = settings.seed;
		UploadSettings();
	}*/
	return settings.seed;
}

//  GetDataAt ( VECTOR2D query position, TERRAIN_SAMPLE_QUERY output data )
// Samples the world for data. Modifies out_data with the results.
// Returns true on successful query. Otherwise, returns false and out_data is not modified.
bool CMCCPlanet::GetDataAt ( const Vector2d& n_position, World::terrainSampleQuery_t* out_data )
{
	// If active, then query terrain.
	if ( b_activePlanet && Zones.GetActiveTerrain() )
	{
		// Query terrain for all the information. If planet is active, then we automatically assume that Zones is active.
		out_data->biome = Zones.GetTerrainBiomeAt( Vector3d(n_position) );
		out_data->terra = Zones.GetTerrainTypeAt( Vector3d(n_position) );
		out_data->elevation = 0;
		//out_data->elevation = Zones.GetTerrainElevationAt( Vector3d(n_position) );

		return true;
	}
	// If not active, find the file the matches the position
	else
	{
		throw Core::NotYetImplementedException();
		/*RangeVector targetPosition;
		char stemp_fname [256];

		// Set target planet
		CGameSettings::Active()->SetTerrainSaveFile( m_targetPlanet.c_str() );

		// Generate filename
		targetPosition = Zones.PositionToRV( Vector3d(n_position) );
		sprintf( stemp_fname, "%s/%d_%d_%d", CGameSettings::Active()->GetTerrainSaveDir().c_str(), targetPosition.x, targetPosition.y, targetPosition.z );
		//cout << stemp_fname << endl;
		// Set current planet
		CGameSettings::Active()->SetTerrainSaveFile( m_currentPlanet.c_str() );

		// Now, find the file
		if ( IO::FileExists(stemp_fname) )
		{
			CBinaryFile file;
			Terrain::COctreeIO::filestruct_t fs;

			file.Open( stemp_fname, CBinaryFile::IO_READ );
			// Load the data
			fread( &fs, sizeof( Terrain::COctreeIO::filestruct_t ), 1, file.GetFILE() );
			// Set data
			out_data->biome = fs.biome_type;
			out_data->terra = fs.terra_type;
			out_data->elevation = fs.elevation;

			// Close file
			file.Close();
			
			// Return data was found
			return true;
		}
		else
		{
			return false;
		}*/
	}
}







void CMCCPlanet::UploadSettings ( void )
{
	//CGameSettings::Active()->i_cl_DefaultSeed = settings.seed;
	//cout << "Setting world seed: " << std::hex << settings.seed << std::dec << endl;
}

// Saves the realm settings
void CMCCPlanet::Save ( void )
{
	CGameSettings::Active()->SetTerrainSaveFile( m_targetPlanet.c_str() );

	// Save file, creating if possible
	CBinaryFile savefile;
	savefile.Open( (CGameSettings::Active()->GetWorldSaveDir()+"/"+CGameSettings::Active()->GetTerrainSaveFile()+".settings").c_str(), CBinaryFile::IO_WRITE );
	if ( savefile.IsOpen() )
	{
		FILE* fp = savefile.GetFILE();
		fwrite( (char*)&settings, sizeof( planetSettings_t ), 1, fp );
	}

	CGameSettings::Active()->SetTerrainSaveFile( m_currentPlanet.c_str() );
}
// Loads realm settings from file without saving
void CMCCPlanet::Load ( void )
{
	CGameSettings::Active()->SetTerrainSaveFile( m_targetPlanet.c_str() );

	// Load file, creating if possible
	CBinaryFile loadfile;
	loadfile.Open( (CGameSettings::Active()->GetWorldSaveDir()+"/"+CGameSettings::Active()->GetTerrainSaveFile()+".settings").c_str(), CBinaryFile::IO_READ );
	if ( loadfile.IsOpen() )
	{
		FILE* fp = loadfile.GetFILE();
		fread( (char*)&settings, sizeof( planetSettings_t ), 1, fp );
		if ( b_activePlanet ) {
			UploadSettings();
		}
	}

	CGameSettings::Active()->SetTerrainSaveFile( m_currentPlanet.c_str() );
}