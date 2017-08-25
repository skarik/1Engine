
#ifndef _TOWN_GEN_A0_H_
#define _TOWN_GEN_A0_H_

#include "CTownGenerationInterface.h"

class TownGen_A0 : public CTownGenerationInterface
{

public:
	void Generate ( void ) override;

private:
	// == Generation routines ==
	//	GenerateRoads
	// First comes up with road traits. After traits are randomly chosen, will generate road map.
	void				GenerateRoads ( void );
	//	GenerateLocations
	// Generates instances and big one-shot areas in towns that aren't buildings, such as open markets or wells.
	void				GenerateLocations ( void );
	//	GenerateBuildings
	// Places buildings, and generates rooms and layouts based on target building sizes. Will destroy certain roads.
	void				GenerateBuildings ( void );
};

#endif//_TOWN_GEN_A0_H_