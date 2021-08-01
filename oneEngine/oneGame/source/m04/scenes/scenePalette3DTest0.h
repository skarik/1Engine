#ifndef GAME_SCENE_PALETTE_3D_TEST_0_H_
#define GAME_SCENE_PALETTE_3D_TEST_0_H_

#include "engine/state/CGameScene.h"

class scenePalette3DTest0 : public CGameScene
{
public:
	static const char* GetName ( void )
	{ return "Palette3DTest0"; }
protected:
	GAME_API void LoadScene ( void );
};

#endif//GAME_SCENE_PALETTE_3D_TEST_0_H_