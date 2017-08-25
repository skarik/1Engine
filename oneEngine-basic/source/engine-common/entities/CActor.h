
#ifndef _C_ACTOR_H_
#define _C_ACTOR_H_

// Includes
#include "core/math/Ray.h"
#include "engine/behavior/CGameObject.h"
//#include "engine-common/entities/CActorAnimation.h"
#include "engine-common/types/Damage.h"
//#include "engine-common/types/ActorAnimation.h"

class CInventory;
class CPlayerInventory;
class CItemBase;

class CharacterStats;
class CRacialStats;

// Enumeration
enum eActorObjectType
{
	// ACTOR_TYPE_GENERAL refers to the base class CActor
	ACTOR_TYPE_GENERAL	= 0,

	// (AFTER specific)
	// ACTOR_TYPE_PLAYER refers to a player class.
	// CAfterPlayer is the main player object for AFTER.
	ACTOR_TYPE_PLAYER	= 1,
	// ACTOR_TYPE_PLAYER_SIMPLE refers to a simple player class.
	// The only class that they can be guaranteed to cast into is a CActor.
	ACTOR_TYPE_PLAYER_SIMPLE	= 11,

	// ACTOR_TYPE_NPC refers to an NPC.
	ACTOR_TYPE_NPC		= 2,
	// (AFTER specific)
	// ACTOR_TYPE_CHARACTER refers to any child of the CCharacter class.
	// Guaranteed that it can be cast into a CCharacter. From there, can cast upwards based on CharacterType().
	ACTOR_TYPE_CHARACTER	= 22,

	// ACTOR_TYPE_TREE refers to a tree actor.
	// Trees are usually deep ingrained in a foliage system.
	ACTOR_TYPE_TREE		= 3,

	// ACTOR_TYPE_PROP refers to a world prop.
	// Props are interactable objects.
	ACTOR_TYPE_PROP		= 4
};

// Class Definition
class CActor : public CGameObject
{
	ClassName( "CActor" );
	BaseClass( "CActor" );
public:
	// Constructor
	CActor ( void ) : CGameObject ()
	{
		layer = physical::layer::Actor;

		bInteractable = false;
		bAlive = false;
	};
	// Destructor
	~CActor ( void ) {};
	
	// Step functions
	void Update ( void ) {};
	void LateUpdate ( void ) {};

	void FixedUpdate ( void ) {};

public:
	/* ===Read Only Properties=== */

	// Retrieve the type of actor this is.
	virtual eActorObjectType ActorType ( void ) { return ACTOR_TYPE_GENERAL; }

	/* ===Callbacks=== */
	//void	OnHit		( void );
	virtual void	OnDamaged	( Damage const&, DamageFeedback* =NULL ) {}
	virtual void	OnDeath		( Damage const& ) {}
	virtual void	OnDealDamage( Damage&, CActor* receivingCharacter ) {}

	virtual void	OnInteract	( CActor* interactingActor ) {}
	virtual void	OnInteractLookAt	( CActor* interactingActor ) {}
	virtual void	OnInteractLookAway	( CActor* interactingActor ) {}

	virtual void	OnEquip ( CItemBase* pItem ) {}
	virtual void	OnUnequip ( CItemBase* pItem ) {}

	/* ===Common Getters=== */
	/* Transform Getters */
	// Get eye ray. Commonly used for raycasts.
	virtual	Ray		GetEyeRay ( void )
	{
		Ray newRay;
		newRay.pos = transform.world.position;
		newRay.dir = transform.world.rotation * Vector3d::forward;
		return newRay;
	};
	// Get aim ray. Used for off-center weapon projectiles.
	virtual Ray		GetAimRay ( const Vector3d & )
	{
		return GetEyeRay();
	}
	// Get aiming arc. Used for melee weapons.
	// Returns a Vector4d indicating the following aiming properties:
	// X is the width of the arc in degrees.
	// Y is the vertical rotation offset. (non-zero means not aiming down center of screen).
	// Z is the depth rotation.
	// W is ~0 for zero depth, ~1 for deep depth
	virtual Vector4d GetAimingArc ( void )
	{
		return Vector4d( 37,0,3,0 );
	}
	// Get aiming direction. Used for melee weapons.
	virtual Rotator GetAimRotator ( void )
	{
		return transform.world.rotation;
	}

	/* Component Getters */
	// Get actor inventory
	virtual CInventory*		GetInventory ( void )
	{
		return NULL;
	}

	// Is this an actor or a character?
	virtual bool IsCharacter ( void )
	{
		return false;
	}

	// Returns a pointer to the stats
	virtual CharacterStats* GetCharStats ( void ) { return NULL; }
	// Returns a pointer to misc stats
	virtual CRacialStats*	GetRacialStats ( void ) { return NULL; };

	// ===Transform Getters===
	virtual XTransform	GetHoldTransform ( char i_handIndex=0 ) { return XTransform(); };
	virtual XTransform	GetBeltTransform ( char i_beltIndex ) { return XTransform(); };

	// ===Base Animation Interface==
	virtual void	PlayAnimation ( const string& sActionName ) { ; }

public:
	// Returns if the actor is able to be interacted with.
	bool	Interactable ( void )
	{
		return bInteractable;
	}
	// Returns if the actor is alive.
	bool	IsAlive ( void )
	{
		return bAlive;
	}

protected:
	// Can this actor be interacted with? Interactable actors will send OnInteract when the use key is pressed on them.
	//  Depending on the calling actor that is attempting interaction, the functions OnInteractLookAt and OnInteractLookAway will be called as well.
	//  Those two functions could be used for putting outlines around interactable objects that are looked at.
	bool	bInteractable;

	// Is this actor alive? Alive actors will call OnDeath when their health drops below zero.
	bool	bAlive;
private:
	;
};

// Function for state functions
typedef void* (CActor::*stateFunc_t)(void);


#endif