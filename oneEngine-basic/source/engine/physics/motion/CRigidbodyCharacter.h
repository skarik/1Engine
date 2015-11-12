
#ifndef _C_RIGIDBODY_CHARACTER_H_
#define _C_RIGIDBODY_CHARACTER_H_

#include "CRigidbody.h"

class physCharacter;

class CRigidBodyCharacter : public CRigidBody
{

public:
	// Regular constructor
	ENGINE_API explicit CRigidBodyCharacter ( CCollider* pTargetCollider, CGameObject * pOwnerGameObject, float fMass=1.0f );
	// Destructor
	ENGINE_API ~CRigidBodyCharacter ( void );

	// Physics step
	ENGINE_API void FixedUpdate ( void ) override;
	ENGINE_API void RigidbodyUpdate ( void ); // Rigidbody update is a specific type of update

	// Engine step
	ENGINE_API void Update ( void );

	// Get and set
	ENGINE_API virtual void		SetVelocity ( Vector3d );
	ENGINE_API virtual Vector3d	GetVelocity ( void );

	// Accessors
	ENGINE_API physRigidBody* GetBody ( void ) override;

	class mContactModifier;
protected:
	//hkpCharacterRigidBody* mCharController;
	physCharacter*		mCharController;
	mContactModifier*	mContactListener;
};

#endif//_C_RIGIDBODY_CHARACTER_H_