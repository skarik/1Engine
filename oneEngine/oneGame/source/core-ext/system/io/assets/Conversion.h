//===============================================================================================//
//
//		resources.h
//
// Contains definition for Resources class, which should be used for all game-content loading
// Resources stores the all the plugin directories for additional loading
//
//===============================================================================================//
#ifndef CORE_EXT_RESOURCE_CONVERSION_H_
#define CORE_EXT_RESOURCE_CONVERSION_H_

#include <string>
#include <vector>
#include "stdio.h"

#include "core/types/types.h"

namespace core
{
	namespace converter
	{
		static const char* kEngineModelFileExtension = "mpd";
		static const char* kEngineImageFileExtension = "bpd";

		struct arConversionEntry
		{
			const char source [8];
			const char* const target;
		};

		// Global table providing list of convertable filetypes.
		// Used by the converter interface
		static const arConversionEntry g_convertableTable [] =
		{
			{"fbx",  kEngineModelFileExtension},
			{"gltf", kEngineModelFileExtension},
			{"glb",  kEngineModelFileExtension},

			{"gal",  kEngineImageFileExtension},
			{"png",  kEngineImageFileExtension},
			{"jpg",  kEngineImageFileExtension},
			{"jpeg", kEngineImageFileExtension},
			{"tga",  kEngineImageFileExtension},
			{"gif",  kEngineImageFileExtension},
		};
	}

	class Converter
	{
	private:
		//=========================================//
		// Internal state

		static bool m_initialized;
		static bool m_haveConverter;
		static bool m_haveConverter32;

		//=========================================//
		// Internal functions

		// Loads up all main paths and plugin paths
		static void FindConverters ( void );

	public:
		//=========================================//
		// Public constants



		//=========================================//
		// Public functions

		//	ConvertFile( output )
		// Converts a file, automatically generating the output filename.
		CORE_API static bool ConvertFile ( const char* n_filename );
		//	ConvertFile( input, output )
		// Converts a file to given destination filename.
		CORE_API static bool ConvertFile ( const char* n_filename, const char* n_outputname );

	};
}

#endif//CORE_EXT_RESOURCE_CONVERSION_H_