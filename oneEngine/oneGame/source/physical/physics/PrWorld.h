#ifndef PHYSICS_PR_WORLD_H_
#define PHYSICS_PR_WORLD_H_

#include "core/types/types.h"
#include "core/math/Vector3d.h"

#include "bullet/BulletCollision/BroadphaseCollision/btBroadphaseInterface.h"
#include "bullet/BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h"

#include <vector>

class IPrVolume;

struct prWorldCreationParams
{
	Vector3f	m_gravity;
};

//	class PrWorld : represents the simulation area for a physics state.
// Contains needed broadphase, narrowphase, and related callbacks. Manages their coordinate system.
// Also stores active volumes and other interfaces, ie IPrWaterVolumes and PrWaterQuery.
// The underlying physics representation stores the simulation objects for the world, ie RigidBodies and constraints.
class PrWorld
{
public:
	PHYS_API explicit	PrWorld ( const prWorldCreationParams& params );
						~PrWorld ( void );

	//	AddVolume ( volume ) : Adds a volume to the internal listing
	PHYS_API void		AddVolume( IPrVolume* volume_to_add );
	//	RemoveVolume ( volume ) : Removes a volume from the internal listing
	PHYS_API void		RemoveVolume( IPrVolume* volume_to_remove );

private:
	bool	m_enabled; // Is this world simulated?

	// Bullet objects:
	btBroadphaseInterface*		m_broadphase;
	btDiscreteDynamicsWorld*	m_dynamicsWorld;

	// Volume listing:
	std::vector<IPrVolume*>		m_volumes;

public:
	btBroadphaseInterface*		ApiBroadphase ( void )
		{ return m_broadphase; }
	btDiscreteDynamicsWorld*	ApiWorld ( void )
		{ return m_dynamicsWorld; }
};

#endif//PHYSICS_PR_WORLD_H_
