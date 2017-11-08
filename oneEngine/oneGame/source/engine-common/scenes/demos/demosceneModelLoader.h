#ifndef DEMOSCENE_MODEL_LOADER_H_
#define DEMOSCENE_MODEL_LOADER_H_

#include "engine/state/CGameScene.h"

class demosceneModelLoader : public CGameScene
{
protected:
	ENGCOM_API void LoadScene(void);
};

#endif//DEMOSCENE_MODEL_LOADER_H_