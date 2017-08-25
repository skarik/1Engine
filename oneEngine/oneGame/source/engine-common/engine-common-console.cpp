
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