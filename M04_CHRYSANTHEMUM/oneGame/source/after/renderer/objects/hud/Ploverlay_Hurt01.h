

#ifndef _PL_OVERLAY_HURT_01_H_
#define _PL_OVERLAY_HURT_01_H_

#include "engine/behavior/CGameBehavior.h"
#include "renderer/object/CRenderableObject.h"

class Ploverlay_Hurt01 : public CGameBehavior, public CRenderableObject
{
public:
	// Constructor
	explicit		Ploverlay_Hurt01 ( void );
	// Destructor
					~Ploverlay_Hurt01 ( void );

	// Update for hud values
	void			Update ( void );
	// Render for drawing
	bool			Render ( const char pass );


	Real			mHurtValue;
private:
	glMaterial*		mMaterial;

};

#endif//_PL_OVERLAY_HURT_01_H_
