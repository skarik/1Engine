#include "PrCast.h"

#include "physical/physics/motion/IPrRigidBody.h"

#include "physical/physics/PrPhysics.h"
#include "physical/physics/PrWorld.h"
#include "physical/physics/shapes/PrShape.h"
#include "physical/interface/tobt.h"

struct prClosestNotMeConvexResultCallback : public btCollisionWorld::ClosestConvexResultCallback
{
	btCollisionObject* m_me;

	prClosestNotMeConvexResultCallback ( btCollisionObject *me, const btVector3 &fromA, const btVector3 &toA )
		: btCollisionWorld::ClosestConvexResultCallback(fromA, toA),
		m_me(me)
	{
	}

	virtual btScalar addSingleResult(btCollisionWorld::LocalConvexResult& convexResult, bool normalInWorldSpace)
	{
		// ignore self
		if ( convexResult.m_hitCollisionObject == m_me )
			return 1.0f;
		
		// ignore result if there is no contact response
		//if ( !convexResult.m_hitCollisionObject->hasContactResponse() )
		//	return 1.0f;

		return ClosestConvexResultCallback::addSingleResult( convexResult, normalInWorldSpace );
	}
};

/*PrCast::PrCast ( const prRaycastQuery& query )
{

}*/

PrCast::PrCast ( const prShapecastQuery& query )
	: m_hits(NULL), m_hits_count(0)
{
	PrWorld* world = query.world;
	if ( world == NULL )
	{
		world = PrPhysics::Active()->CurrentWorld();
		if ( world == NULL ) throw core::NullReferenceException();
	}
	if ( query.shape == NULL || (!query.shape->ApiShape()->isConvex() && !query.shape->ApiShape()->isConvex2d()) )
	{
		throw core::InvalidArgumentException();
	}

	// Check for single collision:
	if ( query.maxHits == 1 )
	{
		prClosestNotMeConvexResultCallback collisionCollector(
			(query.ignore == NULL) ? NULL : query.ignore->ApiBody(),
			physical::bt(query.start.position),
			physical::bt(query.end.position)
		);
		collisionCollector.m_collisionFilterGroup = query.collision.layer;

		world->ApiWorld()->convexSweepTest(
			(btConvexShape*)query.shape->ApiShape(),
			physical::bt(query.start),
			physical::bt(query.end),
			collisionCollector,
			query.allowedPenetration
		);

		// If we have a single hit, save it.
		if (collisionCollector.hasHit())
		{
			m_hits_count = 1;
			
			// Set the hit info
			m_hits = new RaycastHit[1];
			m_hits[0].hit = true;
			m_hits[0].fraction = collisionCollector.m_closestHitFraction;
		}
	}
	// Actually collect collisions:
	else
	{
		throw core::NotYetImplementedException();
	}
}

PrCast::~PrCast ( void )
{
	delete[] m_hits;
	m_hits = NULL;

	m_hits_count = 0;
}


bool PrCast::Hit ( void ) const
{
	return m_hits_count > 0;
}
