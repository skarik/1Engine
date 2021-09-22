#ifndef GAME_SCENE_EDITOR_LEVEL_BUILDER_H_
#define GAME_SCENE_EDITOR_LEVEL_BUILDER_H_

#include "engine/state/CGameScene.h"

class sceneEditorLevelBuilder : public CGameScene
{
public:
	GAME_API sceneEditorLevelBuilder ( void )
		: CGameScene()
	{
		bIsAdditive = true;
		bCanBeStreamed = false;
	}

	ER_DEFINE_SCENE_NAME( GAME_API, "LevelBuilder" );
protected:
	GAME_API void			LoadScene ( void );
};

#endif//GAME_SCENE_EDITOR_LEVEL_BUILDER_H_