
#ifndef _C_OCTREE_PAUSE_LOADER_H_
#define _C_OCTREE_PAUSE_LOADER_H_

#include "engine/behavior/CGameBehavior.h"
#include "renderer/object/CRenderableObject.h"

class CBitmapFont;

class COctreePauseloader : public CRenderableObject, public CGameBehavior
{

public:
	COctreePauseloader ( void );
	~COctreePauseloader ( void );

	void Reset ( void );

	void Update ( void );
	bool Render ( const char pass );

	bool GetDoneLoad ( void );
public:
	Vector3d	m_targetLoadcheckPosition;

private:
	glMaterial*	bgMaterial;
	glMaterial*	barMaterial;

	glMaterial*		matFntLoader;
	CBitmapFont*	fntLoaderText;

	Real		currentPercentage;
	Real		targetPercentage;

	Real		displayTime;
	Real		displayAlpha;
	Real		slowDisplayAlpha;
};

#endif//_C_OCTREE_PAUSE_LOADER_H_