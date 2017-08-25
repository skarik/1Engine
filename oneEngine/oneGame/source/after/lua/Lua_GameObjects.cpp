
#include "engine-common/lua/CLuaController.h"

#include "after/lua/Lua.h"

#include "after/entities/character/CCharacter.h"
#include "after/states/ai/NPC_AIState.h"

#include "after/entities/character/npc/CNpcBase.h"
#include "after/entities/CCharacterModel.h"

#include "engine/state/CGameState.h"
#include "engine-common/entities/CWaypoint.h"

//int lua_gs_FindBehavior ( lua_State *luaState );

// Characters
int lua_char_stats_get_health ( lua_State* luaState );
int lua_char_stats_get_health_max ( lua_State* luaState );
int lua_char_stats_get_mana ( lua_State* luaState );
int lua_char_stats_get_mana_max ( lua_State* luaState );
int lua_char_stats_get_stamina ( lua_State* luaState );
int lua_char_stats_get_stamina_max ( lua_State* luaState );


// NPCS
int lua_npc_ai_MoveTo ( lua_State* luaState );
int lua_npc_ai_Wander ( lua_State* luaState );
int lua_npc_ai_Follow ( lua_State* luaState );
int lua_npc_ai_SetFollowTarget ( lua_State* luaState );
int lua_npc_ai_PerformAttack ( lua_State* luaState );
int lua_npc_ai_FaceAt ( lua_State* luaState );
int lua_npc_ai_PerformDefend ( lua_State* luaState );
int lua_npc_ai_SetFocus ( lua_State* luaState );

int lua_npc_ai_PlayAnimation ( lua_State* luaState );

// Waypoint
int lua_waypoint_Create ( lua_State* luaState );
int lua_waypoint_Destroy( lua_State* luaState );
int lua_waypoint_ChangeType	( lua_State* luaState );
int lua_waypoint_SetPosition( lua_State* luaState );


// REGISTER
int Lua::LuaRegister_AFTERObjects ( lua_State* luaState )
{
	lua_register( luaState, "_char_stats_get_health",		lua_char_stats_get_health );
	lua_register( luaState, "_char_stats_get_health_max",	lua_char_stats_get_health_max );
	lua_register( luaState, "_char_stats_get_mana",			lua_char_stats_get_mana );
	lua_register( luaState, "_char_stats_get_mana_max",		lua_char_stats_get_mana_max );
	lua_register( luaState, "_char_stats_get_stamina",		lua_char_stats_get_stamina );
	lua_register( luaState, "_char_stats_get_stamina_max",	lua_char_stats_get_stamina_max );

	lua_register( luaState,	"_npc_ai_MoveTo",			lua_npc_ai_MoveTo );
	lua_register( luaState,	"_npc_ai_Wander",			lua_npc_ai_Wander );
	lua_register( luaState,	"_npc_ai_Follow",			lua_npc_ai_Follow );
	lua_register( luaState,	"_npc_ai_SetFollowTarget",	lua_npc_ai_SetFollowTarget );
	lua_register( luaState, "_npc_ai_PerformAttack",	lua_npc_ai_PerformAttack );
	lua_register( luaState, "_npc_ai_FaceAt",			lua_npc_ai_FaceAt );
	lua_register( luaState, "_npc_ai_PerformDefend",	lua_npc_ai_PerformDefend );
	lua_register( luaState, "_npc_ai_SetFocus",			lua_npc_ai_SetFocus );

	lua_register( luaState, "_npc_ai_PlayAnimation",	lua_npc_ai_PlayAnimation );

	lua_register( luaState, "_waypoint_Create",			lua_waypoint_Create );
	lua_register( luaState, "_waypoint_Destroy",		lua_waypoint_Destroy );
	lua_register( luaState, "_waypoint_ChangeType",		lua_waypoint_ChangeType );
	lua_register( luaState, "_waypoint_SetPosition",	lua_waypoint_SetPosition );

	return 0;
}


int lua_char_stats_get_health ( lua_State* luaState )
{
	// Load pointer from Lua stack
	CCharacter* target = (CCharacter*) lua_touserdata( luaState, 1 );
	if ( target && target->GetCharStats() )
	{
		lua_pushnumber( luaState, target->GetCharStats()->fHealth );
	}
	return 1;
}
int lua_char_stats_get_health_max ( lua_State* luaState )
{
	// Load pointer from Lua stack
	CCharacter* target = (CCharacter*) lua_touserdata( luaState, 1 );
	if ( target && target->GetCharStats() )
	{
		lua_pushnumber( luaState, target->GetCharStats()->fHealthMax );
	}
	return 1;
}
int lua_char_stats_get_mana ( lua_State* luaState )
{
	// Load pointer from Lua stack
	CCharacter* target = (CCharacter*) lua_touserdata( luaState, 1 );
	if ( target && target->GetCharStats() )
	{
		lua_pushnumber( luaState, target->GetCharStats()->fMana );
	}
	return 1;
}
int lua_char_stats_get_mana_max ( lua_State* luaState )
{
	// Load pointer from Lua stack
	CCharacter* target = (CCharacter*) lua_touserdata( luaState, 1 );
	if ( target && target->GetCharStats() )
	{
		lua_pushnumber( luaState, target->GetCharStats()->fManaMax );
	}
	return 1;
}
int lua_char_stats_get_stamina ( lua_State* luaState )
{
	// Load pointer from Lua stack
	CCharacter* target = (CCharacter*) lua_touserdata( luaState, 1 );
	if ( target && target->GetCharStats() )
	{
		lua_pushnumber( luaState, target->GetCharStats()->fStamina );
	}
	return 1;
}
int lua_char_stats_get_stamina_max ( lua_State* luaState )
{
	// Load pointer from Lua stack
	CCharacter* target = (CCharacter*) lua_touserdata( luaState, 1 );
	if ( target && target->GetCharStats() )
	{
		lua_pushnumber( luaState, target->GetCharStats()->fStaminaMax );
	}
	return 1;
}


int lua_npc_ai_MoveTo ( lua_State *luaState )
{
	// Load pointer from Lua stack
	NPC::AIState* target = (NPC::AIState*) lua_touserdata( luaState, 1 );
	if ( target )
	{
		Vector3d targetPosition;
		targetPosition.x = (ftype) lua_tonumber( luaState, 2 );
		targetPosition.y = (ftype) lua_tonumber( luaState, 3 );
		targetPosition.z = (ftype) lua_tonumber( luaState, 4 );

		NPC::AIAccessor t ( target );
		bool result = t.MoveTo( targetPosition, lua_toboolean( luaState,5 )!=0 );
		lua_pushboolean( luaState, result );
	}
	return 1; // Returns value of MoveTo (true when reach target)
}
int lua_npc_ai_Wander ( lua_State *luaState )
{
	// Load pointer from Lua stack
	NPC::AIState* target = (NPC::AIState*) lua_touserdata( luaState, 1 );
	if ( target ) {
		NPC::AIAccessor t ( target );
		t.Wander();
	}
	return 0; // Returns no value
}
int lua_npc_ai_Follow ( lua_State *luaState )
{
	// Load pointer from Lua stack
	NPC::AIState* target = (NPC::AIState*) lua_touserdata( luaState, 1 );
	if ( target ) {
		NPC::AIAccessor t ( target );
		t.Follow();
	}
	return 0; // Returns no value
}
int lua_npc_ai_SetFollowTarget ( lua_State *luaState )
{
	// Load pointer from Lua stack
	NPC::AIState* target = (NPC::AIState*) lua_touserdata( luaState, 1 );
	if ( target ) {
		CCharacter*	targetChar = (CCharacter*) lua_touserdata( luaState, 2 );

		NPC::AIAccessor t ( target );
		t.SetFollowTarget(targetChar);
	}
	return 0; // Returns no value
}
int lua_npc_ai_PerformAttack ( lua_State *luaState )
{
	// Load pointer from Lua stack
	NPC::AIState* target = (NPC::AIState*) lua_touserdata( luaState, 1 );
	if ( target )
	{
		NPC::AIAccessor t ( target );
		t.PerformAttack();
		
		lua_State* L = Lua::Controller->GetState();
		lua_pushboolean( L, target->response_atk.atk_performed );
		lua_pushboolean( L, target->response_atk.atk_waitForResult );
		lua_pushnumber( L, target->response_atk.atk_waittime );
		return 3;
	}
	return 0; // Returns no value
}
int lua_npc_ai_FaceAt ( lua_State *luaState )
{
	NPC::AIState* target = (NPC::AIState*) lua_touserdata( luaState, 1 );
	if ( target ) {
		Vector3d targetPosition;
		targetPosition.x = (ftype) lua_tonumber( luaState, 2 );
		targetPosition.y = (ftype) lua_tonumber( luaState, 3 );
		targetPosition.z = (ftype) lua_tonumber( luaState, 4 );

		NPC::AIAccessor t ( target );
		t.FaceAt( targetPosition );
	}
	return 0;
}
int lua_npc_ai_PerformDefend ( lua_State *luaState )
{
	// Load pointer from Lua stack
	NPC::AIState* target = (NPC::AIState*) lua_touserdata( luaState, 1 );
	if ( target ) {

		NPC::AIAccessor t ( target );
		t.PerformDefend();
		
		return 0;
	}
	return 0; // Returns no value
}
int lua_npc_ai_SetFocus ( lua_State* luaState )
{
	// Load pointer from Lua stack
	NPC::AIState* target = (NPC::AIState*) lua_touserdata( luaState, 1 );
	if ( target ) {

		NPC::AIAccessor t ( target );
		t.SetFocus( lua_tointeger( luaState,2 ), lua_tostring( luaState,3) );
		
		return 0;
	}
	return 0; // Returns no value
}

int lua_npc_ai_PlayAnimation ( lua_State* luaState )
{
	NPC::CNpcBase* target = (NPC::CNpcBase*) lua_touserdata( luaState, 1 );
	if ( target ) {
		char anim [256];
		strcpy( anim, lua_tostring( luaState, 2 ) );
		printf( "%s\n", anim );
		target->GetCharModel()->PlayScriptedAnimation( anim );
		target->GetCharModel()->PlayAnimation( anim );
	}
	return 0; // Returns no value
}


int lua_waypoint_Create ( lua_State* luaState )
{
	CWaypoint* newWaypoint = new CWaypoint;
	lua_pushlightuserdata( luaState, newWaypoint );
	return 1;
}
int lua_waypoint_Destroy( lua_State* luaState )
{
	CWaypoint* target = (CWaypoint*) lua_touserdata( luaState, 1 );
	if ( target )
	{
		CGameState::Active()->DeleteObject( target );
	}
	return 0; // Returns no value
}
int lua_waypoint_ChangeType	( lua_State* luaState )
{
	CWaypoint* target = (CWaypoint*) lua_touserdata( luaState, 1 );
	if ( target ) {
		target->type = (CWaypoint::eWaypointType) lua_tointeger( luaState, 2 );
	}
	return 0;
}
int lua_waypoint_SetPosition( lua_State* luaState )
{
	CWaypoint* target = (CWaypoint*) lua_touserdata( luaState, 1 );
	if ( target ) {
		Vector3d targetPosition;
		targetPosition.x = (ftype) lua_tonumber( luaState, 2 );
		targetPosition.y = (ftype) lua_tonumber( luaState, 3 );
		targetPosition.z = (ftype) lua_tonumber( luaState, 4 );
		target->m_position = targetPosition;
	}
	return 0;
}