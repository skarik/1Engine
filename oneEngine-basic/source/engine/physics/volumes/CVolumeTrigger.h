
#ifndef _C_VOLUME_TRIGGER_
#define _C_VOLUME_TRIGGER_

// Include Rigidbody class to inherited from
#include "engine/physics/motion/CRigidBody.h"

class CVolumeTrigger : public CRigidBody
{
public:
	CVolumeTrigger ( CCollider* pTargetCollider, CGameObject * pOwnerGameObject );
	~CVolumeTrigger ( void );

};

#endif