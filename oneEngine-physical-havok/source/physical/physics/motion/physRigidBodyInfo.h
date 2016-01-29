
#ifndef _PHYS_WRAPPER_RIGIDBODY_INFO_H_
#define _PHYS_WRAPPER_RIGIDBODY_INFO_H_

//#include "wrapper_common.h"
//#include "physVector4.h"
//#include "physQuaternion.h"
//#include "physMotion.h"
#include "physical/physics.h"
#include "physical/mode.h"

class physRigidBodyInfo
{
public:
	physRigidBodyInfo ()
	{
		m_mass = 1.0f;
		m_friction = 0.2f;
		m_restitution = 0.0f;
		m_shape = NULL;

		m_centerOfMass = physVector4(0,0,0);
	}

	Real_32		m_mass;
	Real_32		m_friction;
	Real_32		m_restitution;
	Real_32		m_linearDamping;
	Real_32		m_angularDamping;

	physShape*	m_shape;
	physCollisionFilter	m_collisionFilterInfo;
	physVector4	m_centerOfMass;
	physMotionType	m_motionType;

	physVector4 m_position;
	physQuaternion m_rotation;

#ifdef PHYSICS_USING_HAVOK
	FORCE_INLINE void saveToHk ( hkpRigidBodyCinfo& info )
	{
		info.m_mass	= m_mass;
		info.m_friction	= m_friction;
		info.m_restitution	= m_restitution;
		info.m_linearDamping	= m_linearDamping;
		info.m_angularDamping	= m_angularDamping;

		info.m_shape	= m_shape->getShape();
		info.m_collisionFilterInfo	= m_collisionFilterInfo;
		info.m_centerOfMass	= m_centerOfMass;
		info.m_motionType	= m_motionType;

		info.m_position	= m_position;
		info.m_rotation	= m_rotation;
	}
#endif
};

#endif//_PHYS_WRAPPER_RIGIDBODY_INFO_H_