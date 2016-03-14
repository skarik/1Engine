
#include "CMCCRealm.h"

#include "core/settings/CGameSettings.h"
#include "core/system/io/CBinaryFile.h"

arstring<256>	CMCCRealm::m_currentRealm ( "" );

CMCCRealm::CMCCRealm ( const char* targetRealm )
	: m_targetRealm( targetRealm ), b_activeRealm( false )
{
	// First reset settings
	settings.seed			= 0;
	settings.seed_name[0]	= '\0';
	settings.selection_count= 0;
	settings.soulname_previous_person[0] = '\0';
	settings.person_count	= 0;
	// Now perform loading info
	CheckActiveRealm();
	if ( m_targetRealm.length() == 0 ) {
		m_targetRealm = CGameSettings::Active()->GetRealmTargetName().c_str();
		b_activeRealm = true;
	}
	Load();
}

CMCCRealm::~CMCCRealm ( void )
{
	Save();
}


void CMCCRealm::CheckActiveRealm ( void )
{
	m_currentRealm = CGameSettings::Active()->GetRealmTargetName().c_str();
	if ( m_currentRealm == m_targetRealm ) {
		b_activeRealm = true;
	}
	else {
		b_activeRealm = false;
	}
}

// Sets the seed for the realm.
// If this is the active realm, will set the default seed
void CMCCRealm::SetSeed ( const int32_t n_seed )
{
	CheckActiveRealm();
	settings.seed = n_seed;
	if ( b_activeRealm ) {
		CGameSettings::Active()->i_cl_DefaultSeed = settings.seed;
		UploadSettings();
	}
}
// Set seed via string hash w/ special cases
void CMCCRealm::SetSeed ( const char* str )
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
int32_t CMCCRealm::GetSeed ( void )
{
	CheckActiveRealm();
	if ( b_activeRealm ) {
		CGameSettings::Active()->i_cl_DefaultSeed = settings.seed;
		UploadSettings();
	}
	return settings.seed;
}

// Increment the selection count
void CMCCRealm::IncSelectCount ( void )
{
	settings.selection_count++;
}
// Get the selection count
uint32_t CMCCRealm::GetSelectCount ( void )
{
	return settings.selection_count;
}

// Set the name of the last character played on the world
void CMCCRealm::SetSavedSoulname ( const char* n_newname )
{
	strcpy( settings.soulname_previous_person, n_newname );
}
// Get the name of the last character played on the world
const char* CMCCRealm::GetSavedSoulname ( void )
{
	return settings.soulname_previous_person;
}

// Increment the person count
void CMCCRealm::IncPersonCount ( void )
{
	settings.person_count++;
}
// Get the person count
uint32_t CMCCRealm::GetPersonCount ( void )
{
	return settings.person_count;
}

// Upload the REALM settings to the main world setting
void CMCCRealm::UploadSettings ( void )
{
	CGameSettings::Active()->i_cl_DefaultSeed = settings.seed;
	std::cout << "Setting world seed: " << std::hex << settings.seed << std::dec << std::endl;
}



// Saves the realm settings
void CMCCRealm::Save ( void )
{
	CGameSettings::Active()->SetRealmSaveTarget( m_targetRealm.c_str() );

	// Save file, creating if possible
	CBinaryFile savefile;
	savefile.Open( (CGameSettings::Active()->MakeRealmSaveDirectory()+"/settings").c_str(), CBinaryFile::IO_WRITE );
	if ( savefile.IsOpen() )
	{
		FILE* fp = savefile.GetFILE();
		fwrite( (char*)&settings, sizeof( realmSettings_t ), 1, fp );
	}

	CGameSettings::Active()->SetRealmSaveTarget( m_currentRealm.c_str() );
}
// Loads realm settings from file without saving
void CMCCRealm::Load ( void )
{
	CGameSettings::Active()->SetRealmSaveTarget( m_targetRealm.c_str() );

	// Load file, creating if possible
	CBinaryFile loadfile;
	loadfile.Open( (CGameSettings::Active()->MakeRealmSaveDirectory()+"/settings").c_str(), CBinaryFile::IO_READ );
	if ( loadfile.IsOpen() )
	{
		FILE* fp = loadfile.GetFILE();
		fread( (char*)&settings, sizeof( realmSettings_t ), 1, fp );
		if ( b_activeRealm ) {
			UploadSettings();
		}
	}

	CGameSettings::Active()->SetRealmSaveTarget( m_currentRealm.c_str() );
}