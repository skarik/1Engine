
#ifndef _PHYS_WRAPPER_CHARATER_RIGIDBODY_INFO_H_
#define _PHYS_WRAPPER_CHARATER_RIGIDBODY_INFO_H_

//#include "wrapper_common.h"
//#include "physVector4.h"
//#include "physQuaternion.h"
//#include "physMotion.h"
#include "physical/physics.h"
#include "physical/mode.h"

class physCharacterRigidBodyInfo
{
public:
	physCharacterRigidBodyInfo ()
	{
		m_mass = 100.0f;
		m_maxForce = 1000.0f;
		m_friction = 0.0f;
		m_maxSlope = HK_REAL_PI / 3.0f;
		m_unweldingHeightOffsetFactor = 0.5f;
		m_up.set( 0,1,0 );
		m_maxLinearVelocity = 20.0f;
		m_allowedPenetrationDepth = -0.1f;
		m_maxSpeedForSimplexSolver = 10.0f;
		m_collisionFilterInfo = 0;
		m_position.setZero4();
		//m_rotation.setIdentity(); // FUCKIN ERROR
		m_supportDistance = 0.1f;
		m_hardSupportDistance = 0.0f;
		m_shape = NULL;
	}

	Real_32		m_mass;
	Real_32		m_maxForce;
	Real_32		m_friction;
	Real_32		m_maxLinearVelocity;
	Real_32		m_unweldingHeightOffsetFactor;

	Real_32		m_allowedPenetrationDepth;
	Real_32		m_maxSpeedForSimplexSolver;

	physVector4	m_up;
	Real_32		m_maxSlope;
	Real_32		m_supportDistance;
	Real_32		m_hardSupportDistance;

	physShape*	m_shape;
	physCollisionFilter	m_collisionFilterInfo;

	physVector4 m_position;
	physQuaternion m_rotation;

#ifdef PHYSICS_USING_HAVOK
	FORCE_INLINE void saveToHk ( hkpCharacterRigidBodyCinfo& info )
	{
		info.m_mass	= m_mass;
		info.m_maxForce	= m_maxForce;
		info.m_friction	= m_friction;
		info.m_maxLinearVelocity = m_maxLinearVelocity;
		info.m_unweldingHeightOffsetFactor = m_unweldingHeightOffsetFactor;

		info.m_allowedPenetrationDepth	= m_allowedPenetrationDepth;
		info.m_maxSpeedForSimplexSolver	= m_maxSpeedForSimplexSolver;

		info.m_up	= m_up;
		info.m_maxSlope	= m_maxSlope;
		info.m_supportDistance	= m_supportDistance;
		info.m_hardSupportDistance	= m_hardSupportDistance;

		info.m_shape	= m_shape->getShape();
		info.m_collisionFilterInfo	= m_collisionFilterInfo;

		info.m_position	= m_position;
		info.m_rotation	= m_rotation;
	}
#endif
};

#endif//_PHYS_WRAPPER_CHARATER_RIGIDBODY_INFO_H_