
#ifndef _ENGINE_PHYSICS_SHAPES_PHYSSHAPE_H_
#define _ENGINE_PHYSICS_SHAPES_PHYSSHAPE_H_

#include "physical/mode.h"
#include "core/types/types.h"

class CPhysics;

#ifdef PHYSICS_USING_HAVOK
	// Havok specific
	class hkpRigidBodyCinfo;
	class hkpRigidBody;
	class hkpShape;
	class hkpPhantom;

	// Physics Typedefs
	//typedef hkpRigidBodyCinfo	physRigidBodyInfo;
	class physRigidBodyInfo;
	//typedef hkpRigidBody		physRigidBody;
	//typedef hkpShape			physShape;		// Collision Shape
	typedef hkpPhantom			physCollisionVolume;

	// Physics Interface
	class physRigidBody;

	//
	// Shape container class
	//
	// Required for additional buffer between Havok and Engine
	class physShape
	{
	public:
		PHYS_API physShape () : m_shape(NULL) {;}
		FORCE_INLINE PHYS_API virtual ~physShape ();
		// Explicit shape get
		PHYS_API hkpShape*	getShape ( void );
		PHYS_API const hkpShape*	getShape ( void ) const;
		// Non-explicit shape get
		PHYS_API operator const hkpShape* ();
	protected:
		hkpShape*	m_shape;
	private:
		friend CPhysics;
		// Internal hack push
		physShape (hkpShape* shape) : m_shape(shape) {;}
	};
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