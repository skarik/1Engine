
#ifndef _C_CHARACTER_MOTION_H_
#define _C_CHARACTER_MOTION_H_

#include "core/types/types.h"
#include "core/math/Vector3d.h"
#include "after/states/NPC_CombatState.h"
#include "after/types/character/MovementEnum.h"

#include <list>

class CRacialStats;
class CCapsuleCollider;
class CRigidBody;
class CCharacter;
class CInputControl;
class CVehicleActor;
class CCharacterModel;
class CAfterCharacterAnimator;

// CCharacterMotion handles all character motion, including rigidbody movement.
// This motion can be overridden (with m_disableMotion), but the heavy lifting is done with this class.
// 
// To disable input-controlled motion, set m_disableMotion to true.
// This class can be polled whether to allow miscellenous item usage through Get_HandsBusy and Get_BodyBusy.
// Get_BodyBusy represents if the motion is busy but we allow minor actions.
// Get_HandsBusy represents if the motion cannot permit any other actions.

class CCharacterMotion
{
public:
	typedef void* (CCharacterMotion::*motion_t)(void);

	//=========================================//
	// Motion Settings
	//=========================================//

	// Is motion disabled?
	bool	m_disableMotion;

	// Stats used for motion settings
	CCharacter*		m_character;
	CRacialStats*	m_stats;
	CCharacterModel*m_model;
	CAfterCharacterAnimator*	m_animator;

	// Can the character slide down walls
	bool	m_canWallSlide;
	// Can the character run across walls
	bool	m_canWallRun;
	// Can the character run up walls
	bool	m_canWallClimb;
	// Can the character grab ledges and climb up them
	bool	m_canLedgeClimb;
	// Can the character perform a slide after sprinting
	bool	m_canCombatSlide;
	// Can the character sprint
	bool	m_canSprint;
	// Can the character jump
	bool	m_canJump;

	// Used to forward input
	CInputControl*			m_input;

	CCharacterMotion ( void );
	~CCharacterMotion( void );

public:
	//=========================================//
	// Internal Motion State
	//=========================================//

	motion_t				m_moveType;
	motion_t				m_moveTypePrevious;
	motion_t				m_moveTypeQueued;	// Move type to take on next frame
	motion_t				m_moveTypeNext;		// Move type to take on after current action (used when stunned)

	CCapsuleCollider*		m_collider;
	CRigidBody*				m_rigidbody;

	// Used to forward/edit input (and feed it output)
	Vector3d	vTurnInput;
	Vector3d	vCharRotation;
	Vector3d	vHeadRotation;

	// Current normal and downward tangent of terrain
	// Need to be input from external source
	Vector3d	vTerrainTangent;		
	ushort		iTerrainCurrentBlock;

	// Collider state
	bool	bIsCrouching;
	bool	bStartedCrouching;
	bool	bWantsCrouch;
	bool	bCanUncrouch;

	bool	bIsProne;
	bool	bStartedProne;
	bool	bWantsProne;
	bool	bCanUnprone;

	bool	bIsSprinting;
	bool	bCanSprint;

	Real	fPlayerHeight;

	// Time counter
	Real	fSlideCounter;
	Real	fWallrunCooldown;

	// Motion state
	Vector3d vMoveVelocity;
	Vector3d vMoveLastGroundNormal;

	bool	bOnGround;

	// =Constants=
	ftype	fWallSlideTime;

	ftype	fWallClimbStepTime;

	ftype	fSlideSpeedConstant;
	ftype	fRigidbodyNormalFriction;
	ftype	fRigidbodySlideFriction;
	ftype	fRigidbodyNormalRestitution;
	ftype	fRigidbodyMovementRestitution;

	ftype	fLedgeGrabMaxHeight;
	ftype	fLedgeGrabMinHeight;
	ftype	fLedgeGrabMaxDistance;	// Max distance for ledge grabbing from ledge
	ftype	fLedgeTargetHangHeight; // Vertical offset from head to edge

	ftype	fVaultTargetMaxHeight; // Max height a character can vault up
	ftype	fVaultTargetMaxDistance; // Max distance to start vaulting

	// Current vehicle the player is in
	CVehicleActor*	currentVehicle;
	// Current falling velocity
	Vector3d	vFallingVelocity;
	// If movement should be calculated unobstructed (ie MoveUnstuck)
	bool		bMoveUnobstructed;
	// If autojump should be used regardless of speed
	bool		bOverrideAutojump;
	// Current hanging hand position, and current hanging surface wall normal
	Vector3d	vHangingPosition;
	Vector3d	vHangingWall;
	ftype		fHangingHeight;	// Vertical offset from head to edge
	Vector3d	vTargetHangingPosition;
	ftype		fHangingTimer;
	// Current vaulting mode
	ftype		vVaultHeight;
	bool		bVaultObject;
	ftype		vVaultTime;
	// Current climbing base position, and current climbing surface wall normal
	Vector3d	vClimbingBase;
	Vector3d	vClimbingWall;
	// Wall running and sliding counters
	ftype		fWallSlideTurnThreshold;
	ftype		fWallSlideTurnSum;
	bool		bWallSlideTurned;
	ftype		fWallClimbCounter;
	int			iWallRunStepCount;
	// On ground turning and moving thresholds
	ftype		fOnGroundTurnThreshold;
	ftype		fOnGroundMoveThreshold;
	int			iOnGroundState;
	// Current stun timer to count when to disable the stun
	std::list<Vector4d>	vLinearMotions;
	//
	bool	bWallRunReady;
	// 
	//bool	bAllowingSprint;

	// Stunning timer
	ftype		fStunTimer;

private:
	bool		m_charIsPlayer;

public:
	//
	void InitMovement ( void );
	void FreeMovement ( void );
	// Set
	void SetMovetypeVariables ( void );
	void SetMovetypeConstants ( void );

	// Frame (General) Update
	void Update ( void );
	// Fixed step (Physics) Update
	void FixedUpdate ( Vector3d* io_turnInput, Vector3d* io_charRotation, Vector3d* io_headRotation );

	// Stops motion and resets the falling damage counter
	void StopMotion ( void );

	// Minor level busy
	bool Get_BodyBusy ( void );
	// Full level busy
	bool Get_HandsBusy ( void );

	
	//void UpdateTerrainSample ( void );
	void MoveUnstuck ( void );

	// Set the movement mode based on Enum
	void SetMovementModeQueued ( const NPC::eMovementEnumeration mode );
	void SetMovementModeImmediate ( const NPC::eMovementEnumeration mode );
	void SetMovementModeNext ( const NPC::eMovementEnumeration mode );
	// Return the movement mode based on the Enum
	NPC::eMovementEnumeration GetMovementMode ( void ) const;

private:
	FORCE_INLINE motion_t getMotionState ( const NPC::eMovementEnumeration mode );
	FORCE_INLINE void* updateMoveType ( motion_t n_newMoveType );

	// Common movetype routines (called across multiple states)
	void MvtCommonCrouching ( void );
	void MvtSetPlayerHeight ( float fnPlayerHeight );
	void MvtSetPlayerHeightStick ( float fnPlayerHeight );

	// Get effective hull size
	Real MvtGetEffectiveHullRadius ( void );

	// Camera sets
	FORCE_INLINE void CamSetFOVTarget ( const float nOffsetTarget );
	FORCE_INLINE void CamSetRollTarget ( const float nOffsetTarget );
	FORCE_INLINE void CamSetShakeTarget ( const float nSizeTarget );
	FORCE_INLINE void CamSetAnimationStick ( const bool nGlueCamera );
	FORCE_INLINE void CamPunchView ( const Vector3d& nPunchAmount );

	// Sets bOnGround on returns that value.
	// Also updates the ground normal.
	bool OnGround ( Real& outGroundDistance );


	//  UpdateFalling( velocity )
	// Saves the current velocity for falling damage.
	void	UpdateFalling ( const Vector3d & velocity );
	//  EndFalling ()
	// Performs code for ending falling. Fall damage is calculated here, but not executed.
	// It returns the fall damage factor calculated, and resets the fall velocity saved.
	ftype	EndFalling ( void );


	//  CheckLedgeGrabbing()
	// Checks ahead for a grabbable ledge.
	// If the player can grab the ledge, returns true, and vHangingWall and vHangingPosition will have valid values.
	// Otherwise, returns false
	bool	CheckLedgeGrabbing ( void );
	//  CheckWallSliding()
	// Checks ahead for a slideable wall.
	// If the player can slide on the wall, returns true, and vHandlingWall will have a valid value;
	// Otherwise, returns false
	bool	CheckWallSliding ( void );
	//	CheckWallRunClimbing()
	// Check ahead for a place for the player to wall run climb and sets vHangingWall to the normal of the wall.
	// If the player has a foothold, returns true. Otherwise, returns false
	bool	CheckWallRunClimbing ( void );
	//	CheckWallRunLateral()
	// Cehck ahead to either side for a wall the player can run across, and sets vHangingWall to the normal of the wall.
	// If the player has a foothold, returns true. Otherwise, returns false
	bool	CheckWallRunLateral ( void );


	//  CheckVaultableObject()
	// Checks ahead for a vaultable object.
	// If the object is small enough to be vaulted up/over, returns true.
	bool	CheckVaultableObject ( void );


private:
	//=========================================//
	// Move States
	//=========================================//

	void*	mvt_OnGround ( void );
	void*	mvt_Falling ( void );
	void*	mvt_Swimming ( void );

	// Sliding and rolling
	void*	mvt_CombatSlide ( void );
	void*	mvt_CombatRoll ( void );

	// Terrain sliding
	void*	mvt_TerraSlide ( void );

	// Ledge grabbing
	void*	mvt_ClimbStart ( void );
	void*	mvt_ClimbHang ( void );
	void*	mvt_ClimbUp ( void );

	// Climbing trees/ladders
	void*	mvt_Climbing ( void );

	// Vaulting objects
	void*	mvt_VaultStart ( void );
	void*	mvt_VaultHop ( void );
	void*	mvt_VaultClimb ( void );

	// In vehicle
	void*	mvt_InVehicle ( void );

	// Death
	void*	mvt_Saviorize ( void );
	void*	mvt_Dead ( void );
	void*	mvt_DeadRecover ( void );

	// Stunned
	void*	mvt_Stunned ( void );

	// Fallen
	void*	mvt_FellOnBackStart ( void );
	void*	mvt_FellOnBack ( void );
	void*	mvt_FellOnBackUp ( void );

	// Mobility
	void*	mvt_WallClimb ( void ); // run up wall
	void*	mvt_WallSlide ( void ); // starts when hit wall in mvt_Falling. uses roll + slide timer
	void*	mvt_WallRun ( void );	// run across wall

	// Skills
	void*	mvt_Minidash ( void ); // minidash

	// Combat
	void*	mvt_AtkGround ( void );
	void*	mvt_AtkAir ( void );
	void*	mvt_AtkSlide ( void );

};

#endif//_C_CHARACTER_MOTION_H_