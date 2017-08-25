

#include "CTestPhysicsFloor.h"
#include "core/time/time.h"
#include "engine/physics/collider/types/CBoxCollider.h"
#include "engine/physics/motion/CRigidbody.h"
#include "renderer/object/shapes/CCubeRenderablePrimitive.h"

CTestPhysicsFloor::CTestPhysicsFloor ( void )
	: CGameObject()
{
	pCollider = new CBoxCollider ( Vector3d( 128,128,1 ) );
	pRigidbody = new CRigidBody ( pCollider, this );
	pRigidbody->SetMotionType( physMotion::MOTION_FIXED );
	pRigidbody->SetCollisionLayer( Layers::PHYS_LANDSCAPE );
	pRenderable = new CCubeRenderablePrimitive ( 128,128,1 );
	pRenderable->transform.SetParent( &transform );
}
CTestPhysicsFloor::~CTestPhysicsFloor ( void )
{
	delete pRenderable;
	delete pRigidbody;
	delete pCollider;
}

void CTestPhysicsFloor::Update ( void )
{
	// Shrink the floor. Update both the renderer and the collider.
	/*Vector3d vSize = pCollider->GetSize();	// Get the current size
	vSize.x -= Time::deltaTime*4.0f;	// Decrement it 4 feet per second
	vSize.y -= Time::deltaTime*4.0f;
	pCollider->SetSize( vSize );		//  Set the size of the collider.
	pRenderable->SetSize( vSize.x, vSize.y, vSize.z ); // Set the size of the rendered floor.*/
}