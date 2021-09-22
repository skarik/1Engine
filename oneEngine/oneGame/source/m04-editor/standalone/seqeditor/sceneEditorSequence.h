#ifndef GAME_SCENE_EDITOR_SEQUENCE_
#define GAME_SCENE_EDITOR_SEQUENCE_

#include "engine/state/CGameScene.h"

class sceneEditorSequence : public CGameScene
{
public:
	GAME_API sceneEditorSequence ( void )
		: CGameScene()
	{
		bIsAdditive = true;
		bCanBeStreamed = false;
	}

	ER_DEFINE_SCENE_NAME( GAME_API, "SequenceEditor" );
protected:
	GAME_API void LoadScene ( void );
};

#endif//GAME_SCENE_EDITOR_SEQUENCE_