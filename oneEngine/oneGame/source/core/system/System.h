#ifndef ENGINE_OS_SYSTEM_H_
#define ENGINE_OS_SYSTEM_H_

#include "core/containers/arstring.h"

namespace System
{
	struct sFileDialogueEntry
	{
		arstring<8> extension;
		arstring<56> filetype;

		sFileDialogueEntry ( void )
			: extension()
			, filetype()
			{}

		sFileDialogueEntry (const char* in_extension, const char* in_filetype)
			: extension(in_extension)
			, filetype(in_filetype)
			{}
	};

	bool GetOpenFilename ( char* nOutFilename, sFileDialogueEntry* nFiletypes, int nFiletypeCount, const char* nInitialDir="", const char* nDialogueTitle="Open File" );
	bool GetSaveFilename ( char* nOutFilename, sFileDialogueEntry* nFiletypes, int nFiletypeCount, const char* nInitialDir="", const char* nDialogueTitle="Save File" );

	void GetWorkingDirectory ( char* nOutDirectory, int nStrlen );
}

#endif//ENGINE_OS_SYSTEM_H_