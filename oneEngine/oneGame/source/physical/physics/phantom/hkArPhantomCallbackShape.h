
#ifndef _HK_AR_CPHANTOM_CALLBACK_SHAPE_
#define _HK_AR_CPHANTOM_CALLBACK_SHAPE_

#include "physical/physics/CPhysicsCommon.h"

class hkArCallbackDistributor
{
public:
	virtual void phantomEnterEvent ( const physCollidable *phantomColl, const physCollidable *otherColl, const physCollisionInput &env ) =0;
	virtual void phantomLeaveEvent ( const physCollidable *phantomColl, const physCollidable *otherColl ) =0;
};

class hkArPhantomCallbackShape : public physPhantomCallbackShape
{
public:
	void phantomEnterEvent ( const physCollidable *phantomColl, const physCollidable *otherColl, const physCollisionInput &env );
	void phantomLeaveEvent ( const physCollidable *phantomColl, const physCollidable *otherColl );

	hkArCallbackDistributor*	distributor;
};

#endif