//
//
// class CDungeonFactory
// Controls instantiating dungeons.
//
//

#ifndef _C_DUNGEON_FACTORY_H_
#define _C_DUNGEON_FACTORY_H_

class CDungeonBase;
class CTerrainGenerator;

#include "engine_common.h"
#include "standard_types.h"

#include <typeinfo>
#include <string>
using std::type_info;
using std::string;

#if __cplusplus > 199711L
	#include <unordered_map>
	using std::unordered_map;
#else
	#include <unordered_map>
	using std::tr1::unordered_map;
#endif

namespace Dungeon
{
	// Instantiation template prototype
	template<typename T> CDungeonBase * _inst( CTerrainGenerator*gen );
	// Factory class
	class CDungeonFactory
	{
	public:
		unordered_map<string,short>									dungeon_indexes;
		unordered_map<short,CDungeonBase*(*)(CTerrainGenerator*)>	dungeon_insts;

		CDungeonFactory ( void )
		{
			RegisterDungeons();
		}
		~CDungeonFactory ( void )
		{
			;
		}
	private:
		void RegisterDungeons ( void );

#define RegisterDungeon(A,ID) \
	dungeon_indexes[typeid(A).name()] = ID ; \
	dungeon_insts[ ID ] = &_inst<A>;

	};

	extern CDungeonFactory Factory;

	// Instantiation template
	template<typename T> CDungeonBase * _inst( CTerrainGenerator*gen )
	{
		T* t = new T(gen);
		t->index = Factory.dungeon_indexes[typeid(T).name()];
		return t;
	};


};

#endif//_C_DUNGEON_FACTORY_H_