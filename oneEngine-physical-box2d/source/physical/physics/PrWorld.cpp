#include "physical/physics/PrWorld.h"
#include "physical/physics/PrPhysics.h"
#include "physical/interface/tobt.h"
#include "physical/physics/motion/IPrGameMotion.h"

#include "bullet/btBulletDynamicsCommon.h" // Todo: minimize this header

PrWorld::PrWorld ( const prWorldCreationParams& params )
	: m_enabled(false)
{
	m_broadphase	= new btDbvtBroadphase();
	m_dynamicsWorld	= new btDiscreteDynamicsWorld(
		PrPhysics::Active()->GetDispatcher(),
		m_broadphase,
		PrPhysics::Active()->GetSolver(), 
		PrPhysics::Active()->GetCollisionConfiguration());

	m_dynamicsWorld->setGravity( physical::bt(params.m_gravity) );
}

PrWorld::~PrWorld ( void )
{
	if ( m_dynamicsWorld != NULL )
	{
		int i;
		// Destroy all associated constraints with the world:
		for (i = m_dynamicsWorld->getNumConstraints() - 1; i >= 0; i--)
		{
			m_dynamicsWorld->removeConstraint(m_dynamicsWorld->getConstraint(i));
		}
		// Notify all associated rigidbodies with the world:
		for (i = m_dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
		{
			btCollisionObject* obj = m_dynamicsWorld->getCollisionObjectArray()[i];

			//btRigidBody* body = btRigidBody::upcast(obj);
			//if (body && body->getMotionState())
			//{
			//	delete body->getMotionState();
			//}

			// Remove this object from the dynamics world
			m_dynamicsWorld->removeCollisionObject(obj);

			// Get the Engine connection:
			IPrGameMotion* gameObject = static_cast<IPrGameMotion*>(obj->getUserPointer());
			// Signal the gameObject that it has been removed from the world
			if ( gameObject != NULL ) {
				gameObject->OnWorldDestroyed();
			}
		}
	}

	delete m_dynamicsWorld;
	m_dynamicsWorld = NULL;

	delete m_broadphase;
	m_broadphase = NULL;
}

PHYS_API void PrWorld::AddVolume(IPrVolume * volume_to_add)
{
	m_volumes.push_back(volume_to_add);
}

PHYS_API void PrWorld::RemoveVolume(IPrVolume * volume_to_remove)
{
	auto l_findResult = std::find(m_volumes.begin(), m_volumes.end(), volume_to_remove);
	if ( l_findResult != m_volumes.end() )
	{
		m_volumes.erase(l_findResult);
	}
}
