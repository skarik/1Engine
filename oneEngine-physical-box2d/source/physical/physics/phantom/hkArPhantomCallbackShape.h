
#ifndef _HK_AR_CPHANTOM_CALLBACK_SHAPE_
#define _HK_AR_CPHANTOM_CALLBACK_SHAPE_

#include "physical/physics/CPhysicsCommon.h"

class hkArCallbackDistributor
{
public:
	virtual void phantomEnterEvent ( const hkpCollidable *phantomColl, const hkpCollidable *otherColl, const hkpCollisionInput &env ) =0;
	virtual void phantomLeaveEvent ( const hkpCollidable *phantomColl, const hkpCollidable *otherColl ) =0;
};

class hkArPhantomCallbackShape : public hkpPhantomCallbackShape
{
public:
	void phantomEnterEvent ( const hkpCollidable *phantomColl, const hkpCollidable *otherColl, const hkpCollisionInput &env );
	void phantomLeaveEvent ( const hkpCollidable *phantomColl, const hkpCollidable *otherColl );

	hkArCallbackDistributor*	distributor;
};

#endif