
#ifndef _ENGINE_COMMON_LUA_SYS_CONSOLE_H_
#define _ENGINE_COMMON_LUA_SYS_CONSOLE_H_

#include <string>
#include "core/types/types.h"

using std::string;

namespace Lua
{
	int con_execLua		( string const& cmd );
	int con_execLuaFile	( string const& file );
	int con_luaReload	( string const& );
};

#endif//_ENGINE_COMMON_LUA_SYS_CONSOLE_H_