
#ifndef _CLOUD_PLAYER_H_
#define _CLOUD_PLAYER_H_

// Includes
#include "engine-common/entities/CActor.h"
#include "engine-common/entities/CPlayer.h"

class CAudioListener;
class CInputControl;
class CCharacterMotion;

class CCamera;
class CLight;

class CRigidBody;
class CCapsuleCollider;


//===============================================================================================//
// Basic Player Class
//===============================================================================================//
class CCloudPlayer : public CPlayer
{
	ClassName( "CCloudPlayer" );
public:
	//===============================================================================================//
	// CONSTRUCTION and DESTRUCTION
	//===============================================================================================//

	GAME_API CCloudPlayer ( void );
	GAME_API ~CCloudPlayer( void );

public: // ROUTINES
	//===============================================================================================//
	// READ-ONLY PROPERITES
	//===============================================================================================//

		// Retrieve the type of actor this is.
	virtual eActorObjectType ActorType ( void ) override { return ACTOR_TYPE_PLAYER; }

	//===============================================================================================//
	// Step Events
	//===============================================================================================//

	// Game step

	void Update ( void );
	void LateUpdate ( void );
	//void PostUpdate ( void );

	// Physics Step

	void FixedUpdate ( void );

	//===============================================================================================//
	// Sub-update functions
	//===============================================================================================//

	//	camDefault
	// Provides camera position and works on input
	//void* camDefault ( void );
	//	mvtPhaseFlying
	// Provides player motion and works on additional input
	//void* mvtPhaseFlying ( void );

	//	camShipFirstPerson
	// Provides camera position
	void* camShipFirstPerson ( void );

	//	mvtNormalShip
	// Provides sexy player motion.
	void* mvtNormalShip ( void );


protected: // PROTECTED FIELDS and ROUTINES
	//===============================================================================================//
	// Location state 
	//===============================================================================================//

	Rotator playerRotation;

	//===============================================================================================//
	// Attached objects
	//===============================================================================================//

	CRigidBody*			rigidbody;
	CCapsuleCollider*	collider;

};

#endif//_CLOUD_PLAYER_H_