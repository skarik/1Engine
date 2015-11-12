
#include "physical/physics/phantom/hkArPhantomCallbackShape.h"
#include "hkArDefaultDistributor.h"

// Need the CGameState class to query CBehavior information and set raycast information.
#include "engine/state/CGameState.h"

void hkArDefaultDistributor::phantomEnterEvent ( const hkpCollidable *phantomColl, const hkpCollidable *otherColl, const hkpCollisionInput &env )
{
	/*sCollision result;
	//result.pOther = phantomColl->getOwner())
	//result.vPos = otherColl->
	CGameState::Active()->GetBehavior( ((hkpRigidBody*)(phantomColl->getOwner()))->getUserData() )->OnCollisionEnter(result);
	CGameState::Active()->GetBehavior( ((hkpRigidBody*)(otherColl->getOwner()))->getUserData() )->OnCollisionEnter(result);*/
	throw Core::NotYetImplementedException();
}

void hkArDefaultDistributor::phantomLeaveEvent ( const hkpCollidable *phantomColl, const hkpCollidable *otherColl )
{
	/*sCollision result;
	CGameState::Active()->GetBehavior( ((hkpRigidBody*)(phantomColl->getOwner()))->getUserData() )->OnCollisionLeave(result);
	CGameState::Active()->GetBehavior( ((hkpRigidBody*)(otherColl->getOwner()))->getUserData() )->OnCollisionLeave(result);*/
	throw Core::NotYetImplementedException();
}
