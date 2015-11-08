
// Goggles 01 overlay.
// Creates chromatic aberration shader as well

#ifndef _PL_OVERLAY_GOGGLES_H_
#define _PL_OVERLAY_GOGGLES_H_

#include "engine/behavior/CGameBehavior.h"
#include "renderer/object/CRenderableObject.h"

class CDepthSplitShader;

class Ploverlay_Goggles01 : public CGameBehavior, public CRenderableObject
{
public:
	// Constructor
	explicit		Ploverlay_Goggles01 ( void );
	// Destructor
					~Ploverlay_Goggles01 ( void );

	// Update for hud values
	void			Update ( void );
	// Render for drawing
	bool			Render ( const char pass );


private:
	glMaterial*			mMaterial;
	CDepthSplitShader*	m_screenshader;

};

#endif//_PL_OVERLAY_GOGGLES_H_
