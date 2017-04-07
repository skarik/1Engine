//===============================================================================================//
//	core-ext/system/shell/Status.h
//
// Contains shell-specific feature interfaces
//===============================================================================================//

#ifndef _CORE_EXT_SYSTEM_SHELL_STATUS_H_
#define _CORE_EXT_SYSTEM_SHELL_STATUS_H_

#include "core/types/types.h"

namespace core
{
	namespace shell
	{
		enum ETaskbarState_t
		{
			// Stops displaying progress and returns display to normal state
			TBP_NO_PROGRESS,
			TBP_INDETERMINATE,
			TBP_NORMAL,
			TBP_ERROR,
			TBP_PAUSED,
		};

		//		SetTaskbarProgressHandle ( ) : changes default taskbar handle
		CORE_API void SetTaskbarProgressHandle ( intptr_t shellhandle );
		//		SetTaskbarProgressState ( ) : changes current taskbar state
		CORE_API void SetTaskbarProgressState ( intptr_t shellhandle, const ETaskbarState_t state );
		//		SetTaskbarProgressValue ( ) : changes current taskbar completion value
		CORE_API void SetTaskbarProgressValue ( intptr_t shellhandle, const uint64_t& ullcurrent, const uint64_t& ulltotal );

		//		FlashTray ( ) : flashes the tray icon a given amount of times
		CORE_API void FlashTray ( intptr_t shellhandle, const uint32_t flashcount );
	}
}

#endif//_CORE_EXT_SYSTEM_SHELL_STATUS_H_