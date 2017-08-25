
#ifndef _LUA_GAME_STATE_H_
#define _LUA_GAME_STATE_H_

#include "CLuaController.h"


int L_SetTransform ( lua_State * L );	// Takes an ID and a table.
int L_GetTransform ( lua_State * L );	// Takes an ID. Returns a table.

// Target
// Would like to have the following
/*

transform = Object.GetTransform();

// however the following is easier to implement

transform = GetTransform( Object );

// Should look into linking table functions to hooks.


// SO BASICALLY
function GetObject( ID )
	new_object = {
	
		-- Set default properties
		id = ID;
		transform = L_GO_GetTransform(ID);

		-- Define functions and function aliases
		function GetTransform ()
			return L_GO_GetTransform(id);
		end
		function SetTransform ()
			return L_GO_SetTransform(id,transform);
		end

	};
	return new_object;
end

*/