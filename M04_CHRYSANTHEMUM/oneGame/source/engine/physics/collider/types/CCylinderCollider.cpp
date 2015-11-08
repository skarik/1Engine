
#include "engine/physics/motion/CRigidbody.h"
#include "CCylinderCollider.h"


CCylinderCollider::CCylinderCollider ( ftype height, ftype radius, bool centered )
{
	vSize = Vector3d( 0,0,height );
	fRadius = radius;

	if ( !centered )
		pCollisionShape = Physics::CreateCylinderShape( Vector3d( 0,0,0 ), Vector3d( 0,0,height ), radius );
	else
		pCollisionShape = Physics::CreateCylinderShape( Vector3d( 0,0,-height/2 ), Vector3d( 0,0,height/2 ), radius );
}
CCylinderCollider::~CCylinderCollider ( void )
{
	Physics::FreeShape( pCollisionShape );
}

// Setters
void CCylinderCollider::SetSize ( ftype height, ftype radius )
{
	// Set the current extents of the collider
	//vExtents = size;
	vSize = Vector3d( 0,0,height );
	fRadius = radius;

	// Get the Havok object and set the size of it.
	//((hkpCapsuleShape*)pCollisionShape)->setHalfExtents( hkVector4( vExtents.x*0.5f, vExtents.y*0.5f, vExtents.z*0.5f ) );
	((hkpCylinderShape*)pCollisionShape)->setVertex( 0, hkVector4( 0,0,0 ) );
	((hkpCylinderShape*)pCollisionShape)->setVertex( 1, hkVector4( 0,0,height ) );
	((hkpCylinderShape*)pCollisionShape)->setRadius( radius );
	// Change the rigid body's current shape to the new collider object
	pRigidBody->SetShape( pCollisionShape );

}
void CCylinderCollider::SetHeight ( ftype height )
{
	((hkpCylinderShape*)pCollisionShape)->setVertex( 0, hkVector4( 0,0,0 ) );
	((hkpCylinderShape*)pCollisionShape)->setVertex( 1, hkVector4( 0,0,height ) );
	// Change the rigid body's current shape to the new collider object
	pRigidBody->SetShape( pCollisionShape );
}

// Getters
Vector3d CCylinderCollider::GetSize ( void )
{
	return vSize;
}
ftype CCylinderCollider::GetRadius ( void )
{
	return fRadius;
}