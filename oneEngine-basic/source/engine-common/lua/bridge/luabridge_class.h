//===============================================================================================//
//		luabridge_class.h
//	Class definition for the LuaBridge_Env, LuaBridge_Class class and usage.
//===============================================================================================//
#ifndef _LUA_BRIDGE_CLASS_H_
#define _LUA_BRIDGE_CLASS_H_

#include "LuaBridge/LuaBridge.h"
/*
//=========================================//
// Includes
//=========================================//
#include <string>
#include <functional>

#include "clua.h"

#include "core/types/types.h"

using std::string;

//=========================================//
// Lua system namespace so we don't litter
//=========================================//
namespace Lua
{
	class LuaBridge_Env;

	template <typename Class>
	class LuaBridge_Class
	{
	public:
		// LuaBridge_Class constructor begins a class in the current lua environment.
		explicit LuaBridge_Class ( string const& name, LuaBridge_Env& environment )
		{
			// push a table to the system
		}

		//	addFunction
		template <typename RetType, typename Arg1>
		LuaBridge_Class& addFunction ( string const& name, RetType (Class::*func) (Arg1) );
		template <typename RetType, typename Arg1, typename Arg2>
		LuaBridge_Class& addFunction ( string const& name, RetType (Class::*func) (Arg1,Arg2) );
		template <typename RetType, typename Arg1, typename Arg2, typename Arg3>
		LuaBridge_Class& addFunction ( string const& name, RetType (Class::*func) (Arg1,Arg2,Arg3) );	
		template <typename RetType, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
		LuaBridge_Class& addFunction ( string const& name, RetType (Class::*func) (Arg1,Arg2,Arg3,Arg4) );	
		template <typename RetType, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
		LuaBridge_Class& addFunction ( string const& name, RetType (Class::*func) (Arg1,Arg2,Arg3,Arg4,Arg5) );	

		//	endClass
		// Closes the current class's table.
		LuaBridge_Env& endClass ( void )
		{
			// end the table definition
			return environment;
		}
	};


	class LuaBridge_Env
	{
	public:
		//	beginClass<type>( name )
		// Create a class in lua with the given name, and using the type as a object to bind to.
		template <typename Class>
		LuaBridge_Class<Class> beginClass ( string const& name )
		{
			return LuaBridge_Class<Class>( name, *this );
		}
	};
	LuaBridge_Env	getGlobalNamespace ( lua_State* );

};
*/
#endif//_LUA_BRIDGE_CLASS_H_