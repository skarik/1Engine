

#ifndef _WATER_H_
#define _WATER_H_

// Includes
#include "core/types/types.h"
#include "core/math/Vector3.h"

// Class Definition
class CBaseWaterTester
{
protected:
	static CBaseWaterTester*	Active;
	// Hidden constructor only visible to child classes
	PHYS_API explicit CBaseWaterTester ( void );
public:
	PHYS_API static CBaseWaterTester* Get ( void );

	// Returns true if the position is in fluid
	PHYS_API virtual bool		PositionInside	( Vector3f const& pos ) =0;
	// Returns the fluid velocity at the given position
	PHYS_API virtual Vector3f	GetFlowField	( Vector3f const& pos ) =0;

	//== Sea and Storm Compatibility==
	// Grabs the height of the ocean at the current XY position.
	PHYS_API virtual float		OceanHeight ( Vector3f const& pos ) =0;
};

typedef CBaseWaterTester WaterTester; 


#endif