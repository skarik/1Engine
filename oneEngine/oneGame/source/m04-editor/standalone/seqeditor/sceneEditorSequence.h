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

	static const char* GetName ( void )
		{ return "SequenceEditor"; }
protected:
	GAME_API void LoadScene ( void );
};

#endif//GAME_SCENE_EDITOR_SEQUENCE_