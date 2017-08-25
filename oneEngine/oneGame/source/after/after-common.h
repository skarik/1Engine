#ifndef _AFTER_AFTER_COMMON_H_
#define _AFTER_AFTER_COMMON_H_

//			after
// AFTER's game code.
//
// Relies on the following:
//	core
//	core-ext
//	audio
//	renderer
//	engine
//	engine-common
//

#include "core/types/types.h"

//===============================================================================================//
//	GAME INITIALIZATION
//===============================================================================================//

//===============================================================================================//
//	GameInitialize
//
// After the main engine components have been started, this function is called.
// This calls EngineCommonInitialize first, then performs its own actions.
// It adds game specific bindings to the console, Lua system, and other systems.
//===============================================================================================//
GAME_API int GameInitialize ( void );


#endif//_AFTER_AFTER_COMMON_H_