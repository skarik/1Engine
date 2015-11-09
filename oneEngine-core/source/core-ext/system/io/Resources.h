//===============================================================================================//
//	resources.h
//
// Contains definition for Resources class, which should be used for all game-content loading
// Resources stores the all the plugin directories for additional loading
//===============================================================================================//

#ifndef _CORE_RESOURCE_SEARCHER_
#define _CORE_RESOURCE_SEARCHER_

#include <string>
#include <vector>
#include "stdio.h"

#include "core/types/types.h"

namespace Core
{
	class Resources
	{
	private:
		//=========================================//
		// Internal state

		static std::vector<std::string>		m_paths;

		//=========================================//
		// Internal functions

		// Loads up all main paths and plugin paths
		static void FindAllPaths ( void );

	public:
		//=========================================//
		// Public functions

		//	GetAllPaths( )
		// Returns all search paths for files
		CORE_API static const std::vector<std::string>& GetAllPaths ( void );

		//	GetPrimaryResourcePath( )
		// Returns the system "vanilla" search path
		CORE_API static const std::string GetPrimaryResourcePath ( void );

		//	Open( filename, openmode )
		// Find file in the current paths and open. Uses fopen() internally.
		// Will return NULL if cannot find the file.
		CORE_API static FILE* Open ( const std::string& n_filename, const char* n_openmode );
		CORE_API static FILE* Open ( const char* n_filename, const char* n_openmode );
		
		//	PathTo( filename )
		// Finds file and returns string that represents path to the file. Uses fopen() internally.
		// Will return the input filename if cannot find the file.
		CORE_API static std::string PathTo ( const std::string& n_filename );
		CORE_API static std::string PathTo ( const char* n_filename );

		//	Reset( )
		// Clears out the stored paths so they have to be rebuilt
		CORE_API static void Reset ( void );

	};
}

#endif//_CORE_RESOURCE_SEARCHER_