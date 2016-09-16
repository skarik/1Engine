#include "engine-common/lua/CLuaController.h"

#include "after/lua/Lua.h"
#include "after/entities/world/client/CQuestSystem.h"

int lua_quest_start(lua_State*);
int lua_quest_give(lua_State*);

int Lua::LuaRegister_QuestSystem ( lua_State* luaState )
{

	lua_register( luaState, "_quest_give_", lua_quest_give);
	lua_register( luaState, "_quest_start_", lua_quest_start);

	return 0;
}

int lua_quest_start(lua_State* luaState )
{

	return 0;
}

int lua_quest_give(lua_State* )
{

	return 0;
}