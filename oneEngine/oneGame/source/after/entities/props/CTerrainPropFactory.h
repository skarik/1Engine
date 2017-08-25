
#ifndef _C_TERRA_COMPONENT_FACTORY_H_
#define _C_TERRA_COMPONENT_FACTORY_H_

class CTerrainProp;
class CVoxelTerrain;
struct BlockTrackInfo;

#include "core/common.h"
#include "core/types/types.h"

#include <typeinfo>
#include <string>
using std::type_info;
using std::string;

#include <unordered_map>

namespace Terrain
{
	template<typename T> CTerrainProp * _instComponent( BlockTrackInfo const& inInfo ) { return new T(inInfo); }

	class CTerrainPropFactory
	{
	public:
		std::unordered_map<string,ushort>	component_hash;
		std::unordered_map<ushort,string>	component_list;
		std::unordered_map<ushort,string>	component_name;
		std::unordered_map<string,ushort>	component_swap;
		std::unordered_map<ushort,CTerrainProp*(*)(BlockTrackInfo const&)>	component_inst;

	private:
		friend CVoxelTerrain;
		void RegisterTypes ( void );

	#define RegisterComponent(A,ID) \
		component_hash[typeid(A).name()] = ID ; \
		component_list[ ID ] = typeid(A).name(); \
		component_name[ ID ] = #A; \
		component_swap[#A] = ID ; \
		component_inst[ ID ] = &_instComponent<A>;

	public:
		CTerrainPropFactory ( void )
		{
			RegisterTypes();
		}
		~CTerrainPropFactory ( void )
		{
			
		}

	public:
		// Static factions
		static CTerrainProp* Instantiate ( ushort componentId, BlockTrackInfo const& inInfo );
		static ushort	GetId( CTerrainProp* );
		static unsigned short GetComponentType( const string& );
		static unsigned short GetComponentType( CTerrainProp* );
	};
	// Global prop factory
	extern CTerrainPropFactory PropFactory;
}

#endif//_C_TERRA_COMPONENT_FACTORY_H_