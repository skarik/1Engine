#ifndef PHYSICAL_PHYSICS_MOTION_I_PR_VOLUME_H_
#define PHYSICAL_PHYSICS_MOTION_I_PR_VOLUME_H_

#include "core/types/types.h"

class PrWorld;

class IPrVolume
{
public:
	PHYS_API explicit	IPrVolume ( PrWorld* world );
	PHYS_API virtual	~IPrVolume ( void );
protected:
	PrWorld*	m_world;
};

#endif//PHYSICAL_PHYSICS_MOTION_I_PR_VOLUME_H_