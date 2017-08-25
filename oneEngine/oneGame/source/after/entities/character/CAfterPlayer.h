
#ifndef _C_M_C_C_PLAYER_H_
#define _C_M_C_C_PLAYER_H_

// == Includes ==

#include "after/entities/character/CCharacter.h"
#include "after/entities/hud/player/CPlayerHud.h"

#include "after/states/player/CPlayerStats.h"
#include "after/states/inventory/CPlayerInventory.h"
#include "after/states/inventory/CInventory.h"
#include "after/states/inventory/CCrafting.h"
#include "after/states/NPC_CombatState.h"

#include "after/interfaces/CAfterCharacterAnimator.h"

#include "after/types/character/Dialogue.h"
#include "after/types/character/MovementEnum.h"

class CCharacterMotion;
class CVehicleActor;
class CPlayerHudStatus;
class CPlayerInventoryGUI;
class CPlayerLogbook;
class CPlayerMenu;
class CCharacterScreen;
class CRecipeLibrary;
class CSkillTreeGUI;
class CSkillTree;
class CDialogueGUI;
class CInventoryGUI;
class CQuestSystem;
class CQuestSysRenderer;
class CAudioListener;

class Plhud_RadialMenu;
class Ploverlay_Icing;

class CBloomShader;
class CInputControl;

#include "after/entities/character/CCharacter.h"

//===============================================================================================//
// == Class Definition ==
//===============================================================================================//
//class CMccPlayer : public CPlayer
class CAfterPlayer : public CCharacter
{
	ClassName( "CPlayer" );
public:
	//=========================================//
	// Identifier Enumeration
	virtual NPC::eCharacterType CharacterType ( void ) override { return NPC::CHARACTER_TYPE_PLAYER; }

	//=========================================//
	// ===Constructor and Destructor===

	// Constructor
	explicit CAfterPlayer ( CPlayerStats* );
	// Destructor
	~CAfterPlayer ( void );

	//=========================================//
	// ===Step Functions===

	void Update ( void );
	void LateUpdate ( void );
	void FixedUpdate ( void );

	//=========================================//
	// === Public Getters ===

	CPlayerStats*	GetPlayerStats ( void )
	{
		return pl_stats;
	}
	CRacialStats*	GetRacialStats ( void )
	{
		return pl_race_stats;
	}
	CInventory*	GetInventory ( void ) override
	{
		return pMyInventory;
	}
	// Get the motion state
	CCharacterMotion* GetMotionState ( void ) override
	{
		return m_motion;
	}
	// Get the combat state
	NPC::sCombatInfo*	GetCombatState ( void ) override
	{
		return &combatInfo;
	}

	// ===Common Getters===
	// Get eye ray. Commonly used for raycasts.
	Ray			GetEyeRay ( void ) override;
	// Get aiming direction. Used for melee weapons.
	Rotator		GetAimRotator ( void ) override;
	// Get aiming arc. Used for melee weapons.
	Vector4d	GetAimingArc ( void ) override;
	
	// Get the current movement state
	//NPC::eMovementEnumeration GetMovementMode ( void ) override;

	// Get the current velocity
	Vector3d GetMotionVelocity ( void ) override;

	// Get the current rigidbody
	//CRigidBody*	GetRigidbody ( void ) override;

	// Returns a pointer to the shield. Returns NULL if no shield.
	SkillShield*	GetShield ( void ) override;
	// Returns a list of all the casting spells in a vector. Default implemenation returns a list of empty skills.
	std::vector<CSkill*>	GetCastingSkills ( void ) override;

	// Returns the bleeding out information
	bool		GetBleeding ( Real& o_bled, Real& o_threshold, Real& o_time )
	{
		o_bled		= fBleedDamage;
		o_threshold	= fBleedDamageThreshold;
		o_time		= fBleedTime;
		return bBleeding;
	}

	// === Public Setters ===
	// Set next camera mode
	void SetCameraMode ( const NPC::eCameratypeEnumeration mode = NPC::CAMERA_MATCH );
	// Set next movement state
	//void SetMovementMode ( const NPC::eMovementEnumeration mode ) override;

	// Set if can sprint
	void SetCanSprint ( bool enableSprint ) override;
	// Set the movement scale. Argument is the value the speed will be multiplied by.
	void SetMovementSpeedScale ( float ) override;

	// Sets the shield currently in use by the player
	void	SetShield ( SkillShield* ) override;


	// === General Calls ===
	// Apply stun. Returns true on successful stun. Ignores the second argument.
	bool	ApplyStun ( const char* n_stunname, const ftype n_stunlength ) override;
	// Apply linear motion offset. Returns true when alive.
	bool	ApplyLinearMotion ( const Vector3d& n_motion, const ftype n_motionlength ) override;

	// =Dialogue and Conversation Functions=
	// Start a conversation with the list of NPC/Player. The main NPC to talk with is index 0.
	void StartConversation (string conversation, std::vector<CCharacter*> person);
	// Have the player speak with the given sound file. Argument is either a sound entry or a path to a file.
	void SpeakDialogue ( const string& soundFile ) override;
	// Have the player speak a general line.
	void DoSpeech ( const NPC::eGeneralSpeechType& speechType ) override;

	// =Inventory GUI Functions=
	// Open other inventory to view
	void OpenInventoryForSwapping ( CInventory* nTargetInventory );
	// Pass item to this player's inventory
	void PassSelected (CWeaponItem* selected);
	// Turn close the inventory GUI
	void TurnOffInventoryGUI (void);


	// ===Transform Getters===
	XTransform	GetHoldTransform ( char i_handIndex=0 ) override;
	XTransform	GetBeltTransform ( char i_beltIndex ) override;


	// ===Base Animation Interface===
	void	PlayItemAnimation ( const NPC::eItemAnimType nActionName, const int nSubset, const int nHand, const float fArg, const float fAnimSpeed=0, const float fAttackSkip=0 ) override;
	Real	GetItemAnimationLength ( const NPC::eItemAnimType nActionName, const int nSubset, const int nHand ) override;
	void	PlayAnimation ( const string& sActionName ) override;


	// ==='Callbacks'===
	void OnSpawn ( void );
	void OnDeath ( Damage const& ) override;

	void	OnDamaged	( Damage const& hitDamage, DamageFeedback* dmgFeedback=NULL ) override;
	void	OnDealDamage ( Damage& hitDamage, CActor* receivingCharacter ) override;
	void	OnAnimationEvent ( const Animation::eAnimSystemEvent eventType, const Animation::tag_t tag ) override;
	void	OnEquip ( CItemBase* pItem ) override;
	void	OnUnequip ( CItemBase* pItem ) override;

	// OnGainExperience doesn't actually mean that experience has been gained, similar to OnDamaged.
	//  Based on the input's discipline, you can give certain experience types a special effects.
	// The default behavior is to just add the experience to the stats.
	void	OnGainExperience ( const Experience& incomingExperience ) override;

	void	OnReceiveSignal ( const uint64_t n_signal ) override;


	// == Character State Getters ==
	// Is this character performing a melee attack? The input to the function is the hand to check.
	// If the hand is being used with a melee attack, then return true.
	bool	IsAttackingMelee ( const short hand ) override;
	// Get the frame of the melee attack that the character is attacking. This is used for determining attack cancelling during melee attacks.
	// Attack cancelling is used for both combos and for sword clashing.
	int		GetAttackingFrame ( const short hand ) override;


	// ===Player Specific Functions==
	// 'Punches' or temporarily offset the view with the given offset.
	// This can be used for hurt effects, gun firing, and other single 'punch' camera effects.
	void	PunchView ( Vector3d const& );

	// Return the input control that the player is using
	CInputControl*	GetInputControl ( void ) { return input; };

	// ===Misc Getters===
	// Get the last looked at position
	RaycastHit& GetLookHit ( void ) {
		return rhLookAtResult;
	}
	// Get the last looked at object
	CGameBehavior* GetCurrentLookedAt ( void ) {
		return pCurrentLookedAt;
	}

	// ===Common Setters===
	// Sets the view angle. Normally, this is something like the FOV (field of vision) of a character.
	void SetViewAngle ( float = 100 );
	// Sets the offset for the view angle. This is the function you call when you want to temporarily change the view angle of a character
	// as when, for example, you want to zoom in with a Sniper Rifle.
	void SetViewAngleOffset ( float = 0 );
	// Sets the turn rate sensitivity. You don't need to do this when zooming in. That turn rate decrease is taken care of automatically.
	void SetTurnSensitivity ( float = 1 );

public: // PUBLIC FIELDS
	//===============================================================================================//
	// Input Toggles
	//===============================================================================================//

	bool	bCanMove;		// Controls if the player can edit their own movement. (not yet implemented)
	bool	bHasInput;		// Controls if the player can have any input.
	bool	bOverrideInput; // When the player can not have input, when true, this does not reset inputs to zero.
	bool	bCanMouseMove;	// Controls if the player can turn with the mouse.

protected: // PROTECTED FIELDS and ROUTINES

	friend CCharacterMotion; // TODO: remove this?

	//===============================================================================================//
	// Attached objects
	//===============================================================================================//

	CCamera*		pCamera;	// Attached camera
	CAudioListener*	pListener;	// Attached listener

	//===============================================================================================//
	// Things that happen
	//===============================================================================================//

	void DoPlayerActions ( void );

	void	DoUseCommand ( void );
	void	PickupItemsRadius ( void );
	float	fPickupTimer;
	bool	bPickupDone;

	NPC::sCombatInfo combatInfo;

	CAfterCharacterAnimator	animator;
	void SetMoveAnimation ( const NPC::eMoveAnimType );

	void DoLookAtCommands ( void );
	CGameBehavior*	pCurrentLookedAt;
	float		fMaxUseDistance;
	RaycastHit	rhLookAtResult;

	//===============================================================================================//
	// Motion state
	//===============================================================================================//

	CCharacterMotion*	m_motion;
	
	// Current normal and downward tangent of terrain
	Vector3d	vTerrainNormal;
	Vector3d	vTerrainTangent;
	ushort		iTerrainCurrentBlock;

	// If the player is in an active terrain area
	bool		bInActiveArea;
	// Motion speed multiplier (often changed by weapons)
	ftype		fMovementMultiplier;
	// Can sprint setting (often changed by weapons)
	bool		bSprintDisabled;

	// Current death timer as the player struggles for a bit of life
	//ftype		fSaviorTimer;
	ftype		fDeathTimer;
	ftype		fBleedTime;
	ftype		fBleedDamage;
	ftype		fBleedDamageThreshold;
	bool		bBleeding;
	// Current stun timer to count when to disable the stun
	//ftype		fStunTimer;
	bool		bIsStunned;

	//===============================================================================================//
	// Internal Routines
	//===============================================================================================//

	//  SetMovetypeConstants()
	// Sets constants for the movement check stuff
	void	InitMovetypeConstants ( void );
	void	SetMovetypeConstants ( void );
	//  UpdateTerrainSample()
	// Updates current block being stood on.
	// Also updates local "normal" and "tangent" of the terrain being installed.
	void	UpdateTerrainSample ( void );
	//  MoveUnstuck()
	// Checks for a terrain block where the player is standing.
	// If it's solid, the player is attempted to be moved out of it.
	void	MoveUnstuck ( void );

	//  StunWithAnimation()
	// Stuns the player with the animation. Will stun for entire duration of animation.
	// Returns true when animation is found.
	bool	StunWithAnimation ( const string& nStunName, const ftype nTimescale = 1.0f );

	//===============================================================================================//
	// Camera state
	//===============================================================================================//

	stateFunc_t	m_cameraUpdateType;
	void*	updateCameraState ( stateFunc_t );

	// Player Camera State Information
	Vector3d	vPlayerRotation;
	Vector3d	vCameraRotation;
	Vector3d	vCameraAnimOffsetRotation;
	Vector3d	vViewBob;
	Vector3d	vViewPunch;
	Vector3d	vViewPunchVelocity;
	float		fViewBobTimer;
	float		fViewBobSpeed;
	float		fViewBobAmount;
	float		fViewBobTarget;
	float		fViewAngle;
	float		fViewAngleOffset;
	float		fViewAnglePOffset;
	float		fViewAnglePTarget;
	float		fViewRollOffset;
	float		fViewRollTarget;

	// Setters of the view bob. Separates the movement and view bob just enough.
	void	camViewBobSetSpeed ( float );
	void	camViewBobSetTargetSize ( float );
	// Setters of view angle
	void	camViewAngleSetPOffset ( float = 0 );
	// Setters of camera roll angle
	void	camRollSetOffset ( float = 0 );

	// =Camera States=
	void*	cam_Default ( void );
	void*	cam_ThirdPerson ( void );
	void*	cam_Inventory ( void );
	void*	cam_Beauty ( void );
	void*	cam_BeautyMontage ( void );

	// =Variables=
	bool	bUseThirdPersonMode;
	bool	bUseBeautyCamMode;
	bool	bUseBeautyMontage;
	ftype	fCamAnimBlend;
	bool	bUseAnimCamMode;

	Vector3d vCameraRotationPrev;
	Vector3d vCameraPositionPrev;
	ftype	fCamSwapBlend;
	bool	bCamSwapFirstframe;

	//===============================================================================================//
	// Melee Input
	//===============================================================================================//

	Vector3d vPreviousAimRotation;
	Vector3d vNextAimRotation;

	Vector4d vAimingArc;

	//===============================================================================================//
	// Input 
	//===============================================================================================//

	Vector3d	vDirInput;
	Vector3d	vTurnInput;
	Real		fTurnSensitivity;
	CInputControl*	input;

	// Input function
	void GrabInput ( void );

private:
	bool	isLoaded;

	// == Basic Info ==
	CPlayerInventory*		pMyInventory;
	CPlayerHud*		pMyHud;
	CSkillTree*		m_skilltree;
	CSkillTree*		m_skilltree_race;
	CCrafting*		pMyCrafting;
	CQuestSystem*   pMyQuests;
	// == Player Info ==
	CPlayerStats*	pl_stats;
	CRacialStats*	pl_race_stats;
	CRacialStats*	base_race_stats;

	CInventory**	pl_bags;
	CRecipeLibrary*	pl_itemlib;


	// == Player HUD ==
	CPlayerHudStatus*	pl_hud;
	CPlayerInventoryGUI*		pl_inventoryGUI;
	CPlayerLogbook*		pl_logbookGUI;
	CCharacterScreen*	pl_characterGUI;
	CSkillTreeGUI*		pl_skilltreeGUI;
	CDialogueGUI*		pl_dialogueGUI;
	CInventoryGUI*		pl_chestGUI;
	CQuestSysRenderer* pl_questGUI;

	CPlayerMenu*		pl_gui_menu;

	Plhud_RadialMenu*	pl_radial_menu;

	// == Screen Effects ==
	CBloomShader*	pBloomTarget;
	ftype			fScreenBlurAmount;
	Ploverlay_Icing*pScreenIcing;

	// == Child objects ==
	SkillShield*		m_shield;

	// == Area Info ==
	void InitAreaInfo ( void );
	uint32_t		iCurrentRegion;
	uint32_t		iLastRegion;
	ftype			fRegionTime;
	void UpdateAreaInfo ( void );

	void GenerateSpawnpoint ( void );
};

#endif