
#ifndef _HK_AR_DEFAULT_DISTRIBUTOR_CALLBACKS_H_
#define _HK_AR_DEFAULT_DISTRIBUTOR_CALLBACKS_H_

#include "physical/physics/phantom/hkArPhantomCallbackShape.h"

class hkArDefaultDistributor : public hkArCallbackDistributor
{
public:
	void phantomEnterEvent ( const hkpCollidable *phantomColl, const hkpCollidable *otherColl, const hkpCollisionInput &env ) override;
	void phantomLeaveEvent ( const hkpCollidable *phantomColl, const hkpCollidable *otherColl ) override;
};

#endif//_HK_AR_DEFAULT_DISTRIBUTOR_CALLBACKS_H_
