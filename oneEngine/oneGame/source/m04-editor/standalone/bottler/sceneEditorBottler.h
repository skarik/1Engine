#ifndef GAME_SCENE_EDITOR_BOTTLER_H_
#define GAME_SCENE_EDITOR_BOTTLER_H_

#include "engine/state/CGameScene.h"

class sceneEditorBottler : public CGameScene
{
public:
	GAME_API sceneEditorBottler ( void )
		: CGameScene()
	{
		bIsAdditive = true;
		bCanBeStreamed = false;
	}

	ER_DEFINE_SCENE_NAME( GAME_API, "BottlerUI" );
protected:
	GAME_API void			LoadScene ( void );
};

#endif//GAME_SCENE_EDITOR_MATERIAL_H_