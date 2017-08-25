
#include "core/debug/CDebugConsole.h"
#include "Resources.h"
#include <filesystem>
namespace fs = std::tr2::sys;

namespace core
{
	//=========================================//
	// Statics

	std::vector<std::string> Resources::m_paths;

	//=========================================//
	// Loads up all main paths and plugin paths

	void Resources::FindAllPaths ( void )
	{
		if ( m_paths.empty() )
		{
			m_paths.push_back( "./.res-0/" );
			m_paths.push_back( "./.res-1/" );

			// Now enumerate all folders under the "./addons/" directory
			fs::path	targetDir ( "./addons/" ); 
			fs::directory_iterator end_itr;
			for ( fs::directory_iterator dir_itr ( targetDir ); dir_itr != end_itr; ++dir_itr )
			{
				if ( fs::is_directory( dir_itr->status() ) )
				{
					// For every file, take the file name
#if _MSC_VER <= 1700
					std::string currentName = dir_itr->path().filename();
#else
					std::string currentName = dir_itr->path().filename().string();
#endif

					// Add the path to the front of the list
					m_paths.insert( m_paths.begin(), "./addons/" + currentName );
				}
			}
		}
	}

	//=========================================//
	// Public functions

	//	GetAllPaths () : Returns all search paths for files
	const std::vector<std::string>& Resources::GetAllPaths ( void )
	{
		FindAllPaths();
		return m_paths;
	}
	//	GetPrimaryResourcePath( )
	// Returns the system "vanilla" search path
	const std::string Resources::GetPrimaryResourcePath ( void )
	{
		return m_paths[m_paths.size()-2];
	}

	//	Open( filename, openmode ) : Find file in the current paths and open.
	FILE* Resources::Open ( const std::string& n_filename, const char* n_openmode )
	{
		return Open( n_filename.c_str(), n_openmode );
	}
	FILE* Resources::Open ( const char* n_filename, const char* n_openmode )
	{
		// Load up the paths first
		FindAllPaths();

		// Convert to string
		std::string filename = n_filename;

		// Check the input for ".res" and remove if needed
		if ( std::string(filename).find(".res/") != std::string::npos )
		{
			filename = filename.replace( 0, 5, "" );
			debug::Console->PrintWarning( filename + " was opened using old method!\n" );
		}

		// Loop through the paths and return the first result
		for ( auto itr_path = m_paths.begin(); itr_path != m_paths.end(); ++itr_path )
		{
			std::string currentFilename = *itr_path + filename;
			FILE* file = fopen( currentFilename.c_str(), n_openmode );
			if ( file ) {
				return file;
			}
		}
		
		return NULL;
	}

	//	PathTo( filename ) : Finds file and returns string that represents path to the file.
	std::string Resources::PathTo ( const std::string& n_filename )
	{
		return PathTo( n_filename.c_str() );
	}
	std::string Resources::PathTo ( const char* n_filename )
	{
		// Load up the paths first
		FindAllPaths();

		// Convert to string
		std::string filename = n_filename;

		// Check the input for ".res" and remove if needed
		if ( std::string(filename).find(".res/") != std::string::npos )
		{
			filename = filename.replace( 0, 5, "" );
			debug::Console->PrintWarning( filename + " was opened using old method!\n" );
		}

		// Loop through the paths and return the first result
		for ( auto itr_path = m_paths.begin(); itr_path != m_paths.end(); ++itr_path )
		{
			std::string currentFilename = *itr_path + filename;
			FILE* file = fopen( currentFilename.c_str(), "r" );
			if ( file != NULL )
			{
				fclose( file ); // Close file beforehand
				return currentFilename;
			}
		}
		return n_filename;
	}

	//	Exists( filename ) : Find file in the current paths.
	bool Resources::Exists ( const std::string& n_filename )
	{
		return Exists( n_filename.c_str() );
	}
	bool Resources::Exists ( const char* n_filename )
	{
		FILE* fp = Open(n_filename, "r");
		if (fp != NULL)
		{
			fclose(fp);
			return true;
		}
		return false;
	}

	//	MakePathTo( filename ) : Finds file and creates a string that represents path to the file.
	bool Resources::MakePathTo ( const std::string& n_filename, std::string& o_filename )
	{
		return MakePathTo( n_filename.c_str(), o_filename );
	}
	bool Resources::MakePathTo ( const char* n_filename, std::string& o_filename )
	{
		// Load up the paths first
		FindAllPaths();

		// Convert to string
		std::string filename = n_filename;

		// Check the input for ".res" and remove if needed
		if ( std::string(filename).find(".res/") != std::string::npos )
		{
			filename = filename.replace( 0, 5, "" );
			debug::Console->PrintWarning( filename + " was opened using old method!\n" );
		}

		// Loop through the paths and return the first result
		for ( auto itr_path = m_paths.begin(); itr_path != m_paths.end(); ++itr_path )
		{
			std::string currentFilename = *itr_path + filename;
			FILE* file = fopen( currentFilename.c_str(), "r" );
			if ( file != NULL )
			{
				fclose( file ); // Close file beforehand
				o_filename = currentFilename;
				return true;
			}
		}
		return false;
	}


	//	Reset( ) : Clears out the stored paths so they have to be rebuilt
	void Resources::Reset ( void )
	{
		m_paths.clear();
	}
}