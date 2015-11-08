
#ifndef _C_STUDIO_SPLASH_H_
#define _C_STUDIO_SPLASH_H_

#include "engine/behavior/CGameBehavior.h"
#include "renderer/object/CRenderableObject.h"

class CTexture;
class glMaterial;

class CStudioSplash : public CGameBehavior, public CRenderableObject
{

public:
	CStudioSplash ( void );
	~CStudioSplash ( void );

	void Update ( void );
	bool Render ( const char pass );

	bool Done ( void );

private:
	int fadeState;
	ftype fader;

	glMaterial*	matBase;
	glMaterial*	matPrim;

	CTexture*	texBase;
};

#endif//_C_STUDIO_SPLASH_H_