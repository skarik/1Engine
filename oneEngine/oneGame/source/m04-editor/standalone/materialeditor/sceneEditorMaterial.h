#ifndef GAME_SCENE_EDITOR_MATERIAL_H_
#define GAME_SCENE_EDITOR_MATERIAL_H_

#include "engine/state/CGameScene.h"

class sceneEditorMaterial : public CGameScene
{
public:
	GAME_API sceneEditorMaterial ( void )
		: CGameScene()
	{
		bIsAdditive = true;
		bCanBeStreamed = false;
	}

	ER_DEFINE_SCENE_NAME( GAME_API, "MaterialEditor" );
protected:
	GAME_API void			LoadScene ( void );
};

#endif//GAME_SCENE_EDITOR_MATERIAL_H_