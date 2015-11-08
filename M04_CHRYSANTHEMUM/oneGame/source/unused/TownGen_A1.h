
#ifndef _TOWN_GEN_A1_H_
#define _TOWN_GEN_A1_H_

#include "CTownGenerationInterface.h"

class TownGen_A1 : public CTownGenerationInterface
{

public:
	void Generate ( void ) override;

private:
	Vector3d	townCenterPosition;
	int			mseed;
private:
	// == Generation routines ==
	//	GenerateRoads
	// First comes up with road traits. After traits are randomly chosen, will generate road map.
	/*void				GenerateRoads ( void );
	//	GenerateLocations
	// Generates instances and big one-shot areas in towns that aren't buildings, such as open markets or wells.
	void				GenerateLocations ( void );
	//	GenerateBuildings
	// Places buildings, and generates rooms and layouts based on target building sizes. Will destroy certain roads.
	void				GenerateBuildings ( void );*/


	void				GenerateBuildings_Positions ( void );
	void				GenerateBuildings_Floorplans ( void );

	void				CalculateTownBoundingBox ( void );


	void				GenerateRoad_BuildingEntrances ( void );
	void				GenerateRoad_ConnectMajor ( void );
	void				GenerateRoad_MainStreets ( void );

	void				GenerateRoad_FullSequence ( void );

	// Utility function for random chance based on noise
	//bool				Chance ( const ftype chance, const int seed );
	// Gets a random number from -0.5 to 0.5
	//ftype				RandomNumber ( void );

	// Function to select a position for a building and check for room
	void				AddBuilding ( const World::eBuildingType buildingType );

};

#endif//_TOWN_GEN_A1_H_