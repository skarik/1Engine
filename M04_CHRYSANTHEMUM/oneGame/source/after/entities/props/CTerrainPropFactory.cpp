
#include "after/entities/props/CTerrainProp.h"
#include "CTerrainPropFactory.h"
#include "after/terrain/VoxelTerrain.h"

Terrain::CTerrainPropFactory Terrain::PropFactory;

// Include all the component types
#include "after/entities/props/props/ElectricLampComponent.h"
#include "after/entities/props/props/ElectricSourceComponent.h"
#include "after/entities/props/props/PropLantern.h"
#include "after/entities/props/props/PropCampfire.h"
#include "after/entities/props/props/PropTorch.h"

#include "after/entities/props/props/PropChestBase.h"
#include "after/entities/props/props/PropCrateWooden.h"

#include "after/entities/props/props/PropDoorBase.h"

#include "after/entities/props/flora/PropFloraSnowLily.h"
#include "after/entities/props/flora/PropFloraLifedrop.h"

using std::cout;
using std::endl;

void Terrain::CTerrainPropFactory::RegisterTypes ( void )
{
	RegisterComponent( ElectricLampComponent, 1 );
	RegisterComponent( ElectricSourceComponent, 2 );
	RegisterComponent( PropLantern, 3 );
	RegisterComponent( PropCampfire, 4 );
	RegisterComponent( PropTorch, 5 );

	RegisterComponent( PropChestBase, 10 );
	RegisterComponent( PropCrateWooden, 11 );

	RegisterComponent( PropDoorBase, 100 );

	RegisterComponent( PropFloraSnowLily, 500 );
	RegisterComponent( PropFloraLifedrop, 501 );
}


CTerrainProp* Terrain::CTerrainPropFactory::Instantiate ( ushort componentId, BlockTrackInfo const& inInfo )
{
	//CTerrainProp* component = TerrainProp::TerrainPropFactory.component_inst[componentId]( inInfo );
	//return component;
	std::unordered_map<ushort,CTerrainProp*(*)(BlockTrackInfo const&)>::iterator it
		= PropFactory.component_inst.find( componentId );
	if ( it == PropFactory.component_inst.end() ) {
		return NULL;
	}
	else {
		CTerrainProp* component = it->second( inInfo );
		return component;
	}
}

ushort	Terrain::CTerrainPropFactory::GetId( CTerrainProp* component )
{
//	type_info ti = typeid( *component );
//	return TerrainProp::TerrainPropFactory.component_hash[typeid(*component).name()];
	std::unordered_map<string,ushort>::iterator it = PropFactory.component_hash.find( typeid(*component).name() );
	if ( it == PropFactory.component_hash.end() ) {
		cout << "ERROR: COULD NOT LOCATE ID OF OBJECT: " << typeid(*component).name() << endl;
		cout << "    THIS IS A MASSIVE PROBLEM AND SHOULD BE REPORTED IMMEDIATELY" << endl;
		return 0;
	}
	else {
		return it->second;
	}
}


unsigned short Terrain::CTerrainPropFactory::GetComponentType( CTerrainProp* pComponent )
{
	ushort result = GetId( pComponent );
	if ( result ) {
		return result;
	}

	string tname = pComponent->GetTypeName();
	cout << "Warning: Given TerrainProp '" << tname << "' has no valid Component type." << endl;

	return (unsigned short)(-1);
}

unsigned short Terrain::CTerrainPropFactory::GetComponentType( const std::string& componentName )
{
	std::unordered_map<string,ushort>::iterator it = PropFactory.component_swap.find( componentName );
	if ( it == PropFactory.component_swap.end() )
	{
		cout << "ERROR: COULD NOT LOCATE ID OF OBJECT: " << componentName << endl;
		cout << "    THIS IS A MASSIVE PROBLEM AND SHOULD BE REPORTED IMMEDIATELY" << endl;
		return 0;
	}
	else
	{
		return it->second;
	}
}