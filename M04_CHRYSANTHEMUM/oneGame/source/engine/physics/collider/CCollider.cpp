
#include "engine/physics/motion/CRigidbody.h"
#include "CCollider.h"

//==Collision callbacks==
void CCollider::OnCollide ( sCollision& collisionInfo )
{
	CGameBehavior* target = (CGameBehavior*)pRigidBody->GetTargetTransform()->owner;
	if ( target ) {
		target->OnCollide( collisionInfo );
	}
}
void CCollider::OnEnter ( sCollision& collisionInfo )
{
	CGameBehavior* target = (CGameBehavior*)pRigidBody->GetTargetTransform()->owner;
	if ( target ) {
		target->OnCollisionEnter( collisionInfo );
	}
}
void CCollider::OnLeave ( sCollision& collisionInfo )
{
	CGameBehavior* target = (CGameBehavior*)pRigidBody->GetTargetTransform()->owner;
	if ( target ) {
		target->OnCollisionLeave( collisionInfo );
	}
}