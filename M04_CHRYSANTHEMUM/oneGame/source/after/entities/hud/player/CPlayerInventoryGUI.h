#ifndef _C_PLAYER_INVENTORY_GUI_H_
#define _C_PLAYER_INVENTORY_GUI_H_

#include "renderer/object/CRenderableObject.h"
#include "after/states/inventory/CPlayerInventory.h"
#include "engine/behavior/CGameBehavior.h"
#include "renderer/texture/CBitmapFont.h"
#include "after/states/inventory/CCrafting.h"
#include "core/math/Vector2d.h"
#include "CInventoryGUI.h"

#include "core/math/Rect.h"

class CAfterPlayer;

class CWearableItem;
//glMaterial.h
//CRTCamera.h
//CRenderTexture.h
class CInputControl;
class CPlayerLogbook;

class CPlayerInventoryGUI: public CGameBehavior, public CRenderableObject
{
	ClassName ("PlayerInventoryGUI");

public:
	explicit CPlayerInventoryGUI (CAfterPlayer* p_player, CInventory** p_inventory, CRecipeLibrary* p_lib, CCrafting* p_craft, CPlayerLogbook* p_logbook, CInventoryGUI* p_chest );

	~CPlayerInventoryGUI(void);

	//void Update (void) {}
	void Update (void);
	bool Render ( const char pass );
	
	// Set visiblity
	void SetVisibility ( bool visibility );
	void SetInventoryVisibility ( bool & visibility );
	void SetCraftingVisibility ( bool & visibility );
	void SetEquipmentVisibility ( bool & visibility );

	void SetDrawName ( bool doit );

	CWeaponItem**	wHotbar;

	void ReceiveSkill (CSkill* skill);

	void EmptyCrafting (void);

	CWeaponItem* GetSelected (void);
	void DropSelected (void);

	void UILimitPosition ( Vector2d& position, const Vector2d& size );

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
	void DrawHotbar (void);
	void DrawMenuHotbar (void);
	void DrawWearables (void);
	void DrawPicked (void);
	void DrawTooltips (void);
	void DrawCrafting (void);

	// Crafting Draw
	void DrawCraftingDefault (void);
	void DrawCraftingForge (void);

	// ========================
	// Mouse control
	void GetMouseInfo (void);
	void GetCraftingMouseInfo (void);

	void DoClickyEmpty (void);
	void DoClickyFull (void);
	CWeaponItem* DoRightClicky (void);
	void DragWindows (void);
	void MinCloseWindows (void);
	void ResolutionUpdate (void);

	void DoCraftingClicky (void);
	void DoCraftingClickyFull (void);

	// ========================
	// Crafting Specific
	CWeaponItem* DoCrafting (void);
	void UpdateCrafting (void);

	// Create the forge lists to choose things from
	void CreateForgeSubclassList ( void );
	// Refresh what result types are locked with the current forge choices
	void ForgeRefreshLocks ( void );
	// Refresh (recreate) the part list. Will apply either forge or reforge rules based on input.
	void ForgeRefreshPartlist ( void );

	// ========================
	// Equipment Specific
	void ReceiveArmorList (void);

	// ========================
	// 
	void DrawNameOnSwitch (CWeaponItem* equipped);


	CCrafting*		pCraft;
	CPlayerInventory**	pInventory;
	CAfterPlayer*	pPlayer;
	CInventoryGUI*	pChest;

	CPlayerLogbook*	pLogbook;
	
	CWeaponItem*	wPicked;
	CWeaponItem*	wResult;

	CWearableItem**	pGear;
	Rect			pGearPositions [9];

	Rect			pForgePositions [7];

	float			width_spacing;

	// Bag,X,Y position specifiers. Have specific meanings depending on bags, not documented
	short			sBag;
	short			sSlotX; // If non-negative, denotes X position in current bag. If negative(?) and sBag is DRAG, denotes the current bag.
	short			sSlotY;
	short			sCurrentBag;
	short			sCraftType;

	short			sMouseControlStyle;

	float			sMouseX;
	float			sMouseY;
	float			sPrevMouseX;
	float			sPrevMouseY;
	float			sFadeIn;
	float			sSolid;
	// X Position of hotbar
	float			fHotbarX;
	// Probably not used now.
	float			fHotbarSpacing;
	// Padding between items in hotbar
	float			fHotbarPadding;
	// Base unit of spacing for the hotbar (item is fHotbarWidth x fHotbarWidth)
	float			fHotbarWidth;
	// Bottom of the hotbar
	float			fHotbarYbottom;

	bool			bDrawInventory;
	bool			bDrawHotbar;
	bool			bDrawItemName;
	bool			bFade;
	bool			bWearables;

	// Crafting info=
	// Show more crafting options
	bool			bCraftingMoreOptions;
	// Helper storage class
	struct t_forgegroup_info {
		string		name;
		ItemType	type;
		bool		enabled;
		explicit t_forgegroup_info ( const char* nname, ItemType ntype )
			: name(nname), type(ntype), enabled(false)
		{
		}
	};
	// Other crafting info stuff
	struct t_crafting_state {
		ftype  sMouseMessageTimer;
		ftype  sMouseMessageAlpha;
		string sMouseMessage;

		GenItemType	mForgeSearchParameter;
		bool	mForgeMoreOptions;
		std::vector<t_forgegroup_info> mForgeCategories;
		bool	mReforging;

		ItemType	mForgeItemType;

		/*vector<tForgePart> mForgeParts0;
		vector<tForgePart> mForgeParts1;
		vector<tForgePart> mForgeParts2;
		vector<tForgePart> mForgeParts3;*/

		std::vector<tForgePart> mForgeParts [4];
		int mForgePartSelect [4];

		// Default values
		t_crafting_state ( void )
			: sMouseMessageTimer(0), sMouseMessageAlpha(0),
			mForgeSearchParameter(WeaponItem::ItemBlade), mForgeMoreOptions(false),
			mForgeItemType(WeaponItem::TypeDefault)
		{
			mForgePartSelect[0] = 0;
			mForgePartSelect[1] = 0;
			mForgePartSelect[2] = 0;
			mForgePartSelect[3] = 0;
		}
	} mCraftingState;

	// Item font color
	Color			cDrawItemFont;

	// Window state of the inventory GUI parts
	sWindowState	sItems;
	sWindowState	sEquipment;
	sWindowState	sCrafting;
	sWindowState	sHotbar;

	// Rendering objects
	glMaterial*		matFntDebug;
	CBitmapFont*	fntDebug;
	glMaterial*		matDrawDebug;

	// Input control grabber
	CInputControl*	input;
};
#endif