#include "physical/physics/PrPhysics.h"
#include "physical/physics/PrWorld.h"

#include "bullet/btBulletDynamicsCommon.h"

void PrPhysics::Initialize_Engine ( void )
{
	// collision configuration contains default setup for memory, collision setup
	m_collisionConfiguration = new btDefaultCollisionConfiguration();

	// use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
	m_dispatcher = new btCollisionDispatcher(m_collisionConfiguration);

	// the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
	m_solver = new btSequentialImpulseConstraintSolver;
}

void PrPhysics::Free_Engine ( void )
{
	delete m_solver;
	m_solver = NULL;

	delete m_solver;
	m_solver = NULL;

	delete m_solver;
	m_solver = NULL;
}

//	UpdateSimulationTarget (deltaTime) : prepares simulation for given amount of time.
void PrPhysics::UpdateSimulationTarget ( Real elasped_time )
{
	m_simulationTarget += elasped_time;
}

//	SimulationAtTarget() : Simulation has reached end of a sim?
bool PrPhysics::SimulationAtTarget ( void )
{
	return m_simulationTime >= m_simulationTarget;
}

//	SimulationAtSubstep() : Simulation has reached end of sub-sim?
bool PrPhysics::SimulationAtSubstep ( void )
{
	return true;
}

Real PrPhysics::SimulationOvershoot ( Real timestep )
{
	if (SimulationAtTarget())
		return (Real)((m_simulationTarget - m_simulationTime + timestep) / timestep);
	else
		return 1.0F;
}

//	Simulate() : Simulate physics.
void PrPhysics::Simulate ( Real timestep )
{
	m_simulationTime += timestep;
	for (PrWorld* world : m_worlds)
	{
		if (world != NULL)
		{
			world->ApiWorld()->stepSimulation(timestep, 1, timestep);

			if (world->ApiWorld() && world->ApiWorld()->getDebugDrawer())
			{
				world->ApiWorld()->debugDrawWorld();
			}
		}
	}
}