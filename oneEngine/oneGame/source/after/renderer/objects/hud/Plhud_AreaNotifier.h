
#ifndef _PL_HUD_AREA_NOTIFIER_H_
#define _PL_HUD_AREA_NOTIFIER_H_

#include "core/containers/arstring.h"
#include "engine/behavior/CGameBehavior.h"
#include "renderer/object/CRenderableObject.h"

class CBitmapFont;

// Plhud_AreaNotfier is the popup that shows on screen for arriving in areas.
// It draws the text it's given, and then dissapears
// Input is limited to 256 characters
class Plhud_AreaNotifier : public CGameBehavior, public CRenderableObject
{
	ClassName( "Plhud_AreaNotifier" );
public:
	// Constructor
	explicit		Plhud_AreaNotifier ( const char* nStringToDraw, const char* nPretitle="", const char* nPosttitle="" );
	// Destructor
					~Plhud_AreaNotifier ( void );

	//
	void			Update ( void );
	// Render routine
	bool			Render ( const char pass );
private:
	arstring<256>	mString;
	arstring<256>	mPrestring;
	arstring<256>	mPoststring;
	ftype			mLerpTimer;

	static CBitmapFont*	fntNotifier;
	static glMaterial*	matNotifierDrawer;
};

#endif//_PL_HUD_AREA_NOTIFIER_H_