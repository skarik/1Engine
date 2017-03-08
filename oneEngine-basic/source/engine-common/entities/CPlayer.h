
#ifndef _C_PLAYER_H_
#define _C_PLAYER_H_

// Includes
#include "CActor.h"
//#include "CCamera.h"
//#include "CLight.h"

// testosterone
//#include "CCapsuleCollider.h"
//#include "CCylinderCollider.h"
//#include "CRigidbody.h"

// estrogen
//#include "CInventory.h"
//#include "CPlayerHud.h"

class CAudioListener;
class CInputControl;
class CCharacterMotion;

class CCamera;
class CLight;
/*
// Enumeration
enum ePlayerObjectType
{
	// PL_TYPE_GENERAL refers to the base class CPlayer
	// Has very minimal functionality, only linked to the CCharacter.
	// Often can be used for spectator camera.
	PL_TYPE_GENERAL = 0,

	// PL_TYPE_REALCHAR refers to the class CMccPlayer.
	// CMccPlayer is the main player object for AFTER.
	PL_TYPE_REALCHAR = 1
};*/

//===============================================================================================//
// Basic Player Class
//===============================================================================================//
class CPlayer : public CActor
{
	ClassName( "CPlayer" );
public:
	//===============================================================================================//
	// CONSTRUCTION and DESTRUCTION
	//===============================================================================================//

	ENGCOM_API CPlayer ( void );
	ENGCOM_API ~CPlayer( void );

public: // ROUTINES
	//===============================================================================================//
	// READ-ONLY PROPERITES
	//===============================================================================================//

	// Retrieve the type of actor this is.
	virtual eActorObjectType ActorType ( void ) override { return ACTOR_TYPE_PLAYER_SIMPLE; }

	//===============================================================================================//
	// Step Events
	//===============================================================================================//

	// Game step

	void Update ( void );
	void LateUpdate ( void );
	//void PostUpdate ( void );

	// Physics Step

	//void FixedUpdate ( void );

	//===============================================================================================//
	// Sub-update functions
	//===============================================================================================//

	//	camDefault
	// Provides camera position and works on input
	void* camDefault ( void );
	//	mvtPhaseFlying
	// Provides player motion and works on additional input
	void* mvtPhaseFlying ( void );


public: // PUBLIC FIELDS
	//===============================================================================================//
	// Input Toggles
	//===============================================================================================//

	bool	bCanMove;		// Controls if the player can edit their own movement. (not yet implemented)
	bool	bHasInput;		// Controls if the player can have any input.
	bool	bOverrideInput; // When the player can not have input, when true, this does not reset inputs to zero.
	bool	bCanMouseMove;	// Controls if the player can turn with the mouse.

protected: // PROTECTED FIELDS and ROUTINES
	//===============================================================================================//
	// Attached objects
	//===============================================================================================//

	CCamera*	pCamera;	// Attached camera
	CAudioListener*	pListener;	// Attached listener

	//===============================================================================================//
	// Location state 
	//===============================================================================================//

	Vector3d	vPlayerRotation;
	Vector3d	vCameraRotation;

	//===============================================================================================//
	// Input 
	//===============================================================================================//

	Vector3d	vDirInput;
	Vector3d	vTurnInput;
	Real		fTurnSensitivity;
	CInputControl*	input;

	// Input function
	ENGCOM_API void GrabInput ( void );
};


// CPlayer is the base player class.
// At minimum, CPlayer provides basic camera control.
/*
// Class definition
class CPlayer : public CCharacter
{
	ClassName( "CPlayer" );
protected:
	friend CCharacterMotion;
public:
	// ===Constructor and Destructor===
	// Constructor
	CPlayer ( void );
	// Destructor
	~CPlayer ( void );
	
	// ===Step Functions===
	void Update ( void );
	void LateUpdate ( void );
	void PostUpdate ( void );

	void FixedUpdate ( void );

	// === Public Getter ===
	static CCharacter*	GetActivePlayer ( void ) { return m_activePlayer; }

	// === Public Setters ===
	virtual void SetCameraMode ( const NPC::eCameratypeEnumeration mode = NPC::CAMERA_MATCH ) {}
	virtual void SetMovementMode ( const NPC::eMovementEnumeration mode ) {}
	virtual NPC::eMovementEnumeration GetMovementMode ( void ) { return NPC::MOVEMENT_DEFAULT; }
public:
	// Overridable Player Type
	virtual ePlayerObjectType GetPlayerType ( void )
	{
		return PL_TYPE_GENERAL;
	}

	// ===Player Specific Functions==
	// 'Punches' or temporarily offset the view with the given offset.
	// This can be used for hurt effects, gun firing, and other single 'punch' camera effects.
	void	PunchView ( Vector3d const& );

	// ===Common Getters===
	// Get eye ray. Commonly used for raycasts.
	Ray		GetEyeRay ( void ) override
	{
		Ray newRay;
		newRay.pos = pCamera->transform.position;
		newRay.dir = pCamera->transform.Forward();
		return newRay;
	};
	// Get aiming direction. Used for melee weapons.
	Rotator GetAimRotator ( void ) override
	{
		return pCamera->transform.rotation;
	}
	// Get aim ray. Used for off-center weapon projectiles.
	Ray		GetAimRay ( const Vector3d & inPos ) override
	{
		Ray newRay;
		if ( rhLookAtResult.hit )
		{
			newRay.pos = inPos;
			newRay.dir = ( rhLookAtResult.hitPos - inPos ).normal();
		}
		else
		{
			newRay = GetEyeRay();
			newRay.pos = inPos;
		}
		return newRay;
	}
	// Returns type of character (0 animal, 1 monster, 2 sentinent)
	NPC::eAggroAIType GetCharacterType ( void ) override
	{
		return NPC::AGGRO_CHARACTER;
	}

	// ===Common Setters===
	// Sets the view angle. Normally, this is something like the FOV (field of vision) of a character.
	void SetViewAngle ( float = 100 );
	// Sets the offset for the view angle. This is the function you call when you want to temporarily change the view angle of a character
	// as when, for example, you want to zoom in with a Sniper Rifle.
	void SetViewAngleOffset ( float = 0 );
	// Sets the turn rate sensitivity. You don't need to do this when zooming in. That turn rate decrease is taken care of automatically.
	void SetTurnSensitivity ( float = 1 );

	// ===Transform Getters===
	XTransform	GetHoldTransform ( char i_handIndex=0 ) override;
	XTransform	GetBeltTransform ( char i_beltIndex ) override;

	// ===Misc Getters===
	// Get the last looked at position
	RaycastHit& GetLookHit ( void ) {
		return rhLookAtResult;
	}
	// Get the last looked at object
	CGameBehavior* GetCurrentLookedAt ( void ) {
		return pCurrentLookedAt;
	}

	// ===Input Toggles===
	// It is unneccesary to have setters/getters for these input toggles.
	bool	bCanMove;		// Controls if the player can edit their own movement. (not yet implemented)
	bool	bHasInput;		// Controls if the player can have any input.
	bool	bOverrideInput; // When the player can not have input, when true, this does not reset inputs to zero.
	bool	bCanMouseMove;	// Controls if the player can turn with the mouse.

protected:
	static CPlayer*	m_activePlayer;

	// ===Function Where Shit Happens==
	virtual void DoPlayerActions ( void );
	//CLight*		pMyLight;
	virtual void DoLookAtCommands ( void );
	CGameBehavior*	pCurrentLookedAt;
	float	fMaxUseDistance;

	//void	DoUseCommand ( void );
	//void	PickupItemsRadius ( void );

	RaycastHit	rhLookAtResult;

	// ===Audio===
	CAudioListener*	pListener;

	// ===Camera===
	CCamera* pCamera;
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

	float		fTurnSensitivity;

	// Setters of the view bob. Separates the movement and view bob just enough.
	void	camViewBobSetSpeed ( float );
	void	camViewBobSetTargetSize ( float );
	// Setters of view angle
	void	camViewAngleSetPOffset ( float = 0 );
	// Setters of camera roll angle
	void	camRollSetOffset ( float = 0 );
	
	// States
	void*	camDefault ( void );

	// Common Routines for use in states
	void	MvtCommonCrouching ( void ); //Common movement routine, such as crouching
	void	MvtSetPlayerHeight ( float = -1 );
	void	MvtSetPlayerHeightStick ( float = -1 );

	// States
	void*	mvtWalkingOnGround ( void );
	void*	mvtFalling ( void );
	void*	mvtSwimmingInWater ( void );

	void*	mvtCombatSlide ( void );
	void*	mvtCombatRoll ( void );

	void*	mvtPhaseFlying ( void );
	
public:
	// ===Movetype Queries===
	virtual bool	GetIsProne ( void ) { return false; }
	virtual bool	GetIsCrouching ( void ) { return false; }
	virtual bool	GetIsSprinting ( void ) { return false; }

	virtual CRigidbody*	GetRigidbody ( void ) { return NULL; }

protected:
	// ===Input===
	//bool	bCanUpdate;

	Vector3d	vDirInput;
	Vector3d	vTurnInput;

	CInputControl*	input;

	// Input function
	void GrabInput ( void );

public:
	CInputControl*	GetInputControl ( void ) { return input; };

};
*/
#endif