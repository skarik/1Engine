
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
// Include some tools
#include "core/utils/StringUtils.h"

// Include OSF IO
#include "core-ext/system/io/mccosf.h"
#include "core-ext/system/io/Resources.h"

void gmsceneSystemBuilder::LoadScene ( void )
{
	debug::Console->PrintMessage( "Loading scene (System Builder).\n" );

	// Create the console (basically our script link)
	CDeveloperConsoleUI* devConsole = new CDeveloperConsoleUI();
	CDeveloperCursor* devCursor = new CDeveloperCursor();

	// Add the builder command
	Engine::Console->AddConsoleFunc( "build", EngineCommon::BuildToTarget );

	// Print a prompt
	debug::Console->PrintMessage( "You are running a debug build of oneEngine.\n" );
	debug::Console->PrintMessage( "Please enter a build to create, or \"scene default\" to go back.\n" );
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
	BUILD_TARGET_MODELS = 0x04,
	BUILD_TARGET_SOUNDS = 0x08
};
void BuildWithInformation ( const char* n_build_directory, const buildMode_t n_build_mode, const uint32_t n_rebuild_mask );

int EngineCommon::BuildToTarget ( const std::string& n_cmd )
{
	// Using the OSF system, load up /system/buildtargets.txt
	// Need to locate the matching object
	FILE* fp_loader = fopen( core::Resources::PathTo("system/buildtargets.txt").c_str(), "rb" );
	COSF_Loader osf_loader ( fp_loader );

	// Read in entries until we hit the end or until we hit a match
	mccOSF_entry_info_t entry;
	osf_loader.GetNext( entry );
	while ( entry.type != mccOSF_entrytype_enum::MCCOSF_ENTRY_EOF )
	{
		if ( strcmp( entry.name, n_cmd.c_str() ) == 0 )
		{
			debug::Console->PrintMessage( "Building to target \"" + string(entry.name) + "\"\n" );

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
					else if ( value.compare("sounds") )
						m_rebuildtargets |= BUILD_TARGET_SOUNDS;
					else
						debug::Console->PrintWarning( "+Unrecognized rebuild target \"" + string(value) + "\"\n" );
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
	debug::Console->PrintWarning( "Could not find build target \"" + n_cmd + "\"\n" );
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
	while ( fs::exists( build_path ) ) fs::remove_all( build_path );
	debug::Console->PrintWarning( "Waiting 500ms for the filesystem.\n" );
	std::this_thread::sleep_for( std::chrono::milliseconds( 500 ) );
	// Now, create the directory again
	fs::create_directories( build_path );

	// Copy over the DLLs and the EXE first
	fs::directory_iterator dirend;
	fs::recursive_directory_iterator direndr;
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
			string filename = path->path().filename().string();
			// Copy over all dynamic libraries or executables that we need
			if ( filename.rfind(".exe") != string::npos || filename.rfind(".dll") != string::npos || filename.rfind(".so") != string::npos )
			{
				printf( "copying file: %s\n", filename.c_str() );
				//fs::path target_path = build_path.string() + "/" + path->path().filename();
				//fs::copy_file( fs::path(path->path().root_path().string()), target_path, fs::copy_option::overwrite_if_exists );
				std::ifstream  src( string(m_exe_path) + "/" + filename,					std::ios::binary);
				std::ofstream  dst( build_path.string() + "/" + path->path().filename().string(),	std::ios::binary);
				if ( !src.is_open() ) throw core::NullReferenceException();
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
			string filename = path->path().filename().string();
			// Copy over all dynamic libraries that we need
			if (filename.rfind(".dll") != string::npos || filename.rfind(".so") != string::npos )
			{
				printf( "copying file: %s\n", filename.c_str() );
				std::ifstream  src( "./" + path->path().string(),							std::ios::binary);
				std::ofstream  dst( build_path.string() + "/" + path->path().filename().string(),	std::ios::binary);
				if ( !src.is_open() ) throw core::NullReferenceException();
				dst << src.rdbuf();
			}
		}
		// Check for resource directories
		else if ( fs::is_directory( path->status() ) )
		{
			string filename = path->path().filename().string();
			// If it's a resource directory, step through that shit
			if ( filename.rfind(".res-") != string::npos )
			{
				printf( "found resource directory: %s\n", filename.c_str() );
				// Create the folder
				fs::path base_resource_path ( build_path.string() + "/" + filename );
				fs::create_directories( base_resource_path );
				// Begin a recursive build or copy of all folders and files
				for ( fs::recursive_directory_iterator rpath( path->path() ); rpath != direndr; ++rpath )
				{
					// If it's a folder, make the folder
					if ( fs::is_directory( rpath->status() ) )
					{
						// Make sure it's a valid folder name
						if ( rpath->path().string().find(".svn") == string::npos )
						{
							printf( "found buildable folder: %s\n", rpath->path().filename().u8string().c_str() );
							fs::path new_resource_path ( build_path.string() + "/" + rpath->path().string() );
							fs::create_directories( new_resource_path );
						}
					}
					// If it's a file, check if should copy it over
					else if ( fs::is_regular_file( rpath->status() ) )
					{
						// Make sure it's a valid file
						if ( rpath->path().string().find(".svn") == string::npos )
						{
							//printf( "found file: %s\n", rpath->path().string().c_str() );
							string resource_name = rpath->path().filename().string();
							printf( "found file: %s\n", resource_name.c_str() );

							// Check the file extension
							string resource_extension = StringUtils::ToLower( StringUtils::GetFileExtension( resource_name ) );
							if (   resource_extension == "bpd" || resource_extension == "pad" || resource_extension == "pcf"
								                               || resource_extension == "seq" || resource_extension == "mph"
															   || resource_extension == "pgm" || resource_extension == "vxg"
															   || resource_extension == "ico"
															   || resource_extension == "mcc"
								|| resource_extension == "m01" || resource_extension == "m04"
								|| resource_extension == "txt" || resource_extension == "yml" || resource_extension == "xml"
								|| resource_extension == "lua" || resource_extension == "js"
								|| resource_extension == "ttf" || resource_extension == "otf"
								|| resource_extension == "vert"|| resource_extension == "frag"|| resource_extension == "stub" 
								|| resource_extension == "wav" || resource_extension == "mp3" || resource_extension == "ogg"
								|| resource_extension == "oel"
								|| resource_extension == "" )
							{
								// File needs to be copied over (the folder will already exist)
								printf( "copying file: %s\n", resource_name.c_str() );
								std::ifstream  src( "./" + rpath->path().string(),							std::ios::binary);
								std::ofstream  dst( build_path.string() + "/" + rpath->path().string(),		std::ios::binary);
								if ( !src.is_open() ) throw core::NullReferenceException();
								dst << src.rdbuf();
							}
						}
					}
				}
			}
		}
	}
	debug::Console->PrintWarning( "Build done!\n" );
}
