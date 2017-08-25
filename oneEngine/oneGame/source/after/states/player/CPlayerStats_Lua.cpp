
#include <unordered_map>

#include "../CharacterStats.h"
#include "../CRacialStats.h"
#include "engine-common/lua/CLuaController.h"

/*
--------------------------------------------
-- Table structure for race properties --
--------------------------------------------
RACES.races[0] = {};
RACES.races[0].iRace				= 0;
-- Movement abilities
RACES.races[0].bCanPowerslide 		= true;
RACES.races[0].bCanSprint			= true;
RACES.races[0].bCanCrouchSprint		= false;
-- Movespeeds + Acceleration
RACES.races[0].fRunSpeed			= 11.0;		-- Upright speed
RACES.races[0].fSprintSpeed			= 17.0;		-- Sprinting speed
RACES.races[0].fCrouchSpeed			= 6.0;		-- Crouching speed
RACES.races[0].fProneSpeed			= 2.0;	
RACES.races[0].fSwimSpeed			= 12.0;	-- Water base move speed
RACES.races[0].fGroundAccelBase		= 55.0;
RACES.races[0].fAirAccelBase		= 10.0;
RACES.races[0].fWaterAccelBase		= 20.0;
-- Body size and height
RACES.races[0].fStandingHeight		= 5.7;
RACES.races[0].fCrouchingHeight		= 3.7;
RACES.races[0].fProneHeight			= 1.6;
RACES.races[0].fCollisionRadius		= 0.7;
-- Point stats
RACES.races[0].startStrength		= 13;
RACES.races[0].startAgility			= 13;
RACES.races[0].startIntelligence	= 13;
*/

std::unordered_map<string,float*>		floatTable;
std::unordered_map<string,bool*>		boolTable;
std::unordered_map<string,uint*>		uintTable;
	
void	CRacialStats::Lua_GenerateLinkTable ( void )
{
	floatTable["fRunSpeed"]			= &fRunSpeed;
	floatTable["fSprintSpeed"]		= &fSprintSpeed;
	floatTable["fCrouchSpeed"]		= &fCrouchSpeed;
	floatTable["fProneSpeed"]		= &fProneSpeed;
	floatTable["fSwimSpeed"]		= &fSwimSpeed;
	floatTable["fGroundAccelBase"]	= &fGroundAccelBase;
	floatTable["fAirAccelBase"]		= &fAirAccelBase;
	floatTable["fWaterAccelBase"]	= &fWaterAccelBase;

	floatTable["fCollisionRadius"]	= &fPlayerRadius;
	floatTable["fStandingHeight"]	= &fStandingHeight;
	floatTable["fCrouchingHeight"]	= &fCrouchingHeight;
	floatTable["fProneHeight"]		= &fProneHeight;

	boolTable["bCanPowerslide"]		= &bCanPowerslide;
	boolTable["bCanSprint"]			= &bCanSprint;
	boolTable["bCanCrouchSprint"]	= &bCanCrouchSprint;

	uintTable["startStrength"]		= &stats->iStrength;
	uintTable["startAgility"]		= &stats->iAgility;
	uintTable["startIntelligence"]	= &stats->iIntelligence;
	uintTable["startCharisma"]		= &stats->iCharisma;

	uintTable["iWieldCount"]		= &iWieldCount;
}

void	CRacialStats::SetLuaDefaults ( void )
{
	if ( !bTableReady ) {
		Lua_GenerateLinkTable();
		bTableReady = true;
	}

	// Call Lua with the current value for race and retrieve the table.
	Lua::Controller->Call( "getRaceValues", 1, Lua::FN_INT, (int)iRace );

	// Now, get the table via direct access
	lua_State*	luaVM = Lua::Controller->GetState();

	if ( !lua_istable( luaVM, -1 ) ) {
		Debug::Console->PrintError( "Bad stats table returned!" );
	}

	//printf( "top: %d\n", lua_gettop( luaVM ) );

	// table is in the stack at index 't'
	lua_pushnil(luaVM);  // Get first key
	while (lua_next(luaVM, -2) != 0)
	{
		// uses 'key' (at index -2) and 'value' (at index -1)
		/*printf("%s - %s\n",
			lua_typename(luaVM, lua_type(luaVM, -2)),
			lua_typename(luaVM, lua_type(luaVM, -1)));*/
		// Get the key
		string key = lua_tostring( luaVM,-2 );
		// Search all the tables for the key
		if ( uintTable.find(key) != uintTable.end() ) {
			(*uintTable[key]) = lua_tointeger( luaVM,-1 );
		}
		else if ( boolTable.find(key) != boolTable.end() ) {
			(*boolTable[key]) = (lua_toboolean( luaVM,-1 ) != 0);
		}
		else if ( floatTable.find(key) != floatTable.end() ) {
			(*floatTable[key]) = (float)(lua_tonumber( luaVM,-1 ));
		}

		// removes 'value'; keeps 'key' for next iteration
		lua_pop(luaVM, 1);
	}
	lua_pop(luaVM, 1);

	//printf( "top: %d\n", lua_gettop( luaVM ) );

}