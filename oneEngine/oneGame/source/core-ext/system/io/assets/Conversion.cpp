#include "core/debug/Console.h"
#include "core/system/io/FileUtils.h"
#include "core/utils/string.h"
#include "core/os.h"

#include "core-ext/system/io/assets/Conversion.h"

namespace core
{
	//=========================================//
	// Statics

	bool Converter::m_initialized = false;
	bool Converter::m_haveConverter = false;
	bool Converter::m_haveConverter32 = false;
	arstring256 Converter::m_pathConverter = "";
	arstring256 Converter::m_pathConverter32 = "";

	//=========================================//
	// Internal functions

	// Checks if converters can be found
	void Converter::FindConverters ( void )
	{
		if (!m_initialized)
		{
			m_pathConverter		= ".tools/dev_tool_oneConverter.exe";
			m_pathConverter32	= ".tools/dev_tool_oneConverter32.exe";

			m_haveConverter		= IO::FileExists(m_pathConverter);
			m_haveConverter32	= IO::FileExists(m_pathConverter32);

			if (!m_haveConverter)
			{
				/// @todo: This is also done in gmSceneSystemBuilder
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
				
				m_pathConverter		= m_exe_path;
				m_pathConverter		+= "/dev_tool_oneConverter.exe";
				m_haveConverter		= IO::FileExists(m_pathConverter);
			}

			m_initialized = true;
		}
	}

	//=========================================//
	// Public functions

	//	ConvertFile( output )
	// Converts a file, automatically generating the output filename.
	bool Converter::ConvertFile ( const char* n_filename )
	{
		FindConverters();

		// Parse input filename
		arstring256 l_inputFilename (n_filename);
		arstring256 l_inputFileext (l_inputFilename);
		core::utils::string::ToFileExtension(l_inputFileext, 256);
		core::utils::string::ToLower(l_inputFileext, 256);

		// Generate the output filename
		arstring256 l_outputFilename (n_filename);
		core::utils::string::ToFileStemLeaf(l_outputFilename, 256);
		arstring256 l_outputFileext ("");
		// Search for valid output
		for (int i = 0; i < sizeof(core::converter::g_convertableTable) / sizeof(core::converter::arConversionEntry); ++i)
		{
			if (l_inputFileext.compare(core::converter::g_convertableTable[i].source))
			{
				l_outputFileext = core::converter::g_convertableTable[i].target;
				l_outputFilename = strncat(l_outputFilename, ".", 2);
				l_outputFilename = strncat(l_outputFilename, l_outputFileext, 256);
				break;
			}
		}

		// Have we had valid output?
		if (l_outputFileext[0] != 0)
		{
			// Convert the file:
			return ConvertFile(l_inputFilename, l_outputFilename);
		}
		else
		{
			// Print unrecognized filetype!
			debug::Console->PrintError( "Unrecognized filetype!\n" );
			return false;
		}
	}
	//	ConvertFile( input, output )
	// Converts a file to given destination filename.
	bool Converter::ConvertFile ( const char* n_filename, const char* n_outputname )
	{
		FindConverters();
		
		// Run the converter if needed now:
		if (m_haveConverter)
		{
#		ifdef _WIN32
			// Create command to run:
			CHAR commandLine [1024] = {};
			snprintf(commandLine, sizeof(commandLine), "\"%s\" \"%s\" \"%s\"", m_pathConverter.c_str(), n_filename, n_outputname);

			// Set up process
			STARTUPINFO startupInfo = {};
			PROCESS_INFORMATION procInfo = {};
			startupInfo.cb = sizeof(STARTUPINFO);
			// Create process
			BOOL r = CreateProcess( NULL, commandLine, NULL, NULL, false, NORMAL_PRIORITY_CLASS, NULL, NULL, &startupInfo, &procInfo );

			if (!r)
			{	// Could not find converter?
				debug::Console->PrintError( "Could not start up the converter!\n" );
			}
			else
			{
				// Wait for it to finish conversion:
				debug::Console->PrintMessage( "Waiting on conversion...\n" );
				WaitForSingleObject( procInfo.hProcess, INFINITE );

				// Close process and thread handles. 
				CloseHandle( procInfo.hProcess );
				CloseHandle( procInfo.hThread );
				
				// Success!
				return true;
			}
#		elif
			debug::Console->PrintError("Cannot convert model: conversion can only be performed on Windows.\n");
#		endif
		}

		return false;
	}

}