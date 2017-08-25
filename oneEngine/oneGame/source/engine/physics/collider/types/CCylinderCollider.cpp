
#include "engine/physics/motion/CRigidbody.h"
#include "CCylinderCollider.h"

#include "physical/physics/shapes/physCylinderShape.h"


CCylinderCollider::CCylinderCollider ( Real height, Real radius, bool centered )
{
	vSize = Vector3d( 0,0,height );
	fRadius = radius;

	if ( !centered )
		pCollisionShape = new physCylinderShape( Vector3d( 0,0,0 ), Vector3d( 0,0,height ), radius );
		//pCollisionShape = Physics::CreateCylinderShape( Vector3d( 0,0,0 ), Vector3d( 0,0,height ), radius );
	else
		pCollisionShape = new physCylinderShape( Vector3d( 0,0,-height/2 ), Vector3d( 0,0,height/2 ), radius );
		//pCollisionShape = Physics::CreateCylinderShape( Vector3d( 0,0,-height/2 ), Vector3d( 0,0,height/2 ), radius );
}
CCylinderCollider::~CCylinderCollider ( void )
{
	Physics::FreeShape( pCollisionShape );
}

// Setters
void CCylinderCollider::SetSize ( Real height, Real radius )
{
	// Set the current extents of the collider
	//vExtents = size;
	vSize = Vector3d( 0,0,height );
	fRadius = radius;

	// Get the Havok object and set the size of it.
	//((hkpCapsuleShape*)pCollisionShape)->setHalfExtents( hkVector4( vExtents.x*0.5f, vExtents.y*0.5f, vExtents.z*0.5f ) );
	((physCylinderShape*)pCollisionShape)->setVertex( 0, Vector3d( 0,0,0 ) );
	((physCylinderShape*)pCollisionShape)->setVertex( 1, Vector3d( 0,0,height ) );
	((physCylinderShape*)pCollisionShape)->setRadius( radius );
	// Change the rigid body's current shape to the new collider object
	pRigidBody->SetShape( pCollisionShape );

}
void CCylinderCollider::SetHeight ( Real height )
{
	((physCylinderShape*)pCollisionShape)->setVertex( 0, Vector3d( 0,0,0 ) );
	((physCylinderShape*)pCollisionShape)->setVertex( 1, Vector3d( 0,0,height ) );
	// Change the rigid body's current shape to the new collider object
	pRigidBody->SetShape( pCollisionShape );
}

// Getters
Vector3d CCylinderCollider::GetSize ( void )
{
	return vSize;
}
Real CCylinderCollider::GetRadius ( void )
{
	return fRadius;
}