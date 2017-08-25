#ifndef PHYSICAL_PHYSICS_MOTION_I_PR_RIGID_BODY_H_
#define PHYSICAL_PHYSICS_MOTION_I_PR_RIGID_BODY_H_

#include "core/types/types.h"
#include "bullet/BulletDynamics/Dynamics/btRigidBody.h"

class IPrRigidBody
{
protected:
	PHYS_API btRigidBody*	NewRigidBody( const btRigidBody::btRigidBodyConstructionInfo& constructionInfo );

	PHYS_API void			DeleteRigidBody( btRigidBody** rigidBodyPointer );
};

#endif//PHYSICAL_PHYSICS_MOTION_I_PR_RIGID_BODY_H_