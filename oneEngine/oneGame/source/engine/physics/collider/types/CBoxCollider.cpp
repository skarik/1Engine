
#include "engine/physics/motion/CRigidbody.h"
#include "CBoxCollider.h"

#include "physical/physics/shapes/physBoxShape.h"

CBoxCollider::CBoxCollider ( Vector3d const& size )
{
	vExtents = size;
	vCenterOfMass = Vector3d( 0,0,0 );

	//pCollisionShape = Physics::CreateBoxShape( vExtents * 0.5f );
	pCollisionShape = new physBoxShape( vExtents * 0.5f );
}
CBoxCollider::CBoxCollider ( BoundingBox const& bbox, Vector3d const& pos )
{
	vExtents = bbox.GetSize();
	//vCenterOfMass = bbox.GetCenterPoint() - pos;
	vCenterOfMass = pos;

	//pCollisionShape = Physics::CreateBoxShape( vExtents * 0.5f );
	//pCollisionShape = Physics::CreateBoxShape( vExtents * 0.5f, bbox.GetCenterPoint() );
	pCollisionShape = new physBoxShape( vExtents * 0.5f, bbox.GetCenterPoint() );
}
CBoxCollider::~CBoxCollider ( void )
{
	Physics::FreeShape( pCollisionShape );
}

// Setters
void CBoxCollider::SetSize ( Vector3d const& size )
{
	// Set the current extents of the collider
	vExtents = size;

	// Get the Havok object and set the size of it.
	((physBoxShape*)pCollisionShape)->setHalfExtents( Vector3d( vExtents.x*0.5f, vExtents.y*0.5f, vExtents.z*0.5f ) );
	// Change the rigid body's current shape to the new collider object
	if ( pRigidBody )
		pRigidBody->SetShape( pCollisionShape );

}
void CBoxCollider::Set ( BoundingBox const& bbox, Vector3d const& pos )
{
	vExtents = bbox.GetSize();
	vCenterOfMass = bbox.GetCenterPoint() - pos;

	// Get the Havok object and set the size of it.
	((physBoxShape*)pCollisionShape)->setHalfExtents( Vector3d( vExtents.x*0.5f, vExtents.y*0.5f, vExtents.z*0.5f ) );
	// Change the rigid body's current shape to the new collider object
	if ( pRigidBody )
		pRigidBody->SetShape( pCollisionShape );
}

// Getters
Vector3d CBoxCollider::GetSize ( void )
{
	return vExtents;
}