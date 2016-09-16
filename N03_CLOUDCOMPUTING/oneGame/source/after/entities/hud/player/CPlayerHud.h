// This is the player hud.
//  This is the bare minimum hud. Health, inventory/hotbar, yadayada.
//  Some interactivity too, maybe.

#ifndef _C_PLAYER_HUD_
#define _C_PLAYER_HUD_

#include "engine/behavior/CGameBehavior.h"
#include "renderer/object/CRenderableObject.h"
class CAfterPlayer;

#include "after/states/inventory/CInventory.h"
#include "renderer/texture/CBitmapFont.h"

#include "core/system/CMemoryMonitor.h"
#include "renderer/camera/CRTCamera.h"
#include "renderer/texture/CRenderTexture.h"
//class CTimeProfilerUI;

class CBlockCursor;

class CActor;

// Class definition
class CPlayerHud : public CGameBehavior, public CRenderableObject
{
	ClassName( "PlayerHUD" );
public:
	// Constructor
	explicit		CPlayerHud ( CAfterPlayer* p_player, CInventory* p_inventory );
	// Destructor
					~CPlayerHud ( void );

	// Update for hud values
	void			Update ( void );
	// Render for drawing
	bool			Render ( const char pass );

private:
	// == Private Routines ==
	// Initialize all your retarded shit here
	void Initialize ( void );

	// ==Updating HUD Elements==
	// Block Cursor
	bool		bDrawBlockCursor;
	CBlockCursor*	blockCursor;
	void UpdateBlockCursor ( void );

	// ==Drawing HUD Elements==
	// Debug Elements
	void DrawPlayerPosition ( void );
	void DrawMemoryStats ( void );
	void DrawPlayerVelocity ( void );

	// Game Elements
	void DrawAimingReticle ( void );
	//void DrawPlayerHealth ( void );
	//void Draw
	void DrawWaypoints (void);

	// == Private Member Data ==
	// Needed shiz
	CAfterPlayer*	pPlayer;
	CInventory*	pInventory;
	glMaterial*		matFntDebug;
	CBitmapFont*	fntDebug;
	CBitmapFont*	fntGamehud;
	CBitmapFont*	fntGamehudSm;
	glMaterial*		matDrawDebug;

	glMaterial*		matDrawHudFnt;
	glMaterial*		matDrawHud;
	glMaterial*		matDrawReticle;
	//CTexture*		texReticle;

	// Testing rts
	/*CRTCamera*		pRTCam;
	CRenderTexture*	pRTTex;
	glMaterial*		matRT;*/

	// Put your silly things here
	bool		bDrawPlayerPosition;
	bool		bDrawMemoryMonitor;
	CMemoryMonitor*	pMemMonitor;


	bool		bDrawPlayerVelocities;

	//CTimeProfilerUI*	timeProfiler;

	bool		bDrawRenderTargets;

};

#endif