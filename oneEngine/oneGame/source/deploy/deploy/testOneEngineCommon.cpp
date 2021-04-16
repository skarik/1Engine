#ifdef _WIN32

#include "core/types/types.h"
#include "core-ext/core-ext.h"
#include "deploy/unit/unit.h"

// Instance Limiting
#ifdef _ENGINE_RELEASE
#include "deploy/win32/LimitSingleInstance.h"
CLimitSingleInstance g_SingleInstanceObj(TEXT("Global\\{479DFDD7-3051-4a3f-B1C2-6ECCCB2A362D}"));
/*static const GUID <<name>> = 
{ 0x479dfdd7, 0x3051, 0x4a3f, { 0xb1, 0xc2, 0x6e, 0xcc, 0xcb, 0x2a, 0x36, 0x2d } };*/
#endif

// Program entry point
int WINAPI WinMain(	HINSTANCE	hInstance,			// Instance
					HINSTANCE	hPrevInstance,		// Previous Instance
					LPSTR		lpCmdLine,			// Command Line Parameters
					int			nCmdShow)			// Window Show State
{
	// Limit the program to a single instance
	#ifdef _ENGINE_RELEASE
		if ( g_SingleInstanceObj.IsAnotherInstanceRunning() )
		{
			MessageBox( NULL, "Another instance of the application is already running.", "Aborting", 0 );
			return 0;
		}
	#endif

	// Throw exception or crash when the memory heap gets corrupted in order to track down larger errors.
	BOOL f=HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);

	// Call core init CBs
	core::OnApplicationStartup();

	// Call the Unit to run
	int returnCode = Unit::Test_EngineCommon(hInstance,hPrevInstance,lpCmdLine,nCmdShow);

	// Call core free CBs
	core::OnApplicationEnd();

	return returnCode;
}

#endif