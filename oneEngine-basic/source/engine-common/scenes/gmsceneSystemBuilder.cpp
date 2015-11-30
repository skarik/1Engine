
#include "gmsceneSystemBuilder.h"

// Include game settings
#include "core/settings/CGameSettings.h"
#include "core/containers/arstring.h"
// Include debug console
#include "core/debug/CDebugConsole.h"
#include "engine/utils/CDeveloperConsole.h"
#include "engine-common/utils/CDeveloperConsoleUI.h"
// Include camera for rendering
#include "renderer/Camera/CCamera.h"

// Include OSF IO
#include "core-ext/system/io/mccosf.h"
#include "core-ext/system/io/Resources.h"

void gmsceneSystemBuilder::LoadScene ( void )
{
	Debug::Console->PrintMessage( "Loading scene (System Builder).\n" );

	// Create the console (basically our script link)
	CDeveloperConsoleUI* devConsole = new CDeveloperConsoleUI();
	CDeveloperCursor* devCursor = new CDeveloperCursor();

	// Add the builder command
	Engine::Console->AddConsoleFunc( "build", EngineCommon::BuildToTarget );

	// Print a prompt
	Debug::Console->PrintMessage( "You are running a debug build of oneEngine.\n" );
	Debug::Console->PrintMessage( "Please enter a build to create, or \"scene default\" to go back.\n" );
}

enum buildMode_t
{
	BUILD_MODE_UNSPECIFIED,
	BUILD_MODE_COPY,
	BUILD_MODE_UNKNOWN
};
enum buildTargets_t
{
	BUILD_TARGET_NONE = 0x00,
	BUILD_TARGET_COMMON = 0x01,
	BUILD_TARGET_IMAGES = 0x02,
	BUILD_TARGET_MODELS = 0x04
};
void BuildWithInformation ( const char* n_build_directory, const buildMode_t n_build_mode, const uint32_t n_rebuild_mask );

int EngineCommon::BuildToTarget ( const std::string& n_cmd )
{
	// Using the OSF system, load up /system/buildtargets.txt
	// Need to locate the matching object
	FILE* fp_loader = fopen( Core::Resources::PathTo("system/buildtargets.txt").c_str(), "rb" );
	COSF_Loader osf_loader ( fp_loader );

	// Read in entries until we hit the end or until we hit a match
	mccOSF_entry_info_t entry;
	osf_loader.GetNext( entry );
	while ( entry.type != mccOSF_entrytype_enum::MCCOSF_ENTRY_EOF )
	{
		if ( strcmp( entry.name, n_cmd.c_str() ) == 0 )
		{
			Debug::Console->PrintMessage( "Building to target \"" + string(entry.name) + "\"\n" );

			arstring<256> m_builddir ( entry.value );
			buildMode_t m_buildmode = BUILD_MODE_UNSPECIFIED;
			uint32_t m_rebuildtargets = BUILD_TARGET_NONE;

			// Found it, load this one
			osf_loader.GoInto( entry );
			osf_loader.GetNext( entry );
			while ( entry.type != mccOSF_entrytype_enum::MCCOSF_ENTRY_END && entry.type != mccOSF_entrytype_enum::MCCOSF_ENTRY_EOF )
			{
				// Read in the project types
				arstring<256> key ( entry.name );
				arstring<256> value ( entry.value );

				if ( key.compare("mode") )
				{
					if ( value.compare("copy") )
						m_buildmode = BUILD_MODE_COPY;
					else
						m_buildmode = BUILD_MODE_UNKNOWN;
				}
				else if ( key.compare("rebuild") )
				{
					if ( value.compare("images") )
						m_rebuildtargets |= BUILD_TARGET_IMAGES;
					else if ( value.compare("models") )
						m_rebuildtargets |= BUILD_TARGET_MODELS;
					else
						Debug::Console->PrintWarning( "+Unrecognized rebuild target \"" + string(value) + "\"\n" );
				}

				// Read next
				osf_loader.GetNext( entry );
			}
			
			// Show what we're building to
			printf( "+Build mode %x\n", m_buildmode );
			printf( "+Rebuild targeting %x\n", m_rebuildtargets );

			// Close the file we opened to write
			fclose( fp_loader );

			// Begin the actual builder
			BuildWithInformation( m_builddir.c_str(), m_buildmode, m_rebuildtargets );

			// Need to leave now that we've built it
			return 0;
		}
		// Read next
		osf_loader.GetNext( entry );
	}
	// Here? We could not find it. Try again later.
	Debug::Console->PrintWarning( "Could not find build target \"" + n_cmd + "\"\n" );
	return 0;
}

#include <filesystem>
#include <thread>
namespace fs = std::tr2::sys;

void BuildWithInformation ( const char* n_build_directory, const buildMode_t n_build_mode, const uint32_t n_rebuild_mask )
{
	// First, go to the directory and clear it out completely.
	fs::path build_path ( n_build_directory );
	fs::remove_all( build_path );
	// Wait for the filesystem to update
	while ( fs::exists( build_path ) );
	Debug::Console->PrintWarning( "Waiting 500ms for the filesystem.\n" );
	std::this_thread::sleep_for( std::chrono::milliseconds( 500 ) );
	// Now, create the directory again
	fs::create_directories( build_path );

	// Copy over the DLLs and the EXE first
	fs::directory_iterator dirend;
#ifdef _WIN32
	// Use GetModuleFileName() to get the Visual Studio build directory
	char m_exe_path [MAX_PATH]; 
	GetModuleFileName( GetModuleHandle(NULL), m_exe_path, sizeof(m_exe_path) );
#else
	// Use readlink() to get the makefile's build directory
	char m_exe_path [256];
	readlink( "/proc/self/exe", m_exe_path, sizeof(m_exe_path) );
#endif
	// Replace all \ with /
	char* m_exe_modder;
	while ( (m_exe_modder = strrchr( m_exe_path, '\\' )) != NULL ) *m_exe_modder = '/';
	// Remove the filename from the path
	*strrchr( m_exe_path, '/' ) = 0;
	// Loop through the compiler build directory
	for ( fs::directory_iterator path( m_exe_path ); path != dirend; ++path )
	{
		if ( fs::is_regular_file( path->status() ) )
		{
			string filename = path->path().filename().c_str();
			// Copy over all dynamic libraries or executables that we need
			if ( filename.rfind(".exe") != string::npos || filename.rfind(".dll") != string::npos || filename.rfind(".so") != string::npos )
			{
				printf( "copying file: %s\n", filename.c_str() );
				//fs::path target_path = build_path.string() + "/" + path->path().filename();
				//fs::copy_file( fs::path(path->path().root_path().string()), target_path, fs::copy_option::overwrite_if_exists );
				std::ifstream  src( string(m_exe_path) + "/" + filename,					std::ios::binary);
				std::ofstream  dst( build_path.string() + "/" + path->path().filename(),	std::ios::binary);
				if ( !src.is_open() ) throw Core::NullReferenceException();
				dst << src.rdbuf();
			}
		}
	}

	// Loop through the working directory for both additional DLLs and all content
	for ( fs::directory_iterator path( "." ); path != dirend; ++path )
	{
		// Check for libraries
		if ( fs::is_regular_file( path->status() ) )
		{
			string filename = path->path().filename().c_str();
			// Copy over all dynamic libraries that we need
			if (filename.rfind(".dll") != string::npos || filename.rfind(".so") != string::npos )
			{
				printf( "copying file: %s\n", filename.c_str() );
				std::ifstream  src( "./" + path->path().string(),							std::ios::binary);
				std::ofstream  dst( build_path.string() + "/" + path->path().filename(),	std::ios::binary);
				if ( !src.is_open() ) throw Core::NullReferenceException();
				dst << src.rdbuf();
			}
		}
		// Check for resource directories
		else if ( fs::is_directory( path->status() ) )
		{
			string filename = path->path().filename().c_str();
			// If it's a resource directory, step through that shit
			if ( filename.rfind(".res-") != string::npos )
			{
				printf( "found resource directory: %s\n", filename.c_str() );
			}
		}
	}
}
