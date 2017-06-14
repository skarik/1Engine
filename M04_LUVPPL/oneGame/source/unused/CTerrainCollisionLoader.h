
#ifndef _C_TERRAIN_COLLISION_LOADER_H_
#define _C_TERRAIN_COLLISION_LOADER_H_

#include <thread>
#include "engine/behavior/CGameBehavior.h"
#include "renderer/object/CRenderableObject.h"

class CBitmapFont;

class CTerrainCollisionLoader
	: public CGameBehavior, public CRenderableObject
{
public:
					CTerrainCollisionLoader ( void );
					~CTerrainCollisionLoader( void );

	void			Update ( void );
	bool			Render ( const char pass );

private:
	glMaterial*		matFont;
	CBitmapFont*	fntDraw;
	ftype			drawAlpha;

	int				iOffset;
private:
	// Loader counters
	struct sCollisionLoader;

	void			TerraLoader ( void );

	bool			bContinueThread;
	std::thread		mtThread;			
};

struct CTerrainCollisionLoader::sCollisionLoader
{
	CTerrainCollisionLoader*	caller;
	void	operator() ( void );
};

#endif//_C_TERRAIN_COLLISION_LOADER_H_