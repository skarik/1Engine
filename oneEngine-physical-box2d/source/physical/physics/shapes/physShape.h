
#ifndef _ENGINE_PHYSICS_SHAPES_PHYSSHAPE_H_
#define _ENGINE_PHYSICS_SHAPES_PHYSSHAPE_H_

#include "physical/mode.h"

#ifdef PHYSICS_USING_HAVOK
	// Havok specific
	class hkpRigidBodyCinfo;
	class hkpRigidBody;
	class hkpShape;
	class hkpPhantom;

	// Physics Typedefs
	typedef hkpRigidBodyCinfo	physRigidBodyInfo;
	//typedef hkpRigidBody		physRigidBody;
	typedef hkpShape			physShape;		// Collision Shape
	typedef hkpPhantom			physCollisionVolume;

	// Physics Interface
	class physRigidBody;
#elif defined(PHYSICS_USING_BOX2D)
	class b2Shape;

	class physRigidBodyInfo;
	class physRigidBody;
	//typedef b2Shape physShape;
	class physCollisionVolume;

	class physShape
	{
	public:
		b2Shape*	getShape ( void )
		{
			return m_shape;
		}
	protected:
		b2Shape*	m_shape;
	};
#else
	// Physics Interface
	class physRigidBodyInfo;
	class physRigidBody;
	class physShape;
	class physCollisionVolume;
#endif

#endif//_ENGINE_PHYSICS_SHAPES_PHYSSHAPE_H_