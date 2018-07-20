//===============================================================================================//
//
//		core-ext/system/shell/Message.h
//
// Contains shell-specific message popups.
//
//===============================================================================================//
#ifndef CORE_EXT_SYSTEM_SHELL_MESSAGE_H_
#define CORE_EXT_SYSTEM_SHELL_MESSAGE_H_

#include "core/types/types.h"

namespace core
{
	namespace shell
	{
		//	ShowErrorMessage(string) : Shows an error message, creating a message window with the OS.
		CORE_API void ShowErrorMessage ( const char* fmt, ... );
	}
}

#endif//CORE_EXT_SYSTEM_SHELL_MESSAGE_H_