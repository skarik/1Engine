// class CPlayerHudStatus
//  Draws the health, stamina, and mana bars on screen.
//  Draws debuffs, hurt direction indicators, and controls damaged view effects.

#ifndef _C_PLAYER_HUD_STATUS_H_
#define _C_PLAYER_HUD_STATUS_H_

// Base classes
#include "engine/behavior/CGameBehavior.h"
#include "renderer/object/CRenderableObject.h"

#include "renderer/texture/CBitmapFont.h"
#include "renderer/material/glMaterial.h"

// Prototpes
class CAfterPlayer;
class CActor;

// Class definition
class CPlayerHudStatus : public CGameBehavior, public CRenderableObject
{
	ClassName( "PlayerHUDStatus" );
public:
	// Constructor
	explicit CPlayerHudStatus ( CAfterPlayer* p_player );
	// Destructor
	~CPlayerHudStatus ( void );

	// Update for hud values
	void Update ( void );
	// Render for drawing
	bool Render ( const char pass );

	// Set killtarget
	void		UpdateKillTarget ( CActor* new_target );
private:
	// == Private Routines ==
	// Initialize all your retarded shit here
	void Initialize ( void );

private:
	// == Private Member Data ==
	// Needed shiz
	CAfterPlayer*	pPlayer;
	CActor*		killTarget;
	Real		killTimer;

	Real		factorTimer [3];
	Real		factorAlpha [3];

	// Materials
	CBitmapFont* fntPnts;
	glMaterial* matFontPnts;
	glMaterial*	matPointBars;
	glMaterial*	matPointBarsBG;
	glMaterial*	matPointBarsEnds;

	glMaterial*	matBuffCircle;

};

#endif