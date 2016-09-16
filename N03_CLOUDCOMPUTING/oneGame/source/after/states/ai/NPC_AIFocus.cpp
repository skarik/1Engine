
#include "NPC_AIState.h"
#include "engine-common/lua/CLuaController.h"
#include "core/time/time.h"

//	SetFocus()
// Changes the AI's focus. It will delete the current AI's focus and then create a new one.
// If the focus is already set to the same one, it will recreate regardless. Thus, this should
// not be called every frame.
void NPC::AIState::SetFocus ( const eBasicAIFocus nnew_focus, const char* nnew_routine )
{
	// Delete old focus
	if ( ai_focus ) {
		delete ai_focus;
	}
	// Create new focus
	switch ( nnew_focus )
	{
	case AIFOCUS_Companion:
		ai_focus = new AIF_CompanionBehavior( this );
		ai_focus->Initialize();
		break;
	case AIFOCUS_Wanderer:
		ai_focus = new AIF_WandererBehavior( this );
		ai_focus->Initialize();
		break;
	case AIFOCUS_LuaGeneral:
		ai_focus = new AIF_GeneralLuaBehavior( this, nnew_routine );
		ai_focus->Initialize();
		break;
	default:
		ai_focus = NULL;
		break;
	}

	i_focus_index = nnew_focus;
	i_focus_routine = nnew_routine;
}
void NPC::AIState::GetFocus ( eBasicAIFocus& o_focus, char* o_routine )
{
	o_focus = i_focus_index;
	strcpy( o_routine, i_focus_routine );
}
void NPC::AIAccessor::SetFocus ( const int nNewFocus, const char* nnNewFocusName )
{
	ai->SetFocus( (eBasicAIFocus)nNewFocus, nnNewFocusName );
}

bool NPC::AIState::AIF_WandererBehavior::Execute ( void )
{
	//cout << ai->ai_think.isAngry << " " << ai->ai_think.isAlerted << endl;
	//if ( !ai->ai_think.isAngry && !ai->ai_think.isAlerted ) { 
	if ( ai->ai_think.infostate == ai_think_state_t::AI_RELAXED )
	{
		ai->AI_Wander();
		return false;
	}
	else
	{
		return true;
	}
}

void NPC::AIState::AIF_CompanionBehavior::Initialize ( void )
{
	// Load LUA
	sprintf( m_environment, "AIF_CompanionBehavior_%x", this );

	Lua::Controller->LoadLuaFile( "ai/focus/companion.lua", m_environment.c_str(), m_environment.c_str() );

	Lua::Controller->SetEnvironment( m_environment.c_str() );

	lua_State* L = Lua::Controller->GetState();

	lua_pushlightuserdata( L, ai );			lua_setfield( L, -2, "targetAI" );
	lua_pushlightuserdata( L, ai->owner );	lua_setfield( L, -2, "mCharacter" );
	lua_pushboolean( L, true );				lua_setfield( L, -2, "_ready" );

	Lua::Controller->CallPop( "Initialize",0 );

	Lua::Controller->ResetEnvironment();
}
void NPC::AIState::AIF_CompanionBehavior::Destruction ( void )
{
	Lua::Controller->FreeLuaFile( "ai/focus/companion.lua", m_environment.c_str() );
}

bool NPC::AIState::AIF_CompanionBehavior::Execute ( void )
{
	// Call the AI think in lua
	Lua::Controller->SetEnvironment( m_environment.c_str() );

	lua_State* L = Lua::Controller->GetState();

	lua_getfield( L, -1, "_ready" );
	if ( lua_toboolean( L, -1 ) ) {
		lua_pop(L,1);
		lua_pushlightuserdata( L, ai );			lua_setfield( L, -2, "targetAI" );
		lua_pushlightuserdata( L, ai->owner );	lua_setfield( L, -2, "mCharacter" );
		lua_pushboolean( L, true );				lua_setfield( L, -2, "_ready" );
	}
	else {
		lua_pop(L,1);
	}
	//lua_pushboolean( L, ai->ai_think.isAngry );		lua_setfield( L, -2, "isAngry" );
	//lua_pushboolean( L, ai->ai_think.isAlerted );	lua_setfield( L, -2, "isAlerted" );
	lua_pushinteger (L, (int)ai->ai_think.infostate );	lua_setfield( L, -2, "angerState" );

	int returns = Lua::Controller->Call( "Execute",1,
		Lua::FN_DOUBLE, (double)Time::deltaTime );

	int result = 0;
	if ( returns == 1 ) {
		result = lua_tointeger( L,-1 );	lua_pop( L,1 );
		Lua::Controller->PopValid( returns-1 );
	}
	else {
		Lua::Controller->PopValid( returns );
		Debug::Console->PrintError( "focus::Execute needs to return one value!\n" );
	}
	Lua::Controller->ResetEnvironment();


	return (result!=0);
}

void NPC::AIState::AIF_CompanionBehavior::PartyCmdFollowMe ( CCharacter* ncharacter )
{
	Lua::Controller->SetEnvironment( m_environment.c_str() );
	Lua::Controller->CallPop( "PartyCmdFollowMe",1,Lua::FN_USERDATA,ncharacter );
	Lua::Controller->ResetEnvironment();
}

void NPC::AIState::AIF_CompanionBehavior::PartyCmdMoveTo ( const Vector3d & nposition )
{
	Lua::Controller->SetEnvironment( m_environment.c_str() );
	Lua::Controller->CallPop( "PartyCmdMoveTo",3,
		Lua::FN_DOUBLE,(double)nposition.x,
		Lua::FN_DOUBLE,(double)nposition.y,
		Lua::FN_DOUBLE,(double)nposition.z );
	Lua::Controller->ResetEnvironment();
}



void NPC::AIState::AIF_GeneralLuaBehavior::Initialize ( void )
{
	// Load LUA
	sprintf( m_environment, "AIF_%s_%x", m_behavior.c_str(), this );

	char fTempFilename [128];
	sprintf( fTempFilename, "ai/focus/%s.lua", m_behavior.c_str() );
	Lua::Controller->LoadLuaFile( fTempFilename, m_environment.c_str(), m_environment.c_str() );

	Lua::Controller->SetEnvironment( m_environment.c_str() );
	/*lua_State* L = Lua::Controller->GetState();
	lua_pushlightuserdata( L, ai );			lua_setfield( L, -2, "targetAI" );
	lua_pushlightuserdata( L, ai->owner );	lua_setfield( L, -2, "mCharacter" );
	lua_pushboolean( L, true );				lua_setfield( L, -2, "_ready" );*/
	Lua::Controller->CallPop( "Initialize",0 );
	Lua::Controller->ResetEnvironment();
}
void NPC::AIState::AIF_GeneralLuaBehavior::Destruction ( void )
{
	char fTempFilename [128];
	sprintf( fTempFilename, "ai/focus/%s.lua", m_behavior.c_str() );
	Lua::Controller->FreeLuaFile( fTempFilename, m_environment.c_str() );
}
bool NPC::AIState::AIF_GeneralLuaBehavior::Execute ( void )
{
	// Call the AI think in lua
	Lua::Controller->SetEnvironment( m_environment.c_str() );
	lua_State* L = Lua::Controller->GetState();

	// Check if _ready is false. If _ready is not true, then reinitialize values
	lua_getfield( L, -1, "_ready" );
	if ( !lua_toboolean( L, -1 ) )
	{
		lua_pop(L,1);
		lua_pushlightuserdata( L, ai );			lua_setfield( L, -2, "targetAI" );
		lua_pushlightuserdata( L, ai->owner );	lua_setfield( L, -2, "mCharacter" );
		lua_pushboolean( L, true );				lua_setfield( L, -2, "_ready" );
		Lua::Controller->CallPop( "Initialize",0 );
	}
	else
	{
		lua_pop(L,1);
	}
	//lua_pushboolean( L, ai->ai_think.isAngry );		lua_setfield( L, -2, "isAngry" );
	//lua_pushboolean( L, ai->ai_think.isAlerted );	lua_setfield( L, -2, "isAlerted" );
	lua_pushinteger (L, (int)ai->ai_think.infostate );	lua_setfield( L, -2, "angerState" );

	int returns = Lua::Controller->Call( "Execute",1,
		Lua::FN_DOUBLE, (double)Time::deltaTime );

	int result = 0;
	if ( returns == 1 ) {
		result = lua_tointeger( L,-1 );	lua_pop( L,1 );
		Lua::Controller->PopValid( returns-1 );
	}
	else {
		Lua::Controller->PopValid( returns );
		Debug::Console->PrintError( "focus::Execute needs to return one value!\n" );
	}
	Lua::Controller->ResetEnvironment();


	return (result!=0);
}