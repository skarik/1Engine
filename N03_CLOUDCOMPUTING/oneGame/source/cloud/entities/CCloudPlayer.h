
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

class CParticleSystem;

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

	//	controlNormalShip
	// Provides logic and control for normal ship behavior
	void controlNormalShip ( void );

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

	// Camera state
	float shakeAmount;
	float fovWiden;

	// Motion control state
	bool drive_afterburner;
	bool drive_gravitydrive;
	float fuel_afterburner;
	float fuel_gravitydrive;

	//===============================================================================================//
	// Attached objects
	//===============================================================================================//

	CRigidBody*			rigidbody;
	CCapsuleCollider*	collider;

	CParticleSystem*	particlesystem;
};

#endif//_CLOUD_PLAYER_H_