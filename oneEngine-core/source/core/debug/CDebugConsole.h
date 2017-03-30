//===============================================================================================//
// onengine/core-ext
//
//	CDebugConsole
//		shitty print debugging class that's lasted since first started with C++
//		(it has pretty colors)
//
//	Author: Joshua Boren
//===============================================================================================//

#ifndef _C_DEBUG_CONSOLE_
#define _C_DEBUG_CONSOLE_

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

		CORE_API static void PrintMessage ( const string& );
		CORE_API static void PrintWarning ( const string& );
		CORE_API static void PrintError ( const string& );

		CORE_API static void DisableOutput ( void );
		CORE_API static void EnableOutput ( void );
	};

	//typedef CDebugConsole DebugConsole;
	CORE_API extern CDebugConsole*	Console;

}

#endif