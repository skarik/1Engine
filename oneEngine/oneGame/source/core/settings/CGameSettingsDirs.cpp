#include "CGameSettings.h"

#ifdef _WIN32
	#include "core/os.h"
#elif (__linux || __unix__ || BSD)
	#include <sys/dir.h>
	#include <sys/stat.h>
#elif __APPLE__
	#ifdef TARGET_OS_IPHONE
		
	#elif TARGET_IPHONE_SIMULATOR
		
	#elif TARGET_OS_MAC
		
	#endif
#endif

// == Make Directory ==
void CGameSettings::MakeDirectory ( const string& directory )
{
	return MakeDirectory( directory.c_str() );
}
void CGameSettings::MakeDirectory ( const char *directory )
{
#ifdef _WIN32
	CreateDirectory( directory, NULL );
#elif (__linux || __unix__ || BSD)
	//mkdir( directory, S_IRWXU );
	mkdir( directory, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH ); //Todo: Find documentation on these damn permissions
#elif __APPLE__
	#ifdef TARGET_OS_IPHONE
		
	#elif TARGET_IPHONE_SIMULATOR
		
	#elif TARGET_OS_MAC
		
	#endif
#endif
}

#include <iostream>

// == Set Save Files ==
// These aren't actually directories, but simple strings giving the names of save files.
void CGameSettings::SetWorldSaveTarget ( const char* sIn )
{
	if ( m_target_file_world != sIn ) {
		std::cout << "World set to " << sIn << std::endl;
	}
	if ( strnlen(sIn,1024) >= 1 ) {
		m_target_file_world = sIn;
	}
	else {
		m_target_file_world = "terra";
	}
	// Recreate realm directory
	SetRealmSaveTarget( m_target_file_realm.c_str() );
	// Create world directory
	string sCurrentDirectory;
	sCurrentDirectory = ".game/.realms/" + m_target_file_realm;
	MakeDirectory( sCurrentDirectory.c_str() );
	sCurrentDirectory = ".game/.realms/" + m_target_file_realm + "/" + m_target_file_world + ".regions";
	MakeDirectory( sCurrentDirectory.c_str() );
	sCurrentDirectory = ".game/.realms/" + m_target_file_realm + "/" + m_target_file_world + ".towns";
	MakeDirectory( sCurrentDirectory.c_str() );
	sCurrentDirectory = ".game/.realms/" + m_target_file_realm + "/" + m_target_file_world + ".dungeons";
	MakeDirectory( sCurrentDirectory.c_str() );
	sCurrentDirectory = ".game/.realms/" + m_target_file_realm + "/" + m_target_file_world + ".loot";
	MakeDirectory( sCurrentDirectory.c_str() );
	sCurrentDirectory = ".game/.realms/" + m_target_file_realm + "/" + m_target_file_world;
	MakeDirectory( sCurrentDirectory.c_str() );
}
void CGameSettings::SetRealmSaveTarget ( const char* sIn )
{
	if ( m_target_file_realm != sIn ) {
		std::cout << "Realm set to " << sIn << std::endl;
	}
	if ( strnlen(sIn,1024) >= 1 ) {
		m_target_file_realm = sIn;
	}
	else {
		m_target_file_realm = "_lucra";
	}
	// Create realm directories
	string sCurrentDirectory;
	sCurrentDirectory = ".game/.realms/" + m_target_file_realm;
	MakeDirectory( sCurrentDirectory.c_str() );
	sCurrentDirectory = ".game/.realms/" + m_target_file_realm + "/chars";
	MakeDirectory( sCurrentDirectory.c_str() );
}
void CGameSettings::SetPlayerSaveTarget ( const char* sIn )
{
	if ( m_target_file_player != sIn ) {
		std::cout << "Player set to " << sIn << std::endl;
	}
	m_target_file_player = sIn;
	// Create player directories
	string sCurrentDirectory;
	sCurrentDirectory = ".game/.players/" + m_target_file_player;
	MakeDirectory( sCurrentDirectory.c_str() );
	sCurrentDirectory = ".game/.players/" + m_target_file_player + "/logbook";
	MakeDirectory( sCurrentDirectory.c_str() );
	sCurrentDirectory = ".game/.players/" + m_target_file_player + "/skills";
	MakeDirectory( sCurrentDirectory.c_str() );
	sCurrentDirectory = ".game/.players/" + m_target_file_player;
}

// == Getters ==
// These can return references because they're references to class data
const string& CGameSettings::GetWorldTargetName ( void )
{
	return m_target_file_world;
}
const string& CGameSettings::GetRealmTargetName ( void )
{
	return m_target_file_realm;
}
const string& CGameSettings::GetPlayerTargetName ( void )
{
	return m_target_file_player;
}

// == Get Save Directories ==
// Using the save files, we construct directories.
string CGameSettings::MakeRealmSaveDirectory ( void )
{
	string sCurrentDirectory;
	sCurrentDirectory = ".game/.realms/" + m_target_file_realm;
	return sCurrentDirectory;
}
string CGameSettings::MakeRealmSaveDirectory ( const string& realmName )
{
	string sCurrentDirectory;
	sCurrentDirectory = ".game/.realms/" + realmName;
	return sCurrentDirectory;
}
string CGameSettings::MakeWorldSaveDirectory ( void )
{
	string sCurrentDirectory;
	sCurrentDirectory = ".game/.realms/" + m_target_file_realm + "/" + m_target_file_world;
	return sCurrentDirectory;
}
string CGameSettings::MakePlayerSaveDirectory ( void )
{
	string sCurrentDirectory;
	sCurrentDirectory = ".game/.players/" + m_target_file_player;
	return sCurrentDirectory;
}
string CGameSettings::MakePlayerSaveDirectory ( const string& playerName )
{
	string sCurrentDirectory;
	sCurrentDirectory = ".game/.players/" + playerName;
	return sCurrentDirectory;
}

void CGameSettings::GetPlayerFileList ( std::vector<string>& stringlist )
{
	string dir = ".game/.players/";
	stringlist.clear();

#ifdef _WIN32
	HANDLE			hFind = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATA	ffd;

	dir += "*";

	hFind = FindFirstFile( dir.c_str(), &ffd );

	do {
		if ( ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) {
			if (( strcmp( ".", ffd.cFileName ) != 0 )&&( strcmp( "..", ffd.cFileName ) != 0 )) {
				stringlist.push_back(string(ffd.cFileName));
			}
		}
	}
	while ( FindNextFile(hFind, &ffd) != 0 );

	FindClose( hFind );
#else
	DIR *dp;
    struct dirent *dirp;
    if((dp  = opendir(dir.c_str())) == NULL) {
        std::cout << "Error(" << errno << ") opening " << dir << std::endl;
        return errno;
    }

    while ((dirp = readdir(dp)) != NULL) {
        stringlist.push_back(string(dirp->d_name));
    }
    closedir(dp);
#endif
}

void CGameSettings::GetRealmFileList ( std::vector<string>& stringlist )
{
	string dir = ".game/.realms/";
	stringlist.clear();

#ifdef _WIN32
	HANDLE			hFind = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATA	ffd;

	dir += "*";

	hFind = FindFirstFile( dir.c_str(), &ffd );

	do {
		if ( ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) {
			if (( strcmp( ".", ffd.cFileName ) != 0 )&&( strcmp( "..", ffd.cFileName ) != 0 )) {
				stringlist.push_back(string(ffd.cFileName));
			}
		}
	}
	while ( FindNextFile(hFind, &ffd) != 0 );

	FindClose( hFind );
#else
	DIR *dp;
    struct dirent *dirp;
    if((dp  = opendir(dir.c_str())) == NULL) {
        std::cout << "Error(" << errno << ") opening " << dir << std::endl;
        return errno;
    }

    while ((dirp = readdir(dp)) != NULL) {
        stringlist.push_back(string(dirp->d_name));
    }
    closedir(dp);
#endif
}