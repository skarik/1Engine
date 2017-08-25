#ifndef PHYSICAL_PHYSICS_FLUID_I_PR_WATER_VOLUME_H_
#define PHYSICAL_PHYSICS_FLUID_I_PR_WATER_VOLUME_H_

#include "core/types/types.h"
#include "core/math/Vector3d.h"
#include "physical/physics/motion/IPrVolume.h"

class IPrWaterVolume : public IPrVolume
{
public:
	PHYS_API explicit		IPrWaterVolume ( PrWorld* world ) : IPrVolume(world) {}
	PHYS_API virtual		~IPrWaterVolume ( void ) {}
};

#endif//PHYSICAL_PHYSICS_FLUID_I_PR_WATER_VOLUME_H_