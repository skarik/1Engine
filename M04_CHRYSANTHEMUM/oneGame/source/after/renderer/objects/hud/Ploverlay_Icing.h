

#ifndef _PL_OVERLAY_ICING_H_
#define _PL_OVERLAY_ICING_H_

#include "engine/behavior/CGameBehavior.h"
#include "renderer/object/CRenderableObject.h"

class Ploverlay_Icing : public CGameBehavior, public CRenderableObject
{
public:
	// Constructor
	explicit		Ploverlay_Icing ( void );
	// Destructor
					~Ploverlay_Icing ( void );

	// Update for hud values
	void			Update ( void );
	// Render for drawing
	bool			Render ( const char pass );


	Real			mIcingValue;
private:
	glMaterial*		mMaterial;

};

#endif//_PL_OVERLAY_ICING_H_
