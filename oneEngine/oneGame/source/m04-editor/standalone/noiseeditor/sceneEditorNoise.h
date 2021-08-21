#ifndef GAME_SCENE_EDITOR_NOISE_H_
#define GAME_SCENE_EDITOR_NOISE_H_

#include "engine/state/CGameScene.h"

class sceneEditorNoise : public CGameScene
{
public:
	GAME_API sceneEditorNoise ( void )
		: CGameScene()
	{
		bIsAdditive = true;
		bCanBeStreamed = false;
	}

	static const char*		GetName ( void )
		{ return "NoiseEditor"; }
protected:
	GAME_API void			LoadScene ( void );
};

#endif//GAME_SCENE_EDITOR_NOISE_H_