
#include "gmsceneSystemBuilder.h"

// Include game settings
#include "core/settings/CGameSettings.h"
// Include debug console
#include "core/debug/CDebugConsole.h"
#include "engine/utils/CDeveloperConsole.h"
#include "engine-common/utils/CDeveloperConsoleUI.h"
// Include camera for rendering
#include "renderer/Camera/CCamera.h"

// Include OSF IO
#include "core-ext/system/io/mccosf.h"

void gmsceneSystemBuilder::LoadScene ( void )
{
	Debug::Console->PrintMessage( "Loading scene (System Builder).\n" );

	// Create the console (basically our script link)
	CDeveloperConsoleUI* devConsole = new CDeveloperConsoleUI();
	CDeveloperCursor* devCursor = new CDeveloperCursor();

	// Add the builder command
	Engine::Console->AddConsoleFunc( "build", EngineCommon::BuildToTarget );

	// Print a prompt
	Debug::Console->PrintMessage( "You are running a debug build of oneEngine.\n" );
	Debug::Console->PrintMessage( "Please enter a build to create, or \"scene default\" to go back.\n" );
}

int EngineCommon::BuildToTarget ( const std::string& n_cmd )
{
	// Using the OSF system, load up /system/buildtargets.txt
	// Need to locate the matching object
	FILE* fp_loader = fopen( ".res/system/buildtargets.txt", "rb" );
	COSF_Loader osf_loader ( fp_loader );

	// Read in entries until we hit the end or until we hit a match
	mccOSF_entry_info_t entry;
	osf_loader.GetNext( entry );
	while ( entry.type != mccOSF_entrytype_enum::MCCOSF_ENTRY_EOF )
	{
		if ( strcmp( entry.name, n_cmd.c_str() ) == 0 )
		{
			// Found it, load this one

			// Need to leave
			fclose( fp_loader );
			return 0;
		}
		// Read next
		osf_loader.GetNext( entry );
	}
	// Here? We could not find it. Try again later.
	Debug::Console->PrintWarning( "Could not find build target \"" + n_cmd + "\"\n" );
	return 0;
}