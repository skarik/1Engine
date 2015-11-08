// Need the CGameState class to query CBehavior information and set raycast information.
#include "engine/state/CGameState.h"
// Need the CCollider class to set collider callbacks 
#include "engine/physics/collider/CCollider.h"
// Need the Phantom implementation
//#include "engine/physics/phantom/hkArPhantomCallbackShape.h"

//==Trigger Phantoms==
// Creation of a trigger phantom
/*hkpRigidBody* PhysicsEngine::CreateTriggerVolume ( physRigidBodyInfo* pBodyInfo, physShape* pShape )
{
	// Create a new callback shape
	hkArPhantomCallbackShape* myPhantomShape = new hkArPhantomCallbackShape();
	// Create a compound shape with the designated shape
	hkpBvShape* bvShape = new hkpBvShape( pShape, myPhantomShape );
	myPhantomShape->removeReference();

	pBodyInfo->m_shape = bvShape;

	hkpRigidBody* pRigidBody = new hkpRigidBody( *pBodyInfo );
	//Physics::World()->addEntity( pRigidBody );
	Physics::AddEntity( pRigidBody );

	bvShape->removeReference();

	return pRigidBody;
}*/