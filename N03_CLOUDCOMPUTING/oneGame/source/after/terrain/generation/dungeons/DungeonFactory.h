//
//
// class CDungeonFactory
// Controls instantiating dungeons.
//
//

#ifndef _C_TG_DUNGEON_FACTORY_H_
#define _C_TG_DUNGEON_FACTORY_H_

#include "core/common.h"
#include "core/types.h"

#include <unordered_map>
#include <typeinfo>
#include <string>

namespace Terrain
{
	class CWorldGen_Terran;
	class DungeonBase;

	namespace Dungeon
	{
		// Instantiation template prototype
		template<typename T> DungeonBase * _inst( CWorldGen_Terran*gen );
		// Factory class
		class CFactory
		{
		public:
			std::unordered_map<string,short>								dungeon_indexes;
			std::unordered_map<short,DungeonBase*(*)(CWorldGen_Terran*)>	dungeon_insts;

			CFactory ( void )
			{
				RegisterDungeons();
			}
			~CFactory ( void )
			{
				;
			}
		private:
			void RegisterDungeons ( void );

			// Registration macro
#define RegisterDungeon(A,ID) \
	dungeon_indexes[typeid(A).name()] = ID ; \
	dungeon_insts[ ID ] = &_inst<A>;
			// End registration macro
		};

		extern CFactory Factory;

		// Instantiation template
		template<typename T> DungeonBase * _inst( CWorldGen_Terran*gen )
		{
			T* t = new T(gen);
			t->index = Factory.dungeon_indexes[typeid(T).name()];
			return t;
		};
	};
};

#endif//_C_TG_DUNGEON_FACTORY_H_