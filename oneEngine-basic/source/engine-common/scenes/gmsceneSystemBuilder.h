
#ifndef _GAME_SCENE_SYSTEM_BUILDER_
#define _GAME_SCENE_SYSTEM_BUILDER_

#include "engine/state/CGameScene.h"

class gmsceneSystemBuilder : public CGameScene
{
protected:
	ENGCOM_API void LoadScene ( void );
};

namespace EngineCommon
{
	//		BuildToTarget( build_target )
	// Builds to the given target
	ENGCOM_API int BuildToTarget ( const std::string& );
}

#endif//_GAME_SCENE_SYSTEM_LOADER_