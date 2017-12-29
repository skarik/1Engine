//=onengine/core-ext=============================================================================//
//
//		CDebugConsole - Ultra basic print-statment debugging class.
//
// Used for print statement debugging. It has pretty colors.
// Code is dated to before any team member knew how to program C++.
//
//===============================================================================================//
#ifndef C_DEBUG_CONSOLE_
#define C_DEBUG_CONSOLE_

#include "core/types/types.h"

// System Specific Includes
#ifdef _WIN32
	#include <stdio.h>
	#include <fcntl.h>
	#include <io.h>
	#include <iostream>
	#include <fstream>
#endif
#ifdef __linux

#endif

// General Includes
#include <string>
using std::string;

namespace debug
{
	// Class Definition
	class CDebugConsole
	{
	private: // Cannot instantiate
		CDebugConsole ( void ) {}
		CDebugConsole ( const CDebugConsole& ) {}
		~CDebugConsole ( void ) {}

		static bool	bOutputEnabled;
	public:
		CORE_API static void Init ( void );
		CORE_API static void Free ( void );

		CORE_API static void PrintMessage ( const char* fmt, ... ); 
		CORE_API static void PrintMessage ( const std::string& str ); 
		CORE_API static void PrintWarning ( const char* fmt, ... );
		CORE_API static void PrintWarning ( const std::string& str ); 
		CORE_API static void PrintError ( const char* fmt, ... );
		CORE_API static void PrintError ( const std::string& str ); 

		CORE_API static void DisableOutput ( void );
		CORE_API static void EnableOutput ( void );
	};

	CORE_API extern CDebugConsole*	Console;
}

#endif//C_DEBUG_CONSOLE_