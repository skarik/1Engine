
#include "CLuaBehavior.h"
#include "CLuaController.h"

CLuaBehavior::CLuaBehavior ( const char* entityName, const char* packageName )
	: CGameBehavior(), m_enableKey(0)
{
	// Check length of inputs
	if ( strlen( entityName ) > 127 ) {
		throw Core::OutOfMemoryException();
	}
	if ( strlen( packageName ) > 95 ) {
		throw Core::OutOfMemoryException();
	}
	// Set package names for loading in
	m_entityName	= entityName;
	m_packageName	= packageName;
	// Create environment value
	sprintf( m_environment.data, "LBR_%x", (unsigned int)(this) );
	// Create lua object
	Initialize();
}
CLuaBehavior::~CLuaBehavior ( void )
{
	Cleanup();
}

void CLuaBehavior::Initialize ( void )
{
	if ( m_packageName.length() > 0 ) {
		Lua::Controller->LoadLuaFile( std::string("../../addons/") + m_packageName.c_str() + "/lua/" + m_entityName.c_str() + ".lua", m_environment.c_str(), m_environment.c_str() );
	}
	else {
		Lua::Controller->LoadLuaFile( std::string("") + m_entityName.c_str() + ".lua", m_environment.c_str(), m_environment.c_str() );
	}
	lua_State* L = Lua::Controller->GetState();

	Lua::Controller->SetEnvironment( m_environment.c_str() );

	// Check the loaded Lua
	m_enableKey = 0;	// Clear enabled keys

	lua_getfield( L, -1, "OnCreate" );
	//lua_getglobal( L, "OnCreate" );
	if ( lua_isfunction( L, -1 ) ) {
		m_enableKey |= EK_OnCreate;
	}
	lua_pop(L,1);
	lua_getfield( L, -1, "OnDestroy" );
	if ( !lua_isnil( L, -1 ) ) {
		m_enableKey |= EK_OnDestroy;
	}
	lua_pop(L,1);

	lua_getfield( L, -1, "Update" );
	if ( !lua_isnil( L, -1 ) ) {
		m_enableKey |= EK_Update;
	}
	lua_pop(L,1);
	lua_getfield( L, -1, "LateUpdate" );
	if ( !lua_isnil( L, -1 ) ) {
		m_enableKey |= EK_LateUpdate;
	}
	lua_pop(L,1);
	lua_getfield( L, -1, "PostUpdate" );
	if ( !lua_isnil( L, -1 ) ) {
		m_enableKey |= EK_PostUpdate;
	}
	lua_pop(L,1);

	lua_getfield( L, -1, "FixedUpdate" );
	if ( !lua_isnil( L, -1 ) ) {
		m_enableKey |= EK_FixedUpdate;
	}
	lua_pop(L,1);

	lua_getfield( L, -1, "OnReceiveSignal" );
	if ( !lua_isnil( L, -1 ) ) {
		m_enableKey |= EK_OnReceiveSignal;
	}
	lua_pop(L,1);

	Lua::Controller->ResetEnvironment();
}

void CLuaBehavior::Cleanup ( void )
{
	// Fix this shit later
}



void CLuaBehavior::OnCreate ( void )
{
	if ( m_enableKey & EK_OnCreate )
	{
		Lua::Controller->SetEnvironment( m_environment.c_str() );
		lua_State* L = Lua::Controller->GetState();
		Lua::Controller->CallPop( "OnCreate",0 );
		Lua::Controller->ResetEnvironment();
	}
}
void CLuaBehavior::OnDestroy ( void )
{
	if ( m_enableKey & EK_OnDestroy )
	{
		Lua::Controller->SetEnvironment( m_environment.c_str() );
		lua_State* L = Lua::Controller->GetState();
		Lua::Controller->CallPop( "OnDestroy",0 );
		Lua::Controller->ResetEnvironment();
	}
}

void CLuaBehavior::Update ( void )
{
	if ( m_enableKey & EK_Update )
	{
		Lua::Controller->SetEnvironment( m_environment.c_str() );
		lua_State* L = Lua::Controller->GetState();
		Lua::Controller->CallPop( "Update",1, Lua::FN_DOUBLE, (double)Time::deltaTime );
		Lua::Controller->ResetEnvironment();
	}
}
void CLuaBehavior::LateUpdate ( void )
{
	if ( m_enableKey & EK_LateUpdate )
	{
		Lua::Controller->SetEnvironment( m_environment.c_str() );
		lua_State* L = Lua::Controller->GetState();
		Lua::Controller->CallPop( "LateUpdate",1, Lua::FN_DOUBLE, (double)Time::deltaTime );
		Lua::Controller->ResetEnvironment();
	}
}
void CLuaBehavior::PostUpdate ( void )
{
	if ( m_enableKey & EK_PostUpdate )
	{
		Lua::Controller->SetEnvironment( m_environment.c_str() );
		lua_State* L = Lua::Controller->GetState();
		Lua::Controller->CallPop( "PostUpdate",1, Lua::FN_DOUBLE, (double)Time::deltaTime );
		Lua::Controller->ResetEnvironment();
	}
}

void CLuaBehavior::FixedUpdate ( void )
{
	if ( m_enableKey & EK_FixedUpdate )
	{
		Lua::Controller->SetEnvironment( m_environment.c_str() );
		lua_State* L = Lua::Controller->GetState();
		Lua::Controller->CallPop( "FixedUpdate",1, Lua::FN_DOUBLE, (double)Time::deltaTime );
		Lua::Controller->ResetEnvironment();
	}
}

void CLuaBehavior::OnReceiveSignal ( const uint64_t signal )
{
	if ( m_enableKey & EK_OnReceiveSignal )
	{
		Lua::Controller->SetEnvironment( m_environment.c_str() );
		lua_State* L = Lua::Controller->GetState();
		Lua::Controller->CallPop( "FixedUpdate",1, Lua::FN_INT, signal );
		Lua::Controller->ResetEnvironment();
	}
}