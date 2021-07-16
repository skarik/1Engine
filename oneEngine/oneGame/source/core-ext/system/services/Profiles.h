//===============================================================================================//
//
//		core-ext/system/services/Profiles.h
//
// Contains system-specific feature interfaces
//
//===============================================================================================//
#ifndef CORE_EXT_SYSTEM_SERVICES_PROFILES_H_
#define CORE_EXT_SYSTEM_SERVICES_PROFILES_H_

#include "core/types/types.h"
#include <string>

namespace core
{
	namespace os
	{
		namespace profiles
		{
			//		GetCurrentProfileName ( ) : Returns unique identifier for the current profile.
			CORE_API std::string GetCurrentProfileName ( void );
		}
	}
}

#endif//CORE_EXT_SYSTEM_SHELL_CLIPBOARD_H_