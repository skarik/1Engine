#ifndef GAME_SCENE_EDITOR_MATERIAL_BROSWER_H_
#define GAME_SCENE_EDITOR_MATERIAL_BROSWER_H_

#include "engine/state/CGameScene.h"

class sceneEditorMaterialBrowser : public CGameScene
{
public:
	GAME_API sceneEditorMaterialBrowser ( void )
		: CGameScene()
	{
		bIsAdditive = true;
		bCanBeStreamed = false;
	}

	ER_DEFINE_SCENE_NAME( GAME_API, "MaterialBrowser" );
protected:
	GAME_API void			LoadScene ( void );
};

#endif//GAME_SCENE_EDITOR_MATERIAL_BROSWER_H_