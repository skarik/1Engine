// class CCharacterScreen
// Draws the player's current stats and status.
// This includes the 3-type stats and the corresponding point values (like health and mana).
// Also drawn are the current debuffs.
// If there is a point available to be spec'd out, this is where the point is placed into the offsets.

#ifndef _C_CHARACTER_SCREEN_H_
#define _C_CHARACTER_SCREEN_H_

#include "engine/behavior/CGameBehavior.h"
#include "renderer/object/CRenderableObject.h"

class CAfterPlayer;
class CPlayerStats;
class CRacialStats;

class CBitmapFont;

class CCharacterScreen : public CGameBehavior, public CRenderableObject
{
	ClassName( "CharacterScreen" );
public:
					CCharacterScreen ( CAfterPlayer* p_player, CPlayerStats* p_playerstats );
					~CCharacterScreen ( void );

					// Update for hud values
	void			Update ( void );
					// Render for drawing
	bool			Render ( const char pass );

					// Set visiblity
	void			SetVisibility ( bool visibility );
private:
	// Player Data
	CAfterPlayer*	pl;
	CPlayerStats*	pl_stats;
	CRacialStats*	race_stats;

	// Draw data
	glMaterial*		matDrawFnt;
	CBitmapFont*	fntDraw;
	glMaterial*		matDrawSys;

private:
	// Draw the stats page
	void			DrawStatsPage ( void );

};

#endif