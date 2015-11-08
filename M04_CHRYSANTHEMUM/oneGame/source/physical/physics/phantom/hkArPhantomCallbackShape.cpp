
#include "hkArPhantomCallbackShape.h"

// Need the CGameState class to query CBehavior information and set raycast information.
#include "engine/state/CGameState.h"

void hkArPhantomCallbackShape::phantomEnterEvent ( const hkpCollidable *phantomColl, const hkpCollidable *otherColl, const hkpCollisionInput &env )
{
	distributor->phantomEnterEvent( phantomColl, otherColl, env );

	/*sCollision result;
	//result.pOther = phantomColl->getOwner())
	//result.vPos = otherColl->
	CGameState::Active()->GetBehavior( ((hkpRigidBody*)(phantomColl->getOwner()))->getUserData() )->OnCollisionEnter(result);
	CGameState::Active()->GetBehavior( ((hkpRigidBody*)(otherColl->getOwner()))->getUserData() )->OnCollisionEnter(result);*/
}

void hkArPhantomCallbackShape::phantomLeaveEvent ( const hkpCollidable *phantomColl, const hkpCollidable *otherColl )
{
	distributor->phantomLeaveEvent( phantomColl, otherColl );

	/*sCollision result;
	CGameState::Active()->GetBehavior( ((hkpRigidBody*)(phantomColl->getOwner()))->getUserData() )->OnCollisionLeave(result);
	CGameState::Active()->GetBehavior( ((hkpRigidBody*)(otherColl->getOwner()))->getUserData() )->OnCollisionLeave(result);*/
}






























/*

        ( ^ )
		|   |
		| p |
		| e |
		| n |
		| i |
   _	| s |    _
  / -\_ |   | _/- \
 | ba  -/    \- lz |
  \ .           . /

*/