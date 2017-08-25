
#ifndef _ENGINE_OS_SYSTEM_H_
#define _ENGINE_OS_SYSTEM_H_

namespace System
{
	struct sFileDialogueEntry {
		char extension [8];
		char filetype [56];
	};

	bool GetOpenFilename ( char* nOutFilename, sFileDialogueEntry* nFiletypes, int nFiletypeCount, const char* nInitialDir="", const char* nDialogueTitle="Open File" );
	bool GetSaveFilename ( char* nOutFilename, sFileDialogueEntry* nFiletypes, int nFiletypeCount, const char* nInitialDir="", const char* nDialogueTitle="Save File" );

	void GetWorkingDirectory ( char* nOutDirectory, int nStrlen );
}

#endif//_ENGINE_OS_SYSTEM_H_