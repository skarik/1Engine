
#ifndef _PL_HUD_TEXT_NOTIFIER_H_
#define _PL_HUD_TEXT_NOTIFIER_H_

#include "core/containers/arstring.h"
#include "engine/behavior/CGameBehavior.h"
#include "renderer/object/CRenderableObject.h"

class CBitmapFont;
class glMaterial;

// Plhud_AreaNotfier is the popup that shows on screen for arriving in areas.
// It draws the text it's given, and then dissapears
// Input is limited to 256 characters
class Plhud_TextNotifier : public CGameBehavior, public CRenderableObject
{
	ClassName( "Plhud_TextNotifier" );
public:
	// Constructor
	explicit		Plhud_TextNotifier ( const char* nStringToDraw, const Vector2d& renderPosition=Vector2d(0.5f,0.8f), bool drawCentered=true );
	// Destructor
					~Plhud_TextNotifier ( void );

	//
	void			Update ( void );
	// Render routine
	bool			Render ( const char pass );
private:
	arstring<256>	mString;
	ftype			mLerpTimer;
	Vector2d		mDrawPos; 
	bool			mDrawCentered;

	static CBitmapFont*	fntNotifier;
	static glMaterial*	matNotifierDrawer;
};

#endif//_PL_HUD_TEXT_NOTIFIER_H_