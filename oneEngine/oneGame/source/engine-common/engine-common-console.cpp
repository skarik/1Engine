
#include "engine-common-console.h"

#include "engine-common/debug/CTimeProfilerUI.h"

//		DebugToggleTimeProfilerUI(...)
// Toggle if time profiler UI is up
int EngineCommon::DebugToggleTimeProfilerUI ( const std::string& )
{
	static CTimeProfilerUI* ui = NULL;
	if ( ui == NULL ) {
		ui = new CTimeProfilerUI();
	}
	else {
		delete_safe(ui);
	}
	return 0;
}

//	CommandClient()
// Performs a debug client command
int EngineCommon::CommandClient ( const std::string& )
{
	return 0;
}

//	CommandClient()
// Performs a debug server command
int EngineCommon::CommandServer ( const std::string& )
{
	return 0;
}