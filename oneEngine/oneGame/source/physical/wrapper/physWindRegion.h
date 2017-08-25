
#ifndef _PHYS_WIND_REGION_H_
#define _PHYS_WIND_REGION_H_

#include "wrapper_common.h"
#include "physVector4.h"
//#include "physMotion.h"

class physWindRegion : public physWorldPostSimulationListener
{
public:
	explicit physWindRegion ( physAabbPhantom* phantom, const physWind* wind, physReal resistanceFactor, physReal obbFactor )
	{
		;
	}
};

#endif//_PHYS_WIND_REGION_H_