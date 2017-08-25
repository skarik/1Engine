
#ifndef _HK_AR_DEFAULT_DISTRIBUTOR_CALLBACKS_H_
#define _HK_AR_DEFAULT_DISTRIBUTOR_CALLBACKS_H_

#include "physical/physics/phantom/hkArPhantomCallbackShape.h"

class hkArDefaultDistributor : public hkArCallbackDistributor
{
public:
	void phantomEnterEvent ( const physCollidable *phantomColl, const physCollidable *otherColl, const physCollisionInput &env ) override;
	void phantomLeaveEvent ( const physCollidable *phantomColl, const physCollidable *otherColl ) override;
};

#endif//_HK_AR_DEFAULT_DISTRIBUTOR_CALLBACKS_H_
