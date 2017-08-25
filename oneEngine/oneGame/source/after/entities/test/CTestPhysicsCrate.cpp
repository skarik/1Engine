
#include "CTestPhysicsCrate.h"
#include "engine/physics/collider/types/CBoxCollider.h"
#include "engine/physics/motion/CRigidbody.h"
#include "renderer/object/shapes/CCubeRenderablePrimitive.h"

CTestPhysicsCrate::CTestPhysicsCrate ( void )
{
	pCollider = new CBoxCollider ( Vector3d( 1,1,1 ) );
	pRigidbody = new CRigidBody ( pCollider, this );
	//pRigidbody->SetCollisionLayer( Layers::PHYS_DYNAMIC );
	pRenderable = new CCubeRenderablePrimitive ( 1,1,1 );
	pRenderable->transform.SetParent( &transform );
}
CTestPhysicsCrate::~CTestPhysicsCrate ( void )
{
	delete pRenderable;
	delete pRigidbody;
	delete pCollider;
}

void CTestPhysicsCrate::FixedUpdate ( void )
{
	// Keeps the crates alives.
	pRigidbody->ApplyForce( Vector3d( 0.01f, 0,0 ) );
}