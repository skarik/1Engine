// Base class for all weapons and items that can be picked up by players. (as well as enemies for that matter. Or just characters in general.)
// Mostly just characters in general can use these.
// Hell, these can be used when they're on the floor.

#ifndef _C_WEAPON_ITEM_
#define _C_WEAPON_ITEM_

// Includes
#include "core/containers/arstring.h"
#include "engine-common/entities/item/CItemBase.h"
#include "engine-common/entities/CActor.h"
#include "engine-common/types/Damage.h"
#include "engine-common/types/ItemProperties.h"
#include "physical/physics/CPhysics.h"
//#include "../CModel.h"
//#include "../CRigidbody.h"
#include "CWeaponItemTypes.h"
#include "after/states/weaponeffect/WeaponEffects.h"
#include "CWeaponItemFactory.h"
#include "CWeaponItemComponents.h"
//#include "../RangeVector.h"	// for RangeVector
#include "after/types/character/Animation.h"

//#include "core-ext/system/io/serializer/CBaseSerializer.h"
#include "core-ext/system/io/serializer.h"
class CRigidBody;

// String
#include <string>
using std::string;
#include <typeinfo>
using std::type_info;


// Class definition
class CInventory;
//class CActor;
class CTexture;
class CRenderTexture;
class CBitmapFont;
class CModel;

struct BlockTrackInfo;

class glMaterial;

// Class Definition
class CWeaponItem : public CItemBase
{
	BaseClass( "WeaponItem" );
	ClassName( "AbstractWeaponItem" );
public:	//==PUBLIC VARIABLE TYPE AND STRUCTURE DECLARATIONS==
	// WItemData struct
	struct WItemData
	{
		short		iItemId;		// Unique item ID
		// =========================
		arstring<128> sInfo;		// Uh, no idea (Currently used for item name)
		arstring<128> sIconOverride;
		// =========================
		bool		bCanStack;		// If item can stack
		short		iMaxStack;		// If previous value true, max stack value
		bool		bCanDegrade;	// If item can degrade
		short		iMaxDurability;	// If can degrage, max durability value
		// =========================
		bool		bIsSkill;		// If item isn't actually an item
		bool		bIsTradable;
		// =========================
		ItemType	eItemType;		// Specific item type
		GenItemType eTopType;		// General item type
		ItemRarity	eItemRarity;	// Item rarity
		// =========================
		float		fWeight;		// Item physical weight
		int			iHands;			// Hands needed to use this item
	};
	// WItemState struct
	struct WItemState
	{
		arstring<128>	sItemName;
		short			iCurrentStack;
		short			iCurrentDurability;
	};
	// WItemIcon struct
	struct WItemIcon
	{
		CRenderTexture*	pRenderTex;
		bool			bGenerated;
		WItemIcon ( void ) {
			pRenderTex = NULL;
			bGenerated = false;
		}
		~WItemIcon ( void );
	};
	// WTooltipStyle
	struct WTooltipStyle
	{
		CBitmapFont*	fontTexture;
		glMaterial*		matFont;
		glMaterial*		matBg;
	};
	// WReticleStyle
	struct WReticleStyle
	{
		glMaterial*		matGeneralHud;
		glMaterial*		matReticle;
		// 0 left
		// 1 right
		// 2 bottom lefty :3
		// 3 bottom right
		uchar			hand_id;
	};

public:
	// Constructor for initializing default values
	// This NEEDS to be called by derived classes.
	// This is done by calling this function in the initializer list. For example, if a class were
	//  named WeaponP90, the constructor definition should look like the following:
	//	WeaponP90::WeaponP90 ( void ) : CWeaponItem( ItemData() )
	// This initializes the class's "system" values to an initial value, as well as performing certain initializing code
	explicit CWeaponItem ( const WItemData & );
	// OnCreate for initializing item ID's to proper values
	// This should be called by the engine to check for user errors. Override this function almost never.
	void OnCreate ( void );
	// Destructor frees any model, collider, or rigidbody in use by the instance.
	// The means that while the variables pBody, pCollider, and pModel are free for use in derived classes,
	//  keep in mind that this base class retains ownership of the pointers.
	~CWeaponItem ( void );

	// Function for initializing the collision/physics
	// If this function is not overloaded, it will simply create a bounding box based on the bounding box of...well, it creates a bounding box.
	// Override this function is you want to have your own collision mesh on the weapon
	virtual void CreatePhysics ( void );

	// Function for copying objects. This by default only copies the essential variables.
	// However, it can be overridden (as will be necessary for more complicated classes with various constructors)
	CWeaponItem& operator= ( const CWeaponItem & original ) { return (*this)=(&original); };
	virtual CWeaponItem& operator= ( const CWeaponItem * original );
	
	// Function for comparing objects. This should return true when objects can be stacked in the inventory.
	virtual bool IsEqual ( CWeaponItem* compare ) { return weaponItemData.iItemId == compare->weaponItemData.iItemId; };

	// Functions for saving and loading.
	// Override this function to get specific item data saved. For example, the randomized weapon class needs to save its properties.
	// You return a buffer of binary data (stored as an array of char) and the length of the new buffer.
	// The saving object gets the ownership of the buffer.
	//virtual char* SaveData ( int& iBufferLength ) { return NULL; }
	// Override this function to get specific item data loaded. If you override the SaveData function, you should be overriding this one!
	// Both a buffer of binary data and the length of the buffer is sent in. The buffer data is specific to the object type.
	// The loading object retains the ownership of the buffer.
	//virtual void LoadData ( char* sBuffer, int iBufferLength ) { ; }
	// Serialization system
	//template <class Serializer>
	//void			serialize ( Serializer &,  const uint );
	virtual void	serialize ( CBaseSerializer &, const uint );

	// These are the lookAt callbacks, which are identical to the ones in the CActor class. With the default player class implementation, these are called when a player is
	// looking at an object. They are not called by any other class. These can be ignored, or you can put on a snazzy effect when the player is looking at the item.
	// Note that when looking at a class instance inheriting both from CActor and CWeaponItem, the CWeaponItem call with take precedence.
	virtual void	OnInteract	( CActor* interactingActor ) {}
	virtual void	OnInteractLookAt	( CActor* interactingActor ) {}
	virtual void	OnInteractLookAway	( CActor* interactingActor ) {}

	// This is called when the HUD for the current weapon needs to be drawn. On default, this draws an aiming reticle.
	virtual void	OnDrawHUD ( const WReticleStyle& n_reticleStyle );

	// These are the equip calls, which are called when the item is 'equipped.' The means the item was put on the belt or equipped otherwise.
	virtual void	OnPlaceinBelt ( CActor* interactingActor ) {}
	virtual void	OnEquip ( CActor* interactingActor ) {}
	virtual void	OnUnequip ( CActor* interactingActor ) {}

	// These are the melee system callbacks, for stopping control or other specific behavior when involved in combat.
	virtual void	OnBlockAttack ( Damage& resultantDamage ) {}

	// Activate the item. Weapons take the firing type as the argument.
	// Override this function to give your items a behavior.
	// Return false if this item cannot be used this way.
	virtual bool Use ( int ) =0;
	// Activating the item has just started. Weapons take the firing type as the argument.
	virtual void StartUse ( int ) {}
	// Activating the item has ended. Weapons take the firing type as the argument.
	virtual void EndUse ( int ) {}
	// Attack with the item. This is, straight up, an attack.
	// Triggered when the owner character animation recieves an 'attack' event.
	virtual void Attack ( XTransform& ) {}
	// Subdraw the item. This is called when the item is selected again when already out.
	// This is also called when the "draw" button is pressed.
	virtual void SubDraw ( void ) {}

	// Return stance of the item. Weapons have a default stance associated with them.
	// This stance affects the defence calculations of the player
	virtual Item::EquipStance GetStance ( void ) { return Item::StanceOffense; }

	// Change who's "holding" this item. NULL if belongs to world
	// Note that setting this object's owner to null WILL force this weapon to belong to the world.
	// Inventories must check if their weapons are still on them, as the weapon does not send any message when this happens!
	void SetOwner ( CActor* pNewOwner );
	// Change the current hold state. Defaults to a world 'hold.'
	// Values are in the CWeaponItem::HoldState enumeration.
	void SetHoldState ( Item::HoldState nHoldState = Item::None );
	// Change the current inventory. NULL if belongs to world
	// This is used for direct inventory queries on certain objects
	void SetInventory ( CInventory* pNewInventory );
	// Get who's "holding" this item. Returns NULL if belongs to world
	CActor* GetOwner ( void );
	// Get the current hold state.
	Item::HoldState GetHoldState ( void );
	// Returns the hold type of the item
	Item::HoldType GetHoldType ( void );

	// Request a holstering motion
	void RequestHolster ( Item::HoldState nNextHoldState = Item::OnBelt );
	// Have we put this item away?
	bool IsHolstered ( void );
	// Are we in the process of putting this item away?
	bool IsHolstering ( void );
	// Take out the item
	void Draw ( Item::HoldState nNextHoldState = Item::Holding );

	// Get the item's current name. For item info and such.
	const char* GetItemName ( void ) const;
	// Returns the item data in a CWeaponItem::WItemData via reference return.
	// This holds the information string of the item, if the item is stackable, and the item ID used for save files.
	void GetItemData ( WItemData& ) const;
	const WItemData* GetItemData ( void ) const;
	// Returns the item data in a CWeaponItem::WItemState via reference return.
	// This holds the name string of the item, the current stack count of the item, and other current data.
	void GetItemState ( WItemState& ) const;
	const WItemState* GetItemState ( void ) const;

	// LateUpdate for common gun code.
	// If you derive and override LateUpdate, you MUST call this base class's code.
	// This can be done via CWeaponItem::LateUpdate() in the derived class's LateUpdate function.
	void LateUpdate ( void );
	// Behavior that you want the weapon to have should be defined in the derived class's Update or PostUpdate function.

	// FixedUpdate for physics gun code.
	// If you derive and override FixedUpdate with your own special physics behavior, you MUST call this to keep the basic functionality.
	// Place CWeaponItem::FixedUpdate() in the derived class's FixedUpdate function to get this behavior.
	void FixedUpdate ( void );

	// This checks of the cooldown has expired for the designated use.
	bool CanUse ( int ) const;
	// GetIsBusy for other code.
	// If the item is busy, it's a sign that whoever is holding it shouldn't be trying to change weapons or drop the item.
	// This is virtual, so you can declare your own behavior in your own classes.
	// By default, the item returns busy if any of the cooldowns are active or if in draw/holster state
	virtual bool GetIsBusy ( void ) const { return (!CanUse(0) || !CanUse(1) || !CanUse(2)) || holsterActive; }

	// Interrupt public call for stopping actions.
	// This is called whenever the weaponItem owner switches weapons, gets stunned, or some other interrupting action happens.
	// This is virtual, and you SHOULD declare your own behaviour for your classes.
	// The default implementation stops holstering.
	virtual void Interrupt ( void ) { ; }

	//Set the info(name) of the item
	void SetInfo (arstring<128> & name);
	//Return the size of the stack of the item
	short GetStackSize (void);
	//Return the maximum stack size of the item
	short GetMaxStack (void);
	//Set the current stack size of the item
	void SetStackSize (short pile);
	//Add to the current stack size of the item
	void AddToStack (short stack);
	// Return the durability of the item
	short GetDurability ( void );
	// Return the maximum durability of the item
	short GetMaxDurability ( void );
	// Set the durability of the item
	void SetDurability (int con);
	//Returns the ID of the current item
	short GetID (void);
	//Return if item can stack
	bool GetCanStack (void);
	// Return if item has durability
	bool GetCanDegrade ( void ) const;
	// Returns if the item is actually a skill
	bool GetIsSkill (void) const;
	// Returns what type of item it is
	ItemType GetItemType (void);
	// Returns number of needed hands to use this time
	int	GetHands (void);
	// Sets main hand this item is being used by
	void SetHand ( const int );
	
	// Returns the rigidbody of the item
	CRigidBody* GetRigidbody ( void );
	// Returns the model of the item
	CModel*		GetModel ( void );

	// Sets the transform to set up a camera for rendering an icon
	virtual void SetInventoryCameraTransform ( XTransform & );
	virtual void SetInventoryCameraWidth ( ftype & );
	// Grabs the icon for this object
	virtual CTexture* GetInventoryIcon ( void );
	// Renders the model for the icon
	virtual void InventoryIconRenderModel ( void );
	// Draws the weapon tooltip overlay
	virtual void DrawTooltip ( const Vector2d &, const WTooltipStyle & );

	// THE FOLLOWING HELPERS SHOULD BE RETHOUGHT. THE HITTYPE MAY NEED TO GO TO ENGINE-COMMON
	// Helper function used for raycasting, since it's not trivial
	/*static Item::HitType Raycast (
		const Ray& ray, const ftype& range,
		RaycastHit* outHitResult, BlockTrackInfo* outBlockInfo, CGameBehavior** outHitBehavior, ftype* outDamageMultiplier,
		const uint32_t collisionFilter=Physics::GetCollisionFilter(Layers::PHYS_BULLET_TRACE,0,31), void* mismatch=NULL
		);
	static Item::HitType Raycast (
		const Ray& ray, const ftype& range,
		RaycastHit* outHitResult, CGameBehavior** outHitBehavior, ftype* outDamageMultiplier,
		const uint32_t collisionFilter=Physics::GetCollisionFilter(Layers::PHYS_BULLET_TRACE,0,31), void* mismatch=NULL
		);
	static Item::HitType Linecast (
		const Ray& ray, const ftype& range, physShape* pShape,
		RaycastHit* outHitResult, CGameBehavior** outHitBehavior, ftype* outDamageMultiplier,
		const uint32_t collisionFilter=Physics::GetCollisionFilter(Layers::PHYS_BULLET_TRACE,0,31), void* mismatch=NULL
		);
	static Item::HitType GetHitType (
		const sCollision& collision,
		CGameBehavior** outHitBehavior, ftype* outDamageMultiplier );*/
protected:
	// This is for setting the initial item data. You'll want to edit the weaponItemData struct here.
	virtual WItemData ItemData ( void )
	{
		WItemData data;

		data.iItemId = -1;

		data.sInfo = "";
		data.bCanStack = false;
		data.iMaxStack = 1;
		data.bIsSkill		= false;
		data.bIsTradable	= false;

		data.bCanDegrade	= false;
		data.iMaxDurability	= 100;

		data.eItemType	= WeaponItem::TypeDefault;
		data.eTopType	= WeaponItem::ItemMisc;
		data.eItemRarity= WeaponItem::RarityNone;

		data.fWeight = 5.0f;
		data.iHands = 1;

		return data;
	};
#define WITEM_DATA_DEFINE protected: virtual WItemData ItemData ( void )
#define WITEMDATA_DEFAULTS WItemData wpdata = CWeaponItem::ItemData();

	// This sets the cooldown for the designated use.
	void SetCooldown ( int, float );
	// This gets the cooldown time left for the given use.
	ftype	GetCooldown ( int ) const;

	// This tosses the item
	void Toss ( Vector3d force );
	// This tosses a stack from the item
	CWeaponItem* TossFromStack ( Vector3d force, short stackCount=1 );

	// This calculates the current hold transform, based on the current hold type
	void	GetHoldTransform ( Vector3d &, Quaternion & );

	// This sets the view angle offset of the owner
	void	SetViewAngleOffset ( const ftype offset = 0 );

	// Sets if items should update their tracker information about the world.
	// Tracker information is used to manage unloading items when out of range.
	static void SetTrackerState ( const bool update_information );

	// The following draw pieces of the weapon tooltip overlay
	void	TooltipDrawBackground ( const Vector2d &, const WTooltipStyle & );
	void	TooltipDrawName ( const Vector2d &, const WTooltipStyle & );
	void	TooltipDrawRarity ( const Vector2d &, const WTooltipStyle & );
	void	TooltipDrawDescription ( const Vector2d &, const WTooltipStyle & );
	void	TooltipDrawPicture ( const Vector2d &, const WTooltipStyle & );

protected:
	// Item information that can be seen by derived classes.
	Item::HoldType	holdType;
	Item::HoldState	holdState;
	CActor*		pOwner;
	CInventory*	pInventory;
	WItemData	weaponItemData;
	WItemState	weaponItemState;
	CActor*		pOriginalOwner;

	bool		bCanPickup;

	Debuffs::WeaponEffects effects;

	// Current hand being held by.
	int			mHand;

	// Pointers to physical and visual presense of the item
	CRigidBody*	pBody;
	CCollider*	pCollider;
	CModel*		pModel;
	
	// Offset when being held
	Vector3d	vHoldingOffset;

	// Inventory properties
	WItemIcon	icon;

	// World Tracker Properties
	//CBoob*		pCurrentSector;	// pointer to owning area (unreliable)
	//RangeVector	vCurrentSector;	// index to owning area (reliable)
	//static bool	bCheckCurrentSector;	// if should check current sector with the terrain

	// Animation info
	//string		sAnimationIdle;
	NPC::eItemAnimType	mAnimationIdleAction;
	int			iAnimationSubset;
	ftype		mAnimationIdleArgs;

private:
	// Cooldown variable. This is taken care of by this class, and so isn't needed to be seen by derived classes.
	float		fCooldown[9];
	// Handing belt index variable. This is set by owning classes, so the weapon knows which holding point to ask for.
	char		iBeltIndex;
	// Timer to turn the object into critical collision mode
	float		fCollisionTimer;

	// Information about toss to execute on the next physics update
	bool		bHasToss;
	Vector3d	vTossVector;

	// Holstering info
	Item::HoldState	holsterTargetHoldstate;	// Next holdstate to go to
	float		holsterTimer;			// Time to keep track of holstering motion
	bool		holsterAway;			// Are we putting this away?
	bool		holsterInterrupted;		// Was the holster action interrupted?
	bool		holsterActive;			// Is the holster code active?
	float		holsterTime_Away;		// Time it takes to put item away
	float		holsterTime_Equip;		// Time it takes to take out item

public:
	static string ItemName ( const short itemid );
	static string ItemName ( const type_info & itemid );

	static CWeaponItem*	Instantiate ( const short itemid );
	static CWeaponItem* Instantiate ( const string & itemid );
	static CWeaponItem* Instantiate ( const type_info & itemid );
	template <class WI> static CWeaponItem* Instantiate ( void );

	CWeaponItem*	Dupe ( void );

};

#endif