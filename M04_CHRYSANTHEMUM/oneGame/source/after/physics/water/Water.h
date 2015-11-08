
#ifndef _AFTER_WATER_CLASS_H_
#define _AFTER_WATER_CLASS_H_

//#include "engine-common/physics/water/Water.h"
#include "physical/physics/water/Water.h"

class CAfterWaterTester : public CBaseWaterTester
{
public:
	// Returns true if the position is in fluid
	virtual bool		PositionInside	( Vector3d const& pos ) override;
	// Returns the fluid velocity at the given position
	virtual Vector3d	GetFlowField	( Vector3d const& pos ) override;

	//== Sea and Storm Compatibility==
	// Grabs the height of the ocean at the current XY position.
	virtual float		OceanHeight ( Vector3d const& pos ) override;
};

#endif//_AFTER_WATER_CLASS_H_