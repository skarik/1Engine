
#ifndef _PHYS_TRIGGER_VOLUME_H_
#define _PHYS_TRIGGER_VOLUME_H_

#include "physRigidbody.h"

#define _FORCE_INLINE_

class physTriggerVolume : public physRigidBody
{
public:
	_FORCE_INLINE_ PHYS_API explicit		physTriggerVolume ( physRigidBodyInfo*, physShape*, physPhantomCallbackShape* );

};

#endif//_PHYS_TRIGGER_VOLUME_H_