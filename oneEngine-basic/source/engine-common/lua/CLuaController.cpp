
#include "CLuaController.h"
#include "core-ext/system/io/Resources.h"

//CLuaController* CLuaController::active	= NULL;
Lua::CLuaController*	Lua::Controller = NULL;

Lua::CLuaController::CLuaController ( void )
	: luaVM( NULL ), inEnvironment(false)
{
	Controller = this;

	InitLua();

	// Run Lua primary init
	RunLuaFile( "init.lua" );

	// Register functions
	/*LuaRegister_GameState( luaVM );
	LuaRegister_Renderer( luaVM );
	LuaRegister_GameObjects( luaVM );
	LuaRegister_QuestSystem( luaVM );*/

	// Run Lua secondary init
	/*RunLuaFile( "sys_engine.lua" );*/
}


Lua::CLuaController::~CLuaController ( void )
{
	FreeLua();

	if ( Controller == this ) {
		Controller = NULL;
	}
}


using namespace Lua;

int		CLuaController::InitLua ( void )
{
	luaVM = luaL_newstate();
	if ( luaVM == NULL )
	{
		//cout << "Error Initializing lua\n";
		return -1;
	}

	luaL_openlibs(luaVM);	// does all the openlib stuff from 4.x
	return 0;
}
int		CLuaController::FreeLua ( void )
{
	lua_close( luaVM );
	luaVM = NULL;

	return 0;
}

//	lua_state* GetState ()
// Returns lua machine.
lua_State*	CLuaController::GetState ( void )
{
	return luaVM;
}

//	RunLuaFile ( Filename )
// Runs the given file.
void	CLuaController::RunLuaFile ( const string& filename, const string& env )
{
	string luaFile = Core::Resources::PathTo("lua/"+filename);
	string workingDir = luaFile.substr( 0, luaFile.find_last_of('/')+1 );

	// Get top of stack
	int top = lua_gettop( luaVM );
	if ( top < 0 ) {
		throw top;
	}
	int s; // s for status
	s = luaL_loadfile( luaVM, luaFile.c_str());		// s:c
	ReportErrors(luaVM, s);
	if ( s == 0 ) // If s == 0, then no error
	{
		if ( env.size() > 0 ) {
			SetEnvironment( env ); // Pushes environment onto stack
			lua_setupvalue( luaVM, -2, 1 ); // pops environment off of stack
		}
		// Push working directory of the file too
		lua_pushstring( luaVM, workingDir.c_str() );
		lua_setglobal( luaVM, "path" );

		s = lua_pcall( luaVM, 0, LUA_MULTRET, 0 );
		ReportErrors(luaVM, s);

		// Get number of returned values and pop them off
		int nresults = lua_gettop( luaVM ) - top;
		PopValid( nresults );
	}
	// Reset environment if needed
	/*if ( env.size() > 0 ) {
		ResetEnvironment(); // Pops environment off the stack
	}*/
}
//	RunLua ( String )
// Runs the given Lua string.
void	CLuaController::RunLua ( const string & cmd )
{
	int s;
	s = luaL_dostring( luaVM,cmd.c_str() );
	ReportErrors(luaVM, s);
}
void	CLuaController::RunLua ( const char *cmd )
{
	int s;
	s = luaL_dostring( luaVM,cmd );
	ReportErrors(luaVM, s);
}

// Reports errors after executing
void	CLuaController::ReportErrors( lua_State *L, int status )
{
	if ( status != 0 ) {
		//std::cerr << "-- " << lua_tostring(L, -1) << std::endl;
		string err = string("lua: ") + lua_tostring(L, -1) + "\n";
		Debug::Console->PrintError( err );
		lua_pop(L, 1); // remove error message
	}
}

void	CLuaController::CallPop ( const string & fnName, const int fnArgNum, ... )
{
	va_list arguments;
	va_start( arguments, fnArgNum );
	int result = Call( fnName, fnArgNum, arguments );
	va_end( arguments );
	PopValid( result );
}
//  Call( FN_NAME, ARG_NUM, ARG0_TYPE, ARG0, ARG1_TYPE, ARG1, ... );
// Calls a Lua function with the given arguments.
int 	CLuaController::Call ( const string & fnName, const int fnArgNum, ... )
{
	va_list arguments;
	va_start( arguments, fnArgNum );
	int result = Call( fnName, fnArgNum, arguments );
	va_end( arguments );
	return result;
}
//  Call( FN_NAME, ARG_NUM, ARG0_TYPE, ARG0, ARG1_TYPE, ARG1, ... );
// Calls a Lua function with the given arguments.
int		CLuaController::Call ( const string & fnName, const int fnArgNum, va_list arguments )
{
	// Get top of stack
	int top = lua_gettop( luaVM );
	// Get global for the function
	if ( !inEnvironment ) {
		lua_getglobal( luaVM, fnName.c_str() );
	}
	else {
		lua_getfield( luaVM, -1, fnName.c_str() );
	}
	// Check if the global is a function
	if( lua_isfunction(luaVM,-1) )
	{
		// Push argument list
		//va_list arguments;
		//va_start( arguments, fnArgNum );
		for ( int i = 0; i < fnArgNum; ++i )
		{
			int type = va_arg( arguments, int );
			if ( type == FN_INT ) {
				int var = va_arg( arguments, int );
				lua_pushinteger( luaVM, var );
			}
			else if ( type == FN_FLOAT ) {
				float var = va_arg( arguments, float );
				printf( "%lf\n", (double)var );
				lua_pushnumber( luaVM, (double)var );
			}
			else if ( type == FN_DOUBLE ) {
				double var = va_arg( arguments, double );
				lua_pushnumber( luaVM, var );
			}
			else if ( type == FN_STRING ) {
				char* var = va_arg( arguments, char* );
				lua_pushstring( luaVM, var );
			}
			else if ( type == FN_USERDATA ) {
				void* var = va_arg( arguments, void* );
				lua_pushlightuserdata( luaVM, var );
			}
			else {
				Debug::Console->PrintError( "Invalid arguments to Lua::Call\n" );
			}
		}
		//va_end( arguments );

		// Run the function
		/*if ( lua_pcall( luaVM, fnArgNum, 1, 0 ) == 0 ) {
			ReportErrors( luaVM, 0 );
		}*/
		int s = lua_pcall( luaVM, fnArgNum, LUA_MULTRET, 0 );
		//int s = lua_pcall( luaVM, fnArgNum, 1, 0 );
		ReportErrors( luaVM, s );
		// Get number of returned values
		int nresults = lua_gettop( luaVM ) - top;
		return nresults;
	}
	else
	{
		// Print that function not exists
		if ( lua_isnil( luaVM, -1 ) ) {
			Debug::Console->PrintError( string("variable \"")+fnName+"\" is nil\n" );
		}
		else {
			Debug::Console->PrintError( string("variable \"")+fnName+"\" is not a function\n" );
		}
		lua_pop( luaVM, 1 );
		return 0;
	}
}
//  PopValid ( Integer )
// If the input argument is valid, attempts to pop that many values off the list
void	CLuaController::PopValid ( const int popCount )
{
	if ( popCount > 0 ) {
		lua_pop( luaVM, 1 );
	}
	else {
		if ( popCount != 0 ) {
			Debug::Console->PrintWarning( "Invalid pop count passed to PopValid().\n" );
		}
	}
}

//	SetEnvironment ( Environment Name, [Inherit From Global] )
// Set the Lua environment to the given environment. The second argument indicated whether
// or not to inherit from the global environment. It is by default true.
void	CLuaController::SetEnvironment ( const string & nenv_name, bool ninherit )
{
	lua_getglobal( luaVM, nenv_name.c_str() );				// s:env
	if ( lua_isnil( luaVM, -1 ) ) // it doesn't exist
	{
		lua_pop( luaVM, 1 );								// s:
		lua_newtable( luaVM );								// s: table

		lua_newtable( luaVM );								// s: mt table
		lua_getglobal( luaVM, "_G" );						// s: global mt table
		lua_setfield( luaVM, -2, "__index" );				// s: mt table
		lua_setmetatable( luaVM, -2 );						// s: table

		lua_setglobal( luaVM, nenv_name.c_str() );			// s:
		
		lua_getglobal( luaVM, nenv_name.c_str() );			// s: env
	}

	lua_getglobal( luaVM, "_G" );      // s: global env
	lua_pushvalue( luaVM, -2 );        // s: env global env
	lua_setfield( luaVM, -2, "_ENV" ); // s: global env
	lua_pop( luaVM,1 );				   // s: env

	inEnvironment = true;
}
//	ResetEnvironment
// Reset the Lua environment to the default starting environment.
void	CLuaController::ResetEnvironment ( void )
{
	// should still have main table on stack
	lua_pop( luaVM, 1 );
		
	lua_getglobal( luaVM, "_G" );		// s: _G
	lua_pushvalue( luaVM, -1 );			// s: _G _G
	lua_setfield( luaVM, -2, "_ENV" );	// s: _G
	lua_pop( luaVM, 1 );				// s: 
		
	inEnvironment = false;
}


//	LoadLuaFile
// Loads a lua file into the given environment. Stores file into the internal table that
// keeps track of files.
void	CLuaController::LoadLuaFile ( const string & filename, const string& env, const string& obj )
{
	bool exists = false;
	for ( uint i = 0; i < luaFileList.size(); ++i ) {
		// Search for exact match on the name
		if ( (std::get<0>(luaFileList[i]) == filename) && (std::get<1>(luaFileList[i]) == env) )
		{
			std::get<2>(luaFileList[i]) += 1; // Increment reference count
			exists = true;
			break;
		}
	}
	if ( !exists )
	{
		luaFileList.push_back( std::make_tuple(filename,env,1) );
		if ( env.size() == 0 ) {
			RunLuaFile( filename );
		}
		else {
			RunLuaFile( filename, env );
		}
	}
	/*if (( obj.size() != 0 ) && ( obj != env )) {
		//RunLua( obj+" = {};" );
		//RunLua( "setmetatable("+obj+","+env+");" );
		//RunLua( "if ("+obj+"==nil) then "+obj+" = {}; setmetatable("+obj+","+env+"); end" );
		lua_getglobal( luaVM, obj.c_str() );
		if ( lua_isnil( luaVM, -1 ) )
		{
			lua_pop( luaVM, 1 );

			lua_newtable( luaVM );					// s: obj

			lua_newtable( luaVM );	// s: mt obj
			lua_getglobal( luaVM, env.c_str() ); // s: env mt obj
			lua_setfield( luaVM, -2, "__index" ); // s: mt obj
			lua_setmetatable( luaVM, -2 ); // s: obj

			lua_setglobal( luaVM, obj.c_str() );	// s: 
		}
		else {
			lua_pop( luaVM, -1 );
		}	
	}*/
}
//	FreeLuaFile
// Decrements the reference on the lua code.
void	CLuaController::FreeLuaFile ( const string & filename, const string& env )
{
	auto it = luaFileList.begin();
	while ( it != luaFileList.end() )
	{
		if ( (std::get<0>(*it) == filename) && (std::get<1>(*it) == env) )
		{
			std::get<2>(*it) -= 1;
			if ( std::get<2>(*it) <= 0 ) {
				it = luaFileList.erase(it);
			}
			// Found target, leaving
			return;
		}
		else {
			++it;
		}
	}
}

// Reloads registered Lua files.
void	CLuaController::ReloadLuaFiles ( void )
{
	for ( auto it = luaFileList.begin(); it != luaFileList.end(); ++it )
	{
		if ( std::get<1>(*it).size() == 0 ) {
			//Lua::Controller->ResetEnvironment();
			Lua::Controller->RunLuaFile( std::get<0>(*it) );
		}
		else {
			//Lua::Controller->SetEnvironment( std::get<1>(*it) );
			Lua::Controller->RunLuaFile( std::get<0>(*it), std::get<1>(*it) );
		}
	}
	//Lua::Controller->ResetEnvironment();
}



//  TableGetNumber ( const char* key )
//  TableGetNumber ( const int key )
// Looks for number at table in stack at position -1 with given index.
// Preserves stack.
double CLuaController::TableGetNumber ( const char* key ) {
	lua_pushstring(luaVM,key);
	lua_gettable(luaVM,-2);
	double value = lua_tonumber(luaVM,-1);
	lua_pop(luaVM,1);
	return value;
}
double CLuaController::TableGetNumber ( const int key ) {
	lua_pushinteger(luaVM,key);
	lua_gettable(luaVM,-2);
	double value = lua_tonumber(luaVM,-1);
	lua_pop(luaVM,1);
	return value;
}