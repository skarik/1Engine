
#include "LuaSys.h"
#include "Lua_Console.h"

int Lua::con_execLua ( string const& cmd )
{
	Lua::Controller->RunLua( cmd );
	return 0;
}
int Lua::con_execLuaFile ( string const& file )
{
	Lua::Controller->RunLuaFile( file );
	return 0;
}
int Lua::con_luaReload ( string const& )
{
	Lua::Controller->ReloadLuaFiles();
	return 0;
}