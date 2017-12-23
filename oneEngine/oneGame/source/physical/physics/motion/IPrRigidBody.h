#ifndef PHYSICAL_PHYSICS_MOTION_I_PR_RIGID_BODY_H_
#define PHYSICAL_PHYSICS_MOTION_I_PR_RIGID_BODY_H_

#include "core/types/types.h"
#include "bullet/BulletDynamics/Dynamics/btRigidBody.h"

class IPrRigidBody
{
protected:
	PHYS_API btRigidBody*	NewRigidBody( const btRigidBody::btRigidBodyConstructionInfo& constructionInfo );

	PHYS_API void			DeleteRigidBody( btRigidBody** rigidBodyPointer );

public:
	// ApiBody() : Returns the underlying physics engine Rigidbody implementation.
	//	Touchy the physics!
	PHYS_API btRigidBody*	ApiBody ( void );

protected:

	// Bullet rigidbody component
	btRigidBody*	body;
};

#endif//PHYSICAL_PHYSICS_MOTION_I_PR_RIGID_BODY_H_