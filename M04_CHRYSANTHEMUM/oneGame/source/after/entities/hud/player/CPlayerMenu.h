// This is the player hud.
//  This is the bare minimum hud. Health, inventory/hotbar, yadayada.
//  Some interactivity too, maybe.

#ifndef _C_PLAYER_MENU_H_
#define _C_PLAYER_MENU_H_

#include "engine/behavior/CGameBehavior.h"
#include "renderer/object/CRenderableObject.h"

#include "after/states/inventory/CInventory.h"
#include "renderer/texture/CBitmapFont.h"
#include "CSkillTreeGUI.h"

class CPlayer;
class CPlayerInventoryGUI;
class CPlayerLogbook;
class CQuestSysRenderer;
class CSkillTreeGUI;
class CCharacterScreen;
class CSkillTree;
class CDialogueGUI;
class CInventoryGUI;

// Class definition
class CPlayerMenu : public CGameBehavior, public CRenderableObject
{
	ClassName( "PlayerHUD" );
public:
	// Constructor
	explicit CPlayerMenu (
		CAfterPlayer* p_player,
		CPlayerInventoryGUI* p_inventorygui,
		CPlayerLogbook* p_logbook,
		CQuestSysRenderer* p_questlog,
		CSkillTreeGUI* p_skilltree,
		CCharacterScreen* p_charscreen,
		CDialogueGUI* p_dialogue_gui,
		CInventoryGUI* p_chest_gui );
	// Destructor
	~CPlayerMenu ( void );
	// Update for hud values
	void Update ( void );
	void LateUpdate ( void ) { stop = false; };
	// Render for drawing
	bool Render ( const char pass );

	void TurnOffInventory (void);
private:

	bool HideAll ( void );

	// ==Updating HUD Elements==
	CAfterPlayer*	pPlayer;

	enum eHUDSelection
	{
		HS_CHARACTER = 0,
		HS_INVENTORY,
		HS_EQUIPMENT,
		HS_CRAFTING,
		HS_LOGBOOK,
		HS_SKILLTREE,
		HS_QUESTLOG,
		HS_DEBUGSPAWNER,
		HS_CHEST,
		HS_NONE
	};
	eHUDSelection	iHudSelectState;
	eHUDSelection	iHudSelectMouseover;
	bool			bShowHudState;
	bool			bShowComponentState[HS_NONE];
	CPlayerInventoryGUI *		pInventoryGUI;
	CPlayerLogbook*		pLogbookGUI;
	CQuestSysRenderer*	pQuestlogGUI;
	CSkillTreeGUI*		pSkillGUI;
	CCharacterScreen*	pCharacterGUI;
	CDialogueGUI*		pDialogueGUI;
	CInventoryGUI*		pChestGUI;

	void InitGUInterface ( void );
	void UpdateGUInterface ( void );
	void DrawGUInterface ( void );

	enum ePAZSelection
	{
		PZ_QUIT = 0,
		PZ_NONE
	};
	ePAZSelection	iPazSelectMouseover;
	bool			bShowPauseMenu;

	void UpdatePauseMenu ( void );
	void DrawPauseMenu ( void );

	// == Materials and Textures for Rendering ==
	CBitmapFont*	fntMenu;
	glMaterial*		matfntMenu;
	glMaterial*		matMenu;

	// == GUI Positioning ==
	Vector2d	vMenuPos;
	Vector2d	vMenuButtonSize;
	ftype		xMenuOffsets[HS_NONE-1];

	bool		stop;
};

#endif