
#include "CDungeonFactory.h"

Dungeon::CDungeonFactory Dungeon::Factory;


#include "CDungeonBase.h"
#include "CDungeonLua.h"
#include "DungeonCaves.h"

void Dungeon::CDungeonFactory::RegisterDungeons ( void )
{
	//Register( CDungeonBase, 0 );
	RegisterDungeon( CDungeonLua, 1 );

	//RegisterDungeon( DungeonCaves, 2 );
}