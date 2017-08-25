#ifndef _C_INVENTORY_GUI_H_
#define _C_INVENTORY_GUI_H_

#include "core/math/Vector2d.h"

#include "engine/behavior/CGameBehavior.h"

#include "renderer/object/CRenderableObject.h"
#include "renderer/texture/CBitmapFont.h"

#include "after/states/inventory/CInventory.h"

class CAfterPlayer;

class CInputControl;


class CInventoryGUI: public CGameBehavior, public CRenderableObject
{
	ClassName ("InventoryGUI");

public:
	explicit CInventoryGUI (CAfterPlayer* p_player, CInventory** p_inventory);

	~CInventoryGUI(void);

	//void Update (void) {}
	void Update (void);
	bool Render ( const char pass );
	
	// Set visiblity
	void SetVisibility ( bool visibility );
	//void SetInventoryVisibility ( bool & visibility );
	//void SetCraftingVisibility ( bool & visibility );
	//void SetEquipmentVisibility ( bool & visibility );

	//void SetDrawName ( bool doit );

	//CWeaponItem**	wHotbar;

	//void ReceiveSkill (CSkill* skill);

	//void EmptyCrafting (void);

	CWeaponItem* GetSelected (void);
	void DropSelected (void);

	void UILimitPosition ( Vector2d& position, const Vector2d& size );

	void OpenInventory (CInventory* new_inventory);

	bool GetIsVisible (void);

	void ReceiveSelected (CWeaponItem* selected);

	bool ValidClick (void);

private:
	// Constant list for internal tracking of non-backpack 'bags'
	enum eBag
	{
		// "DROP" denotes outside inventory.
		// The held item will be dropped on the ground when the mouse is clicked.
		DROP	= -5,
		// Window drag state
		DRAG	= -4,
		// Equipment window
		WEAR	= -3,
		// Crafting result
		RESULT	= -2,
		// Hotbar
		HOTBAR	= -1,
		// Crafting window
		CRAFT	= 0
	};

	// Struct sWindowState
	// Stores basic information about inventory GUI parts
	struct sWindowState
	{
		bool open;
		bool minimized;
		bool dirty_state;
		Vector2d position;	// In 0-1 Y-aligned coordinates
		Vector2d size;
		int substate;

		// Default values
		sWindowState (void) :
			open(true), minimized(false), position(Vector2d(0,0)), size(Vector2d(1,1)), substate(0), dirty_state(false)
		{}
	};

	//Don't need an initializer function just yet
	//void Initialize (void);

	// ========================
	// Draw Functions
	void DrawItems (void);
	//void DrawHotbar (void);
	//void DrawMenuHotbar (void);
	//void DrawWearables (void);
	//void DrawPicked (void);
	void DrawTooltips (void);
	//void DrawCrafting (void);

	// Crafting Draw
	//void DrawCraftingDefault (void);
	//void DrawCraftingForge (void);

	// ========================
	// Mouse control
	void GetMouseInfo (void);
	//void GetCraftingMouseInfo (void);

	void DoClickyEmpty (void);
	void DoClickyFull (void);
	CWeaponItem* DoRightClicky (void);
	void DragWindows (void);
	void MinCloseWindows (void);
	void ResolutionUpdate (void);

	CInventory**	pInventory;
	CInventory*		pTryingThisOut;
	CAfterPlayer*	pPlayer;
	
	CWeaponItem*	wPicked;

	float			width_spacing;

	short			sBag;
	short			sSlotX; // If non-negative, denotes X position in current bag. If negative and sBag is DRAG, denotes the current bag.
	short			sSlotY;

	short			sMouseControlStyle;

	float			sMouseX;
	float			sMouseY;
	float			sPrevMouseX;
	float			sPrevMouseY;

	bool			bDrawInventory;

	Color			cDrawItemFont;

	sWindowState	sItems;

	glMaterial*		matFntDebug;
	CBitmapFont*	fntDebug;
	glMaterial*		matDrawDebug;

	CInputControl*	input;
};
#endif