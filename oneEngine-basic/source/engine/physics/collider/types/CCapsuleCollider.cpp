
#include "engine/physics/motion/CRigidbody.h"
#include "CCapsuleCollider.h"

#include "physical/physics/shapes/physCapsuleShape.h"


CCapsuleCollider::CCapsuleCollider ( ftype height, ftype radius, bool centered )
	: fFootOffset(0)
{
	vSize = Vector3d( 0,0,height );
	fRadius = radius;

	if ( !centered )
		pCollisionShape = new physCapsuleShape ( Vector3d( 0,0,fRadius ), Vector3d( 0,0,height-fRadius ), radius );
		//pCollisionShape = Physics::CreateCapsuleShape( Vector3d( 0,0,fRadius ), Vector3d( 0,0,height-fRadius ), radius );
	else
		pCollisionShape = new physCapsuleShape ( Vector3d( 0,0,-((height/2)-fRadius) ), Vector3d( 0,0,((height/2)-fRadius) ), radius );
		//pCollisionShape = Physics::CreateCapsuleShape( Vector3d( 0,0,-((height/2)-fRadius) ), Vector3d( 0,0,((height/2)-fRadius) ), radius );
}
CCapsuleCollider::~CCapsuleCollider ( void )
{
	delete_safe(pCollisionShape);
}

// Setters
void CCapsuleCollider::SetSize ( ftype height, ftype radius )
{
	// Set the current extents of the collider
	//vExtents = size;
	vSize = Vector3d( 0,0,height );
	fRadius = radius;

	// Get the Havok object and set the size of it.
	//((hkpCapsuleShape*)pCollisionShape)->setHalfExtents( hkVector4( vExtents.x*0.5f, vExtents.y*0.5f, vExtents.z*0.5f ) );
	((physCapsuleShape*)pCollisionShape)->setVertex( 0, Vector3d( 0,0,radius ) );
	((physCapsuleShape*)pCollisionShape)->setVertex( 1, Vector3d( 0,0,height-radius ) );
	((physCapsuleShape*)pCollisionShape)->setRadius( radius );
	// Change the rigid body's current shape to the new collider object
	pRigidBody->SetShape( pCollisionShape );

}
void CCapsuleCollider::SetHeight ( ftype height )
{
	((physCapsuleShape*)pCollisionShape)->setVertex( 0, Vector3d( 0,0,fFootOffset+fRadius ) );
	((physCapsuleShape*)pCollisionShape)->setVertex( 1, Vector3d( 0,0,fFootOffset+height-fRadius ) );
	// Change the rigid body's current shape to the new collider object
	pRigidBody->SetShape( pCollisionShape );
}
void CCapsuleCollider::SetFootOffset ( ftype foot )
{
	fFootOffset = foot;
}

// Getters
Vector3d CCapsuleCollider::GetSize ( void )
{
	return vSize;
}
ftype CCapsuleCollider::GetRadius ( void )
{
	return fRadius;
}
ftype CCapsuleCollider::GetFootOffset ( void )
{
	return fFootOffset;
}