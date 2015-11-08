
#ifndef _ENGINE_COMMON_MINIMAL_WATER_TESTER_H_
#define _ENGINE_COMMON_MINIMAL_WATER_TESTER_H_

#include "physical/physics/water/Water.h"

class MinimalWaterTester : public CBaseWaterTester
{
public:
	MinimalWaterTester ( void ) 
		: CBaseWaterTester()
	{
		;
	}

	// Returns true if the position is in fluid
	bool		PositionInside	( Vector3d const& pos ) override {
		return false;
	}
	// Returns the fluid velocity at the given position
	Vector3d	GetFlowField	( Vector3d const& pos ) override {
		return Vector3d::zero;
	}

	//== Sea and Storm Compatibility==
	// Grabs the height of the ocean at the current XY position.
	float		OceanHeight ( Vector3d const& pos ) override {
		return 0.0f;
	}

};

#endif//_ENGINE_COMMON_MINIMAL_WATER_TESTER_H_