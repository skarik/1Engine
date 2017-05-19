
#include "CLuaController.h"

#include "engine/state/CGameState.h"
#include "engine/behavior/CGameBehavior.h"
#include "engine/behavior/CGameObject.h"


int lua_gs_FindBehavior	( lua_State *luaState );
int lua_gs_DeleteBehavior	( lua_State* luaState );


int lua_go_GetPosition	( lua_State* luaState );
int lua_go_SetPosition	( lua_State* luaState );

int Lua::Register_GameState ( lua_State* luaState )
{
	lua_register( luaState,	"_gs_FindBehavior",			lua_gs_FindBehavior );
	lua_register( luaState,	"_gs_DeleteBehavior",		lua_gs_DeleteBehavior );

	lua_register( luaState,	"_go_GetPosition",			lua_go_GetPosition );
	lua_register( luaState,	"_go_SetPosition",			lua_go_SetPosition );

	return 0;
}


int lua_gs_FindBehavior ( lua_State *luaState )
{
	// Load string from Lua stack
	const char* behaviorName = lua_tolstring( luaState, 1, NULL );

	// Search for target
	CGameBehavior* target = CGameState::Active()->FindFirstObjectWithName( string(behaviorName) );

	// Push back result to stack
	if ( target == NULL ) {
		printf( "lua_gs_FindBehavior: object was null\n" );
		lua_pushnil(luaState);
	}
	else {
		lua_pushlightuserdata(luaState,target);
	}
	return 1; // Returns 1 value
}
int lua_gs_DeleteBehavior ( lua_State *luaState )
{
	// Load pointer from Lua stack
	CGameBehavior* target = (CGameObject*) lua_touserdata( luaState, 1 );

	// Push back result to stack
	if ( target == NULL ) {
		printf( "lua_gs_DeleteBehavior: object was null\n" );
		lua_pushnil(luaState);
	}
	else {
		target->DeleteObject( target );

		lua_pushboolean(luaState,true);
	}
	return 1; // Returns 1 value
}

int lua_go_GetPosition	( lua_State* luaState )
{
	// Load pointer from Lua stack
	CGameObject* target = (CGameObject*) lua_touserdata( luaState, 1 );
	if ( target )
	{
		lua_pushnumber( luaState, target->transform.world.position.x );
		lua_pushnumber( luaState, target->transform.world.position.y );
		lua_pushnumber( luaState, target->transform.world.position.z );
		return 3; // Returns 3 values
	}
	else
	{
		printf( "lua_go_GetPosition: object was null\n" );
		lua_pushnil( luaState );
		return 1;
	}
}
int lua_go_SetPosition	( lua_State* luaState )
{
	// Load pointer from Lua stack
	CGameObject* target = (CGameObject*) lua_touserdata( luaState, 1 );
	if ( target )
	{
		target->transform.world.position.x = (Real) lua_tonumber( luaState, 2 );
		target->transform.world.position.y = (Real) lua_tonumber( luaState, 3 );
		target->transform.world.position.z = (Real) lua_tonumber( luaState, 4 );
	}
	else {
		printf( "lua_go_SetPosition: object was null\n" );
	}
	return 0; // Returns no value
}