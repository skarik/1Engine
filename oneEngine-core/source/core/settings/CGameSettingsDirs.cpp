
// Includes
#include "CGameSettings.h"

#ifdef _WIN32
	// No MFC
	#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN 1
	#endif
	#include <windows.h>
#elif __linux
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
#elif __linux
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
void CGameSettings::SetTerrainSaveFile ( const string& sIn )
{
	if ( sSaveFileTerrain != sIn ) {
		std::cout << "World set to " << sIn << std::endl;
	}
	if ( sIn.length() >= 1 ) {
		sSaveFileTerrain = sIn;
	}
	else {
		sSaveFileTerrain = "terra";
	}
	if ( sSaveFileWorld != "_lucra" )
	{
		string sCurrentDirectory;
		sCurrentDirectory = ".game/.realms/" + sSaveFileWorld;
		MakeDirectory( sCurrentDirectory.c_str() );
		sCurrentDirectory = ".game/.realms/" + sSaveFileWorld + "/" + sSaveFileTerrain + ".regions";
		MakeDirectory( sCurrentDirectory.c_str() );
		sCurrentDirectory = ".game/.realms/" + sSaveFileWorld + "/" + sSaveFileTerrain + ".towns";
		MakeDirectory( sCurrentDirectory.c_str() );
		sCurrentDirectory = ".game/.realms/" + sSaveFileWorld + "/" + sSaveFileTerrain + ".dungeons";
		MakeDirectory( sCurrentDirectory.c_str() );
		sCurrentDirectory = ".game/.realms/" + sSaveFileWorld + "/" + sSaveFileTerrain + ".loot";
		MakeDirectory( sCurrentDirectory.c_str() );
		sCurrentDirectory = ".game/.realms/" + sSaveFileWorld + "/" + sSaveFileTerrain;
		MakeDirectory( sCurrentDirectory.c_str() );
	}
}
void CGameSettings::SetWorldSaveFile ( const string& sIn )
{
	if ( sSaveFileWorld != sIn ) {
		std::cout << "Realm set to " << sIn << std::endl;
	}
	if ( sIn.length() >= 1 ) {
		sSaveFileWorld = sIn;
	}
	else {
		sSaveFileWorld = "_lucra";
	}
	if ( sSaveFileWorld != "_lucra" )
	{
		string sCurrentDirectory;
		sCurrentDirectory = ".game/.realms/" + sSaveFileWorld;
		MakeDirectory( sCurrentDirectory.c_str() );
		sCurrentDirectory = ".game/.realms/" + sSaveFileWorld + "/chars";
		MakeDirectory( sCurrentDirectory.c_str() );
	}
}
void CGameSettings::SetPlayerSaveFile ( const string& sIn )
{
	if ( sSaveFilePlayer != sIn ) {
		std::cout << "Player set to " << sIn << std::endl;
	}
	sSaveFilePlayer = sIn;
	if ( sSaveFilePlayer != "_default" )
	{
		string sCurrentDirectory;
		sCurrentDirectory = ".game/.players/" + sSaveFilePlayer;
		MakeDirectory( sCurrentDirectory.c_str() );
		sCurrentDirectory = ".game/.players/" + sSaveFilePlayer + "/logbook";
		MakeDirectory( sCurrentDirectory.c_str() );
		sCurrentDirectory = ".game/.players/" + sSaveFilePlayer + "/skills";
		MakeDirectory( sCurrentDirectory.c_str() );
		sCurrentDirectory = ".game/.players/" + sSaveFilePlayer;
	}
}

// == Getters ==
// These can return references because they're references to class data
const string& CGameSettings::GetTerrainSaveFile ( void )
{
	return sSaveFileTerrain;
}
const string& CGameSettings::GetWorldSaveFile ( void )
{
	return sSaveFileWorld;
}
const string& CGameSettings::GetPlayerSaveFile ( void )
{
	return sSaveFilePlayer;
}

// == Get Save Directories ==
// Using the save files, we construct directories.
string CGameSettings::GetWorldSaveDir ( void )
{
	string sCurrentDirectory;
	sCurrentDirectory = ".game/.realms/" + sSaveFileWorld;
	//MakeDirectory( sCurrentDirectory.c_str() );
	return sCurrentDirectory;
}
string CGameSettings::GetWorldSaveDir ( const string& realmName )
{
	string sCurrentDirectory;
	sCurrentDirectory = ".game/.realms/" + realmName;
	return sCurrentDirectory;
}
string CGameSettings::GetTerrainSaveDir ( void )
{
	string sCurrentDirectory;
	/*sCurrentDirectory = ".game\\.realms\\" + sSaveFileWorld;
	MakeDirectory( sCurrentDirectory.c_str() );
	sCurrentDirectory = ".game\\.realms\\" + sSaveFileWorld + "\\" + sSaveFileTerrain + ".regions";
	MakeDirectory( sCurrentDirectory.c_str() );
	sCurrentDirectory = ".game\\.realms\\" + sSaveFileWorld + "\\" + sSaveFileTerrain + ".towns";
	MakeDirectory( sCurrentDirectory.c_str() );
	sCurrentDirectory = ".game\\.realms\\" + sSaveFileWorld + "\\" + sSaveFileTerrain + ".dungeons";
	MakeDirectory( sCurrentDirectory.c_str() );
	sCurrentDirectory = ".game\\.realms\\" + sSaveFileWorld + "\\" + sSaveFileTerrain + ".loot";
	MakeDirectory( sCurrentDirectory.c_str() );
	sCurrentDirectory = ".game\\.realms\\" + sSaveFileWorld + "\\" + sSaveFileTerrain;
	MakeDirectory( sCurrentDirectory.c_str() );*/
	sCurrentDirectory = ".game/.realms/" + sSaveFileWorld + "/" + sSaveFileTerrain;
	return sCurrentDirectory;
}
string CGameSettings::GetPlayerSaveDir ( void )
{
	string sCurrentDirectory;
	/*sCurrentDirectory = ".game\\.players\\" + sSaveFilePlayer;
	MakeDirectory( sCurrentDirectory.c_str() );
	sCurrentDirectory = ".game\\.players\\" + sSaveFilePlayer + "\\logbook";
	MakeDirectory( sCurrentDirectory.c_str() );
	sCurrentDirectory = ".game\\.players\\" + sSaveFilePlayer + "\\skills";
	MakeDirectory( sCurrentDirectory.c_str() );
	sCurrentDirectory = ".game\\.players\\" + sSaveFilePlayer;*/
	sCurrentDirectory = ".game/.players/" + sSaveFilePlayer;
	return sCurrentDirectory;
}
string CGameSettings::GetPlayerSaveDir ( const string& playerName )
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