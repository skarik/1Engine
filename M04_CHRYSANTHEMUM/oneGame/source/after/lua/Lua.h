
#ifndef _AFTER_LUA_HOOKS_H_
#define _AFTER_LUA_HOOKS_H_

#include "engine-common/lua/CLuaController.h"

namespace Lua
{
	int LuaRegister_AFTERObjects ( lua_State* luaState );
	int LuaRegister_QuestSystem ( lua_State* luaState );
}

#endif//_AFTER_LUA_HOOKS_H_