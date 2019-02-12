
#include "CLuaController.h"

#include "renderer/state/RrRenderer.h"
#include "renderer/object/CRenderableObject.h"


//int lua_gs_FindBehavior ( lua_State *luaState );


int Lua::Register_Renderer ( lua_State* luaState )
{
	//lua_register( luaState,	"_gs_FindBehavior",			lua_gs_FindBehavior );

	return 0;
}

/*
int lua_gs_FindBehavior ( lua_State *luaState )
{
	// Load name
}*/