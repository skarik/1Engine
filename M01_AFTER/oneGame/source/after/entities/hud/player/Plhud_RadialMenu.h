
#ifndef _PL_HUD_RADIAL_MENU_H_
#define _PL_HUD_RADIAL_MENU_H_

#include "engine/behavior/CGameBehavior.h"
#include "renderer/object/CRenderableObject.h"

#include "renderer/texture/CBitmapFont.h"

// Prototypes
class CAfterPlayer;

// Plhud_RadialMenu is the popup menu
// It draws the text it's given, and then dissapears
// Input is limited to 256 characters
class Plhud_RadialMenu : public CGameBehavior, public CRenderableObject
{
	ClassName( "Plhud_RadialMenu" );
public:
	// Constructor
	explicit		Plhud_RadialMenu ( CAfterPlayer* nTargetPlayer );
	// Destructor
					~Plhud_RadialMenu ( void );

	//
	void			Update ( void );
	// Render routine
	bool			Render ( const char pass );

	// Notice for 'Kill'
	// The command that is being hovered over is to be executed.
	void			NotifyKill ( void );
private:
	//arstring<256>	mString;
	//ftype			mLerpTimer;
	CAfterPlayer*		targetPlayer;
	bool			hasKillSignal;

	ftype			fCurrentSize;
	ftype			spaceDiv;
	int				currentSelection;

	Vector2d		selectionPosition;

	enum eCommandList
	{
		rcl_MOVE_HERE,
		rcl_FOLLOW_ME,
		rcl_LOOK_HERE,
		rcl_ATTACK_THIS,
		rcl_NIX_THAT,
		rcl_BUFF_ME,
		rcl_STAND_GROUND
	};

	std::vector<eCommandList>	commandList;

	static CBitmapFont*	fntRadial;
	static glMaterial*	matRadialDraw;
};

#endif//_PL_HUD_RADIAL_MENU_H_