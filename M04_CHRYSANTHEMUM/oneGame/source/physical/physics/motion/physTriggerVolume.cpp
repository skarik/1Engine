
#include "physTriggerVolume.h"

_FORCE_INLINE_ PHYS_API physTriggerVolume::physTriggerVolume ( physRigidBodyInfo* info, physShape* shape, hkpPhantomCallbackShape* phantom )
	: physRigidBody()
{
	body = Physics::CreateTriggerVolume( info, shape, phantom );
}
