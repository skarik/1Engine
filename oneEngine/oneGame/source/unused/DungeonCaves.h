// class DungeonCaves
// 
// Cavern system, by Joshua Boren
// 
//

#ifndef _DUNGEONS_CAVES_H_
#define _DUNGEONS_CAVES_H_

#include "CDungeonBase.h"

class DungeonCaves : public CDungeonBase
{
public:
	explicit		DungeonCaves ( CTerrainGenerator*gen ) : CDungeonBase(gen) { ; }
			//		~DungeonCaves ( void );

	// Initialize
	//  When called, generates values for the generation table. The values are then
	//  used to determine if the dungeon can be used. If the values aren't good enough,
	//  the dungeon is discarded.
	//  Takes a seed position as an argument.
	virtual void	Initialize ( const RangeVector & );
	// Generate
	//  Runs the dungeon's generation algorithm. Hallways and rooms are created with
	//  this function.
	virtual void	Generate ( void );

private:
	void AddSegment ( const unsigned int iHallway, const unsigned int seed );
};


#endif//_DUNGEONS_CAVES_H_