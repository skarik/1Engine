
#ifndef _C_REGION_MAP_TESTER_H_
#define _C_REGION_MAP_TESTER_H_

// Includes
#include "engine/behavior/CGameBehavior.h"

#include "renderer/object/CRenderableObject.h"
#include "renderer/texture/CRenderTexture.h"

class CRegionMapTester : public CGameBehavior, public CRenderableObject
{

public:
					CRegionMapTester ( void );
					~CRegionMapTester ( void );

	void			Update ( void );
	bool			Render ( const char pass );

private:
	glMaterial*		myMaterial;
	glMaterial*		myMaterial2;
	bool			bNeedUpdate;

	uint32_t		regionmap [64][64];

	uint32_t&	Map ( const int32_t& x, const int32_t& y ) {
		return regionmap[x+32][y+32];
	}

	CRenderTexture*	mRT;
};


#endif//_C_REGION_MAP_TESTER_H_