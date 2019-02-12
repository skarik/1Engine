#include "TrConverterForwardTo32.h"

#include "core/os.h"
#include "core/debug/console.h"
#include <stdio.h>

bool TrConverterForwardTo32::Convert(const char* inputFilename, const char* outputFilename)
{
	// Run the converter if needed now:
#ifdef _WIN32
	// Create command to run:
	CHAR commandLine [1024] = {};
	snprintf(commandLine, sizeof(commandLine), "dev_tool_oneConverter32.exe \"%s\" \"%s\"", inputFilename, outputFilename);

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
#elif
	debug::Console->PrintError("Cannot convert model: conversion can only be performed on Windows.\n");
#endif

	return false;
}