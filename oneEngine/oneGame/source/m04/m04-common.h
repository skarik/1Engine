#ifndef M04_M04_COMMON_H_
#define M04_M04_COMMON_H_

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


#endif//M04_M04_COMMON_H_