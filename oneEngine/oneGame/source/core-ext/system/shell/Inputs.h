//===============================================================================================//
//
//		core-ext/system/shell/Inputs.h
//
// Contains shell-specific feature interfaces
//
//===============================================================================================//
#ifndef CORE_EXT_SYSTEM_SHELL_INPUTS_H
#define CORE_EXT_SYSTEM_SHELL_INPUTS_H

#include "core/types/types.h"

namespace core
{
	namespace shell
	{
		//	GetDoubleClickInterval() : Returns the make time in seconds the OS recognizes a double-click.
		CORE_API const float	GetDoubleClickInterval ( void );

		//	SetSystemMousePosition(position) : Sets the system mouse position
		CORE_API void			SetSystemMousePosition ( const int x, const int y );
	}
}

#endif//CORE_EXT_SYSTEM_SHELL_INPUTS_H