#ifndef GAME_SCENE_PALETTE_3D_TEST_0_H_
#define GAME_SCENE_PALETTE_3D_TEST_0_H_

#include "engine/state/CGameScene.h"

class scenePalette3DTest0 : public CGameScene
{
public:
	ER_DEFINE_SCENE_NAME( GAME_API, "Palette3DTest0" );
protected:
	GAME_API void LoadScene ( void );
};

#endif//GAME_SCENE_PALETTE_3D_TEST_0_H_