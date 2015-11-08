
#ifndef _ENGINE_PHYSICS_SHAPES_PHYSSHAPE_H_
#define _ENGINE_PHYSICS_SHAPES_PHYSSHAPE_H_

#define _USING_HAVOK_
#ifdef _USING_HAVOK_
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
#else
	// Physics Interface
	class physRigidBodyInfo;
	class physRigidBody;
	class physShape;
	class physCollisionVolume;
#endif

#endif//_ENGINE_PHYSICS_SHAPES_PHYSSHAPE_H_