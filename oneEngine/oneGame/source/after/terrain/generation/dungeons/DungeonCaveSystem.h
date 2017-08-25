// class DungeonCaves
// 
// Cavern system, by Joshua Boren
// 
//

#ifndef _TG_DUNGEONS_CAVES_H_
#define _TG_DUNGEONS_CAVES_H_

#include "DungeonBase.h"

namespace Terrain
{
	class DungeonCaveSystem : public DungeonBase
	{
	public:
		explicit		DungeonCaveSystem ( CWorldGen_Terran*gen ) : DungeonBase(gen) { ; }
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
}

#endif//_TG_DUNGEONS_CAVES_H_