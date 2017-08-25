
#include "engine/physics/motion/CRigidbody.h"
#include "CCollider.h"

// callbacks should not be handled this way. instead, they should be registered w/ the IPrGameMotion nonsense!
// In fact, CCollider can be done away with entirely, and the entire shape concept placed within onePhysical.

//==Collision callbacks==
void CCollider::OnCollide ( sCollision& collisionInfo )
{
	CGameBehavior* target = pRigidBody->GetOwner();
	if ( target ) {
		target->OnCollide( collisionInfo );
	}
}
void CCollider::OnEnter ( sCollision& collisionInfo )
{
	CGameBehavior* target = pRigidBody->GetOwner();
	if ( target ) {
		target->OnCollisionEnter( collisionInfo );
	}
}
void CCollider::OnLeave ( sCollision& collisionInfo )
{
	CGameBehavior* target = pRigidBody->GetOwner();
	if ( target ) {
		target->OnCollisionLeave( collisionInfo );
	}
}