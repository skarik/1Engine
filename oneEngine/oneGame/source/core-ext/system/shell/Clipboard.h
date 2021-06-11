//===============================================================================================//
//
//		core-ext/system/shell/Clipboard.h
//
// Contains shell-specific feature interfaces
//
//===============================================================================================//
#ifndef CORE_EXT_SYSTEM_SHELL_CLIPBOARD_H_
#define CORE_EXT_SYSTEM_SHELL_CLIPBOARD_H_

#include "core/types/types.h"
#include <string>

namespace core
{
	namespace shell
	{
		namespace clipboard
		{
			//		ContainsString ( ) : Does the clipboard contain a string?
			CORE_API bool ContainsString ( void );
			//		GetString ( ) : Get current string in clipboard
			CORE_API std::string GetString ( void );
			//		SetString ( str ) : Set current string in clipboard
			CORE_API void SetString ( const char* str );
		}
	}
}

#endif//CORE_EXT_SYSTEM_SHELL_CLIPBOARD_H_