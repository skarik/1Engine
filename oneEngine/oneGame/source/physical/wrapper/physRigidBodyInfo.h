
#ifndef _PHYS_WRAPPER_RIGIDBODY_INFO_H_
#define _PHYS_WRAPPER_RIGIDBODY_INFO_H_

#include "wrapper_common.h"
#include "physVector4.h"
#include "physQuaternion.h"
#include "physMotion.h"

class physRigidBodyInfo
{
public:
	physRigidBodyInfo ()
	{
		m_mass = 1.0f;
		m_friction = 0.2f;
		m_restitution = 0.0f;
		m_shape = NULL;

		m_centerOfMass = physVector4(0,0);
	}

	Real32		m_mass;
	Real32		m_friction;
	Real32		m_restitution;
	Real32		m_linearDamping;
	Real32		m_angularDamping;

	physShape*	m_shape;
	b2Filter	m_collisionFilterInfo;
	physVector4	m_centerOfMass;
	physMotionType	m_motionType;

	physVector4 m_position;
	physQuaternion m_rotation;
};

#endif//_PHYS_WRAPPER_RIGIDBODY_INFO_H_