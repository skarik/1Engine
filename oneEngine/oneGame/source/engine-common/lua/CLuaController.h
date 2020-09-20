//===============================================================================================//
// Lua Controller
// Based on Lua 5.2
//===============================================================================================//
#ifndef _C_LUA_CONTROLLER_H_
#define _C_LUA_CONTROLLER_H_

//=========================================//
// Includes
//=========================================//
#include <stdio.h>
#include <string>
#include <iostream>
#include <cstdarg>
#include <vector>
#include <tuple>

#include "clua.h"

#include "core/types/types.h"
#include "core/debug/Console.h"

using std::string;

//=========================================//
// Lua system namespace
//=========================================//
namespace Lua
{
	// Define Lua-side constants
	const int FN_INT	= 0;
	const int FN_FLOAT	= 1;
	const int FN_DOUBLE	= 2;
	const int FN_STRING	= 3;
	const int FN_USERDATA = 4;
	
//#define LUA_FN_SIGNATURE ( a ) int a ( lua_State *luaState )

	class CLuaController
	{
	public:
		ENGCOM_API explicit		CLuaController ( void );
		ENGCOM_API				~CLuaController ( void );

		//	RunLuaFile ( Filename )
		// Runs the given file.
		ENGCOM_API void		RunLuaFile ( const string& filename, const string& env="" );
		//	RunLua ( String )
		// Runs the given Lua string.
		ENGCOM_API void		RunLua ( const string & );
		ENGCOM_API void		RunLua ( const char* );
		//  Call( FN_NAME, ARG_NUM, ARG0_TYPE, ARG0, ARG1_TYPE, ARG1, ... );
		// Calls a Lua function with the given arguments.
		ENGCOM_API int		Call ( const string &, const int, ... );
		//  CallPop( FN_NAME, ARG_NUM, ARG0_TYPE, ARG0, ARG1_TYPE, ARG1, ... );
		// Calls a Lua function with the given arguments, discarding any returned values.
		ENGCOM_API void		CallPop ( const string &, const int, ... ); 
		//  Call( FN_NAME, ARG_NUM, ARG0_TYPE, ARG0, ARG1_TYPE, ARG1, ... );
		// Calls a Lua function with the given arguments.
		ENGCOM_API int		Call ( const string &, const int, va_list );
		//  TableGetNumber ( const char* key )
		//  TableGetNumber ( const int key )
		// Looks for number at table in stack at position -1 with given index.
		// Preserves stack.
		ENGCOM_API double		TableGetNumber ( const char* key );
		ENGCOM_API double		TableGetNumber ( const int key );

		//	lua_state* GetState ()
		// Returns lua machine.
		ENGCOM_API lua_State*	GetState ( void );
		//	lua_state* automatic conversion.
		// implicitly act as a lua_State pointer
		FORCE_INLINE operator lua_State*() {
			return luaVM;
		}

		//  PopValid ( Integer )
		// If the input argument is valid, attempts to pop that many values off the list
		ENGCOM_API void		PopValid ( const int popCount );
		//	SetEnvironment ( Environment Name, [Inherit From Global] )
		// Set the Lua environment to the given environment. The second argument indicated whether
		// or not to inherit from the global environment.
		ENGCOM_API void		SetEnvironment ( const string &, bool = true );
		//	ResetEnvironment
		// Reset the Lua environment to the default starting environment.
		ENGCOM_API void		ResetEnvironment ( void );

		//	LoadLuaFile
		// Loads a lua file into the given environment. Stores file into the internal table that
		// keeps track of files. Defaults to the global environment.
		ENGCOM_API void		LoadLuaFile ( const string & filename, const string& env="", const string& obj="" );
		//	FreeLuaFile
		// Decrements the reference on the lua code.
		ENGCOM_API void		FreeLuaFile ( const string & filename, const string& env="" );
		// Reloads registered Lua files.
		ENGCOM_API void		ReloadLuaFiles ( void );

		//  ReportErrors
		// Reports lua errors given in a state and a status.
		ENGCOM_API void		ReportErrors( lua_State *L, int status );
	private:
		ENGCOM_API int		InitLua ( void );
		ENGCOM_API int		FreeLua ( void );

	private:
		lua_State*		luaVM;

		typedef std::tuple<string,string,int> luaFileTuple;
		std::vector<luaFileTuple>	luaFileList;
		
		bool			inEnvironment;
	};
	// Pointer to current Lua controller
	ENGCOM_API extern CLuaController* Controller;

	// Lua functions
	int	Register_GameState	( lua_State* luaState );
	int Register_Renderer	( lua_State* luaState );
	//int Register_GameObjects( lua_State* luaState );
	//int LuaRegister_QuestSystem ( lua_State* luaState );

	//===============================================================================================//
	// Below are inline function definitions for the LuaController class
	//===============================================================================================//

	// As the controller is now a wrapper for scripting, it no longer has inline functions available
}

#endif