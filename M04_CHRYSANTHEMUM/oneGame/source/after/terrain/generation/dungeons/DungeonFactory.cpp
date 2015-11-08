
#include "DungeonFactory.h"

Terrain::Dungeon::CFactory Terrain::Dungeon::Factory;

#include "DungeonBase.h"
#include "DungeonLua.h"
#include "DungeonCaveSystem.h"

void Terrain::Dungeon::CFactory::RegisterDungeons ( void )
{
	//Register( CDungeonBase, 0 );
	RegisterDungeon( DungeonLua, 1 );

	RegisterDungeon( DungeonCaveSystem, 2 );
}