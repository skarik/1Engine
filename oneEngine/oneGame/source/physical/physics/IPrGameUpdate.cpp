#include "IPrGameUpdate.h"

#include "core/exceptions/exceptions.h"
#include "core/time/Time.h"
#include "physical/physics/PrPhysics.h"

void IPrGameUpdate::PhysicsUpdate ( const float deltaTime )
{
	PrPhysics* physics = PrPhysics::Active();
	if (physics == NULL)
	{
		throw core::InvalidInstantiationException();
	}

	// Give the physics X amount of time passed.
	physics->UpdateSimulationTarget( deltaTime );
	// While we haven't caught up to the time passed...
	while ( !physics->SimulationAtTarget() )
	{
		if ( physics->SimulationAtSubstep() == false ) {
			throw core::YouSuckException();
			break;
		}

		// Update the physics:
		{
			// Save old times
			Real tempDelta	= Time::deltaTime;
			Real tempSmooth	= Time::smoothDeltaTime;
			// Set delta time to use for timesteps
			Time::deltaTime			= Time::targetFixedTime;
			Time::smoothDeltaTime	= Time::targetFixedTime;
			// Interact from game to physics
			FixedUpdate();
			// Restore previous times
			Time::deltaTime			= tempDelta;
			Time::smoothDeltaTime	= tempSmooth;
		}
		// Step simulation forward across all active worlds.
		physics->Simulate( Time::targetFixedTime );

		// Interact with physics: physics data to game data
		if ( physics->SimulationAtSubstep() )
		{
			RigidbodyUpdate(physics->SimulationOvershoot(Time::targetFixedTime));
		}
	}
	///previousDeltaTime = deltaTime;
	//if ( pWorld->isSimulationAtPsi() ) {
	// Interact with physics: physics data to game data
	//(pGameState->*pt2RigidbodyUpdate) ();
	//}
}
void IPrGameUpdate::PhysicsUpdateThreaded ( const float frameDeltaTime, const float fixedDeltaTime )
{
	// singlethreaded
	/*Physics::World()->setFrameTimeMarker( frameDeltaTime );

	Physics::World()->advanceTime();
	Physics::UpdateSimulationTarget( frameDeltaTime );

	while ( !Physics::World()->isSimulationAtMarker() ) 
	{
	HK_ASSERT( 0x11179564, Physics::pWorld->isSimulationAtPsi() );

	{
	// Interact from game to physics
	(pGameState->*pt2FixedUpdate) ();
	}

	//pWorld->stepDeltaTime( fixedDeltaTime );
	Physics::World()->stepMultithreaded( Physics::JobQueue(), Physics::ThreadPool(), fixedDeltaTime );

	//if ( pWorld->isSimulationAtPsi() )
	{
	// Interact with physics: physics data to game data
	(pGameState->*pt2RigidbodyUpdate) ();
	}
	}

	#ifdef _HAVOK_VISUAL_DEBUGGER_
	// Debugger
	//context->syncTimers( threadPool );
	//Physics::VDB()->step();
	#endif

	// Clear accumulated timer data in this thread and all slave threads
	hkMonitorStream::getInstance().reset();
	Physics::ThreadPool()->clearTimerData();*/
}