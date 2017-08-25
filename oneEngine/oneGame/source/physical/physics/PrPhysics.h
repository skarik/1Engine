#ifndef PHYSICS_PR_PHYSICS_H_
#define PHYSICS_PR_PHYSICS_H_

#include "core/types/types.h"
#include "core/containers/arsingleton.h"
#include "physical/mode.h"
#include "physical/types/layers.h"

#include "bullet/btBulletDynamicsCommon.h" // Todo: minimize this header

#include <vector>
#include <mutex>

class PrWorld;

//	class PrPhysics : Physics manager class. Works as a singleton.
// Provides an interface for PrWorld's to hook into.
class PrPhysics
{
	ARSINGLETON_H_STORAGE(PrPhysics, PHYS_API);
	ARSINGLETON_H_ACCESS (PrPhysics);

private:
	// Private constructor & destructor - as a singleton, does not allow for creation.
	PHYS_API explicit	PrPhysics	( void );
	PHYS_API			~PrPhysics	( void );

	// Loads up default filters.
	void				Initialize_CollisionMasks ( void );
	// Loads up physics engine objects.
	void				Initialize_Engine ( void );

	// Frees up engine objects.
	void				Free_Engine ( void );

public:
	// Initialize() : Initializes the physics engine system and needed values
	PHYS_API void		Initialize	( void );

	//	AddWorld (world) : adds a world to the update list.
	PHYS_API void		AddWorld	( PrWorld* world_to_add_update );
	//	RemoveWorld (world) : removes world from the update list
	PHYS_API void		RemoveWorld	( PrWorld* world_to_remove_update );
	//	CurrentWorld() : returns the currently active world
	// Returns NULL if no world is active.
	PHYS_API PrWorld*	CurrentWorld ( void );

	//	UpdateSimulationTarget (deltaTime) : prepares simulation for given amount of time.
			 void		UpdateSimulationTarget ( Real elasped_time );
	//	SimulationAtTarget() : Simulation has reached end of a sim?
			 bool		SimulationAtTarget ( void );
	//	SimulationAtSubstep() : Simulation has reached end of sub-sim?
			 bool		SimulationAtSubstep ( void );
	//	Simulate() : Simulate physics.
			 void		Simulate ( Real timestep );
	//	SimulationOvershoot() :
			 Real		SimulationOvershoot ( Real timestep );
	//
	PHYS_API physical::prLayerMask	GetLayerMask ( physical::prLayer group )
		{ return m_collisionMasks[group]; }

private:
	std::vector<PrWorld*>	m_worlds;
	std::mutex				m_worlds_lock;

	int						m_activeWorldIndex;
	physical::prLayerMask*	m_collisionMasks;

private:
	btCollisionDispatcher*	m_dispatcher;
	btConstraintSolver*		m_solver;
	btDefaultCollisionConfiguration* m_collisionConfiguration;

	Real64		m_simulationTime;
	Real64		m_simulationTarget;

public:
	btCollisionDispatcher*	GetDispatcher ( void )
		{ return m_dispatcher; }
	btConstraintSolver*		GetSolver ( void )
		{ return m_solver; }
	btDefaultCollisionConfiguration*	GetCollisionConfiguration ( void )
		{ return m_collisionConfiguration; }
	
};

#endif//PHYSICS_PR_PHYSICS_H_