
#ifndef _C_TERRAIN_LOADSCREEN_H_
#define _C_TERRAIN_LOADSCREEN_H_

#include "renderer/object/CRenderableObject.h"
#include "engine/behavior/CGameBehavior.h"

class CBitmapFont;

class CTerrainLoadscreen : public CRenderableObject, public CGameBehavior
{

public:
	CTerrainLoadscreen ( void );
	~CTerrainLoadscreen ( void );

	void Update ( void );
	bool Render ( const char pass );

private:

	glMaterial*	bgMaterial;
	glMaterial*	barMaterial;

	glMaterial*		matFntLoader;
	CBitmapFont*	fntLoaderText;

	ftype		currentPercentage;
	//ftype		targetPercentage;
};

#endif