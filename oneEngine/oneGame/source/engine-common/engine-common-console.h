
#ifndef _ENGINE_COMMON_ENGINE_COMMON_CONSOLE_H_
#define _ENGINE_COMMON_ENGINE_COMMON_CONSOLE_H_

//===============================================================================================//
// Engine-common-console
// 
// engine-common functionality specific to the console
//===============================================================================================//

#include "core/types/types.h"
#include <string>

namespace EngineCommon
{
	//		DebugToggleTimeProfilerUI(...)
	// Toggle if time profiler UI is up
	ENGCOM_API int DebugToggleTimeProfilerUI ( const std::string& );

};

#endif//_ENGINE_COMMON_ENGINE_COMMON_CONSOLE_H_