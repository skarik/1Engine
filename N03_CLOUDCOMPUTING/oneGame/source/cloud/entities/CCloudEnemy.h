#ifndef _C_CLOUD_ENEMY_H_
#define _C_CLOUD_ENEMY_H_

//Includes
#include "engine-common/entities/CActor.h"

class CRigidBody;
class CCapsuleCollider;

class CModel;
class CInstancedModel;
class CInstancedMesh;

struct VAxes {
	bool crouch;
	bool jump;
	bool menuToggle;
	bool sprint;
};

class CCloudEnemy : public CActor
{
	ClassName("CCloudEnemy");
public:
	//===============================================================================================//
	// CONSTRUCTION and DESTRUCTION
	//===============================================================================================//

	GAME_API CCloudEnemy(void);
	GAME_API ~CCloudEnemy(void);

public: // ROUTINES
	//===============================================================================================//
	// READ-ONLY PROPERITES
	//===============================================================================================//

	// Retrieve the type of actor this is.
	virtual eActorObjectType ActorType(void) override { return ACTOR_TYPE_NPC; }

	//===============================================================================================//
	// Step Events
	//===============================================================================================//

	// Game step

	void Update(void);
	void LateUpdate(void);
	//void PostUpdate ( void );

	// Physics Step

	void FixedUpdate(void);

	//===============================================================================================//
	// Callback functions
	//===============================================================================================//

	void OnDamaged ( Damage const&, DamageFeedback* =NULL ) override;
	void OnDeath ( Damage const& ) override;

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
	//void* camShipFirstPerson(void);

	//	mvtNormalShip
	// Provides sexy player motion.
	void* mvtNormalShip(void);

	//Setters to create a virtual controller for the AI to use
	void SetTurnInput(Vector3d turn);
	void SetDirInput(Vector3d dir);
	void SetVAxes(int flags);
	void SetRotation(Rotator rot);

	//Getters to help the AI decide what to do
	Vector3d GetVelocity();

protected: // PROTECTED FIELDS and ROUTINES
	//===============================================================================================//
	// Location state 
	//===============================================================================================//

	Rotator enemyRotation;

	//Virtual Controller stuff for AI to use
	//Warning: Feels pretty shady
	Vector3d vTurnInput;
	Vector3d vDirInput;
	VAxes input;

	//===============================================================================================//
	// Game state 
	//===============================================================================================//
	float health;

	//===============================================================================================//
	// Attached objects
	//===============================================================================================//

	CRigidBody*			rigidbody;
	CCapsuleCollider*	collider;

	uint32_t			manifest_id;

	//CInstancedModel*		model;
	static CModel*				model;
	static CInstancedMesh*		mesh;

	static std::vector<CCloudEnemy*>	manifest;
};

#endif _C_CLOUD_ENEMY_H_