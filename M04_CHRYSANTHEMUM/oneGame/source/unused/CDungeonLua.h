
#ifndef _C_DUNGEON_LUA_H_
#define _C_DUNGEON_LUA_H_

#include "CDungeonBase.h"
#include "CLuaController.h"

class CDungeonLua : public CDungeonBase
{

public:
	explicit		CDungeonLua ( CTerrainGenerator*gen ) : CDungeonBase(gen) { ; }
	// Initialize
	//  Calls corresponding lua function that returns a table. Table then sets the
	//  current dungeon values. If the values aren't good enough, the dungeon is
	//  discarded.
	void			Initialize ( const RangeVector & seedposition ) override { exit(13); };
	
	void			Generate ( void ) override { exit(13); };

	// Or is there a better idea?

	// Yes there is, Generate is called, which returns an n size array of rooms and halls.
	


private:
	ftype			GetTerrainElevation ( void );
	short			GetTerrainBiome ( void );

};

#endif//_C_DUNGEON_LUA_H_