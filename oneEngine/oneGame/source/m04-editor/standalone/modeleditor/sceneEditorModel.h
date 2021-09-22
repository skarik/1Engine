#ifndef GAME_SCENE_EDITOR_MODEL_H_
#define GAME_SCENE_EDITOR_MODEL_H_

#include "engine/state/CGameScene.h"

class sceneEditorModel : public CGameScene
{
public:
	GAME_API sceneEditorModel ( void )
		: CGameScene()
	{
		bIsAdditive = true;
		bCanBeStreamed = false;
	}

	ER_DEFINE_SCENE_NAME( GAME_API, "ModelEditor" );
protected:
	GAME_API void			LoadScene ( void );
};

#endif//GAME_SCENE_EDITOR_MODEL_H_