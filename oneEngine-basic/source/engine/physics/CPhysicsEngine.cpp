
//#include "physical/liscensing.cxx" // Include liscense info

#include "physical/physics/CPhysics.h"
#include "physical/physics/motion/physRigidbody.h"
#include "engine/physics/CPhysicsEngine.h"
#include "engine/state/CGameState.h"
#include "engine/physics/motion/CRigidbody.h"

// Need the CGameState class to query CBehavior information and set raycast information.
#include "engine/state/CGameState.h"
// Need the CCollider class to set collider callbacks 
//#include "engine/physics/collider/CCollider.h"

void PhysicsEngine::Update ( float deltaTime, CGameState* pGameState, void (CGameState::*pt2FixedUpdate)(void), void(CGameState::*pt2RigidbodyUpdate)(void) )
{
	static float previousDeltaTime = 0;

	//CPhysics* instance = Physics::Active();
	//Physics::World()->advanceTime();
	//printf( "%x - %x - %x - %x\n", Physics::Instance, Physics::Active(), Physics::Instance->World(), Physics::World() );
	//hkpWorld* world = Physics::Instance->World();
	Physics::UpdateSimulationTarget( deltaTime );
	//int repeatCount = 0;
	while ( !Physics::SimulationAtTarget() /*&& (repeatCount++ < 1000)*/ )
	{
		//HK_ASSERT( 0x11179564, pWorld->isSimulationAtPsi() );
		if ( Physics::SimulationAtSubstep() == false ) {
			std::cout << "NOOO" << std::endl;
			break;
		}
		{
			// Save old times
			ftype tempDelta		= Time::deltaTime;
			ftype tempSmooth	= Time::smoothDeltaTime;
			// Set delta time to use for timesteps
			Time::deltaTime			= Time::targetFixedTime;
			Time::smoothDeltaTime	= Time::targetFixedTime;
			// Interact from game to physics
			(pGameState->*pt2FixedUpdate) ();
			// Restore previous times
			Time::deltaTime			= tempDelta;
			Time::smoothDeltaTime	= tempSmooth;
		}
		// Step simulation forward
		Physics::Simulate( Time::targetFixedTime );

		// Interact with physics: physics data to game data
		if ( Physics::SimulationAtSubstep() == true ) {
			(pGameState->*pt2RigidbodyUpdate) ();
		}

		//repeatCount++;
		//cout << repeatCount << " " << deltaTime << " " << previousDeltaTime << endl;
	}
	previousDeltaTime = deltaTime;
	//if ( pWorld->isSimulationAtPsi() ) {
	// Interact with physics: physics data to game data
	//(pGameState->*pt2RigidbodyUpdate) ();
	//}
}


void PhysicsEngine::UpdateThreaded ( float frameDeltaTime, float fixedDeltaTime, CGameState* pGameState, void (CGameState::*pt2FixedUpdate)(void), void(CGameState::*pt2RigidbodyUpdate)(void) )
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




//==Raytracing==
// Cast a ray
#include "engine/physics/motion/CMotion.h"
//#include "engine/physics/motion/CRagdollCollision.h"

void PhysicsEngine::Raycast ( Ray const& rDir, ftype fCastDist, RaycastHit * outHitInfo, const physCollisionFilter& collisionFilter, void* mismatch )
{
#ifdef PHYSICS_USING_HAVOK
	// Check for proper input
	if ( outHitInfo == NULL ) {
		throw Core::InvalidArgumentException();
	}

	// Set physics lock state
	Physics::ReadLock();
	/*
	// Create the raycast information.
	hkpWorldRayCastInput hkInputCastRay;
	// Set the start position of the ray
	hkInputCastRay.m_from = physVector4( rDir.pos.x, rDir.pos.y, rDir.pos.z, 0 );
	// Set the end position of the ray
	hkInputCastRay.m_to = hkInputCastRay.m_from;
	hkInputCastRay.m_to.add( physVector4( rDir.dir.x*fCastDist, rDir.dir.y*fCastDist, rDir.dir.z*fCastDist, 0 ) );
	// Set the ray's collision mask
	hkInputCastRay.m_filterInfo = collisionFilter;
	*/
	std::vector<std::pair<ContactPoint,gameid_t>> hitCollection;
	Ray castRay = rDir;
	castRay.dir = castRay.dir.normal() * fCastDist;

	if ( mismatch )
	{
	// Create a ray hit accumulation object
		//hkpAllRayHitCollector hkRayHitCollection;

		// Cast a ray into the lovely world
		//Physics::World()->castRay ( hkInputCastRay, hkRayHitCollection );
		//Physics::Raycast ( hkInputCastRay, hkRayHitCollection );
		Physics::Raycast ( collisionFilter, castRay, Physics::CASTRESULT_MULTIPLE, hitCollection );

		// If the collection has a hit recorded, then we want to grab the hit info.
		// Otherwise, we want to set the outHitInfo to report no hit.
		//if ( hkRayHitCollection.hasHit() )
		ContactPoint* targetHit = NULL;
		CGameBehavior* targetBehavior = NULL;
		bool hasValidHit = false;
		//hkpWorldRayCastOutput hkRayHitOutput;
		//if ( !hkRayHitCollection.getHits().isEmpty() )
		if ( !hitCollection.empty() )
		{
			// Grab the hit info
			for ( uint i = 0; i < hitCollection.size(); ++i )
			{
				// Check for the owner of the Rigidbody
				hasValidHit = true;
				CGameBehavior* behavior = CGameState::Active()->GetBehavior( (gameid_t)hitCollection[i].second );
				if ( behavior )
				{
					if ( behavior->GetTypeName() == "CRigidbody" || behavior->GetTypeName() == "CRagdollCollision" ) {
						if ( ((CMotion*)behavior)->GetOwner() == mismatch ) {
							hasValidHit = false;
						}
					}
					// If found valid hit, then stop
					if ( hasValidHit ) {
						targetHit = &hitCollection[i].first;
						targetBehavior = behavior;
						break;
					}
				}
				else {
					if ( hitCollection[i].second == 0 ) {
						// Has a general collision.
						throw std::exception( "Bad collision hit. All collisions must have an attached gamebehavior!" );
					}
					else {
						throw std::exception( "Bad collision hit. Object mentioned in userData does not exist." );
					}
				}
			}
		}

		if ( hasValidHit )
		{
			// Mark the raytrace as a hit
			outHitInfo->hit = true;
			// Save the hit information
			outHitInfo->distance = targetHit->distance * fCastDist;
			outHitInfo->hitPos = rDir.pos + rDir.dir * outHitInfo->distance;
			outHitInfo->hitNormal = targetHit->normal;

			//gameid_t targetBehaviorId = (gameid_t)((hkpRigidBody*)(hkRayHitOutput.m_rootCollidable->getOwner()))->getUserData();
			//outHitInfo->pHitBehavior = CGameState::Active()->GetBehavior( targetBehaviorId );
			outHitInfo->pHitBehavior = targetBehavior;

#		ifdef _ENGINE_DEBUG
			if ( outHitInfo->pHitBehavior != NULL ) {
				outHitInfo->pHitBody = (dynamic_cast<CMotion*>(outHitInfo->pHitBehavior))->GetBody();
			}
			else {
				std::cout << "Hit an invalid physics object. " << __FILE__ << " at " << __LINE__ << std::endl;
			}
#		else
			if ( outHitInfo->pHitBehavior != NULL ) {
				outHitInfo->pHitBody = ((CRigidBody*)outHitInfo->pHitBehavior)->GetBody(); //(hkpRigidBody*) hkRayHitOutput.m_rootCollidable->getOwner();
			}
#		endif
		}
		else
		{
			outHitInfo->hit = false;
			outHitInfo->distance = -1.0f;
		}
	}
	else
	{
		// Cast a ray into the lovely world
		//Physics::World()->castRay ( hkInputCastRay, hkRayHitCollection );
		//Physics::Raycast ( hkInputCastRay, hkRayHitCollection );
		Physics::Raycast ( collisionFilter, castRay, Physics::CASTRESULT_SINGLE, hitCollection );

		// If the collection has a hit recorded, then we want to grab the hit info.
        // Otherwise, we want to set the outHitInfo to report no hit.
        //if ( hkRayHitCollection.hasHit() )
		if ( !hitCollection.empty() )
        {
			// Mark the raytrace as a hit
			outHitInfo->hit = true;
			// Save the hit information
			outHitInfo->distance = hitCollection[0].first.distance * fCastDist;
			outHitInfo->hitPos = rDir.pos + rDir.dir * outHitInfo->distance;
			outHitInfo->hitNormal = hitCollection[0].first.normal;

			outHitInfo->pHitBehavior = CGameState::Active()->GetBehavior( (gameid_t)hitCollection[0].second );
#		ifdef _ENGINE_DEBUG
			if ( outHitInfo->pHitBehavior != NULL ) {
				outHitInfo->pHitBody = (dynamic_cast<CMotion*>(outHitInfo->pHitBehavior))->GetBody();
			}
			else {
				std::cout << "Hit an invalid physics object. " << __FILE__ << " at " << __LINE__ << std::endl;
			}
#		else
			if ( outHitInfo->pHitBehavior != NULL ) {
				outHitInfo->pHitBody = ((CRigidBody*)outHitInfo->pHitBehavior)->GetBody(); //(hkpRigidBody*) hkRayHitOutput.m_rootCollidable->getOwner();
			}
#		endif
        }
        else
        {
			outHitInfo->hit = false;
			outHitInfo->distance = -1.0f;
        }
	}
	
	Physics::ReadUnlock();
	
	return;
#endif//PHYSICS_USING_HAVOK
#ifdef PHYSICS_USING_BOX2D
	throw Core::NotYetImplementedException();
#endif
	
}


// Cast a shape
void PhysicsEngine::Linearcast ( Ray const& rDir, ftype fCastDist, physShape* pShape, RaycastHit* outHitInfo, const int hitInfoArrayCount, const physCollisionFilter& collisionFilter, void* mismatch )
{
#ifdef PHYSICS_USING_HAVOK
	Physics::ReadLock();

	//hkMotionState* ms = new hkMotionState;
	// Create a new transform
	/*hkTransform* ms = new hkTransform;
	// Set the start position of the collidable
	ms->setIdentity();
	ms->setTranslation( physVector4( rDir.pos.x, rDir.pos.y, rDir.pos.z, 0 ) );*/

	/*hkpCollidable* collidable = new hkpCollidable( pShape->getShape(), ms );
	// Set the collidable's collision mask
	collidable->setCollisionFilterInfo( collisionFilter );

	// Create the linearcast information
	hkpLinearCastInput hkInputCastRay; // FUCK THIS LINE IN PARTICULAR
	// Set the end position of the ray
	hkInputCastRay.m_to = physVector4( rDir.pos.x, rDir.pos.y, rDir.pos.z, 0 );
	hkInputCastRay.m_to.add( physVector4( rDir.dir.x*fCastDist, rDir.dir.y*fCastDist, rDir.dir.z*fCastDist, 0 ) );
	// Set max pentration depth before reported hit
	hkInputCastRay.m_maxExtraPenetration = 0.06f;*/

	// Create the linecast accumulation object
	//hkpAllCdPointCollector hkLineHitCollection;

	// Cast the shape into the world
	//Physics::World()->linearCast( collidable, hkInputCastRay, hkLineHitCollection );
	//Physics::Linearcast( collidable, hkInputCastRay, hkLineHitCollection );

	std::vector<std::pair<ContactPoint,gameid_t>> hitCollection;
	Ray castRay = rDir;
	castRay.dir = castRay.dir.normal() * fCastDist;
	Physics::Linearcast( collisionFilter, pShape, castRay, hitCollection );
	//Physics::Linearcast( collisionFilter, pShape, castRay, hkLineHitCollection );


	// If the collection has hits recorded, we want to grab them.
	// We will return the first 'hitInfoArrayCount' results, sorted by distance.
	int currentHitInfo = 0;
	bool hasValidHit;
	for ( uint i = 0; i < hitCollection.size() && (currentHitInfo < hitInfoArrayCount); ++i )
	{
		// Check the hit's owner for invalid by checking the owner of the hit object
		hasValidHit = true;
		CGameBehavior* behavior = CGameState::Active()->GetBehavior( (gameid_t)hitCollection[i].second );
		if ( mismatch )
		{
			if ( behavior->GetTypeName() == "CRigidbody" || behavior->GetTypeName() == "CRagdollCollision" ) 
			{
				if ( ((CMotion*)behavior)->GetOwner() == mismatch ) 
				{
					hasValidHit = false;
				}
			}
		}

		// If it's a valid hit, add it to the list
		if ( hasValidHit )
		{
			outHitInfo[currentHitInfo].hit = true;
			outHitInfo[currentHitInfo].distance = hitCollection[i].first.distance * fCastDist;
			//outHitInfo[currentHitInfo].hitPos = hkLineHitOutput.m_contact.getPosition();
			outHitInfo[currentHitInfo].hitPos = hitCollection[i].first.position;
			//outHitInfo[currentHitInfo].hitPos = hkLineHitOutput.m_contact.getPosition();
			outHitInfo[currentHitInfo].hitNormal = hitCollection[i].first.normal;

			outHitInfo[currentHitInfo].pHitBehavior = behavior;
			outHitInfo[currentHitInfo].pHitBody = ((CRigidBody*)behavior)->GetBody();//(hkpRigidBody*) hkLineHitOutput.m_rootCollidableB->getOwner();

			// Increment hitinfo index
			currentHitInfo += 1;
		}
	}
	/*
	if ( hkLineHitCollection.hasHit() )
	{
		// Grab the hits
		hkLineHitCollection.sortHits();

		bool hasValidHit;
		hkpRootCdPoint hkLineHitOutput;
		for ( int i = 0; ( i < hkLineHitCollection.getNumHits() )&&( currentHitInfo < hitInfoArrayCount ); ++i )
		{
			// Get the hit
			hkLineHitOutput = hkLineHitCollection.getHits()[i];

			// Check the hit's owner for invalid by checking the owner of the hit object
			hasValidHit = true;
			CGameBehavior* behavior = CGameState::Active()->GetBehavior( (gameid_t)((hkpRigidBody*)(hkLineHitOutput.m_rootCollidableB->getOwner()))->getUserData() );
			if ( mismatch ) {
				if ( behavior->GetTypeName() == "CRigidbody" || behavior->GetTypeName() == "CRagdollCollision" ) {
					if ( ((CMotion*)behavior)->GetOwner() == mismatch ) {
						hasValidHit = false;
					}
				}
			}

			// If it's a valid hit, add it to the list
			if ( hasValidHit )
			{
				outHitInfo[currentHitInfo].hit = true;
				outHitInfo[currentHitInfo].distance = hkLineHitOutput.m_contact.getDistance() * fCastDist;
				//outHitInfo[currentHitInfo].hitPos = hkLineHitOutput.m_contact.getPosition();
				hkLineHitOutput.m_contact.getPosition().store3( &outHitInfo[currentHitInfo].hitPos.x );
				//outHitInfo[currentHitInfo].hitPos = hkLineHitOutput.m_contact.getPosition();
				hkLineHitOutput.m_contact.getNormal().store3( &outHitInfo[currentHitInfo].hitNormal.x );

				outHitInfo[currentHitInfo].pHitBehavior = behavior;
				outHitInfo[currentHitInfo].pHitBody = ((CRigidBody*)behavior)->GetBody();//(hkpRigidBody*) hkLineHitOutput.m_rootCollidableB->getOwner();

				// Increment hitinfo index
				currentHitInfo += 1;
			}
		}
	}
	*/
	// Report no hits or end of list
	if ( currentHitInfo == 0 ) {
		outHitInfo->hit = false; // Still on hit zero, no hit
	}
	else { // Mark last valid value in list as no hit to mark end of hit list
		if ( currentHitInfo < hitInfoArrayCount ) {
			outHitInfo[currentHitInfo].hit = false;
		}
	}
	
	Physics::ReadUnlock();
#endif//PHYSICS_USING_HAVOK
#ifdef PHYSICS_USING_BOX2D
	throw Core::NotYetImplementedException();
#endif
}



//==Raytracing==
// Cast a ray
/*void			Physics::Raycast ( Ray const& rDir, ftype fCastDist, RaycastHit * outHitInfo, uint32_t collisionFilter, void* mismatch )
{
	//pWorld->lock();
	pWorld->markForRead();
	//pWorld->markForWrite();

	// Create the raycast information.
	hkpWorldRayCastInput hkInputCastRay;
	// Set the start position of the ray
	hkInputCastRay.m_from = hkVector4( rDir.pos.x, rDir.pos.y, rDir.pos.z, 0 );
	// Set the end position of the ray
	hkInputCastRay.m_to = hkInputCastRay.m_from;
	hkInputCastRay.m_to.add( hkVector4( rDir.dir.x*fCastDist, rDir.dir.y*fCastDist, rDir.dir.z*fCastDist, 0 ) );
	// Set the ray's collision mask
	hkInputCastRay.m_filterInfo = collisionFilter;

	// Create a ray hit accumulation object
	//hkpClosestRayHitCollector hkRayHitCollection;
	hkpAllRayHitCollector hkRayHitCollection;

	// Cast a ray into the lovely world
	pWorld->castRay ( hkInputCastRay, hkRayHitCollection );

	// If the collection has a hit recorded, then we want to grab the hit info.
	// Otherwise, we want to set the outHitInfo to report no hit.
	//if ( hkRayHitCollection.hasHit() )
	bool hasValidHit = false;
	hkpWorldRayCastOutput hkRayHitOutput;
	if ( !hkRayHitCollection.getHits().isEmpty() )
	{
		// Grab the hit info
		//hkpWorldRayCastOutput hkRayHitOutput = hkRayHitCollection.getHit();
		hkRayHitCollection.sortHits();
		for ( uint i = 0; i < hkRayHitCollection.getHits().getSize(); ++i )
		{
			hkRayHitOutput = hkRayHitCollection.getHits()[i];
			
			hasValidHit = true;

			// Check for the owner of the Rigidbody
			CGameBehavior* behavior = CGameState::pActive->GetBehavior( ((physRigidBody*)(hkRayHitOutput.m_rootCollidable->getOwner()))->getUserData() );
			if ( behavior->GetTypeName() == "CRagdollCollision" ) {
				(CRagdollCollision*)behavior
			}
			else if ( behavior->GetTypeName() == "CRigidbody" ) {

			}
			if ( hasValidHit ) {
				break;
			}
		}
	}

	if ( hasValidHit )
	{
		// Mark the raytrace as a hit
		outHitInfo->hit = true;
		// Save the hit information
		outHitInfo->distance = hkRayHitOutput.m_hitFraction * fCastDist;
		outHitInfo->hitPos = rDir.pos + rDir.dir * outHitInfo->distance;
		//outHitInfo->hitNormal = Vector3d( hkRayHitOutput.m_normal, ;
		hkRayHitOutput.m_normal.store3( &(outHitInfo->hitNormal.x) );
		//outHitInfo->pHitBehavior = hkRayHitOutput.m_shapeKeys[0]
		//outHitInfo->pHitBehavior = hkRayHitCollection.
		outHitInfo->pHitBehavior = CGameState::pActive->GetBehavior( ((physRigidBody*)(hkRayHitOutput.m_rootCollidable->getOwner()))->getUserData() );
		outHitInfo->pHitBody = (physRigidBody*) hkRayHitOutput.m_rootCollidable->getOwner();
	}
	else
	{
		outHitInfo->hit = false;
		outHitInfo->distance = -1.0f;
	}

	pWorld->unmarkForRead();
	//pWorld->unmarkForWrite();
	//pWorld->unlock();
}*/