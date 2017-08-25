
#include "CVoxelTerrain.h"

// Include Generator
#include "CTerrainGenerator.h"

// Include all the component types
#include "CTerrainProp.h"
#include "CTerrainPropFactory.h"
/*
#include "ElectricLampComponent.h"
#include "ElectricSourceComponent.h"*/

#include <string>
using std::string;
using std::cout;
using std::endl;

void CVoxelTerrain::TerraGen_NewComponent ( CBoob * pBoob, const BlockInfo& block, const Vector3d& newPos, const string& sTypeName )
{
	int iGenerationMode = generator->GetGenerationMode();
	if (( iGenerationMode == CTerrainGenerator::GENERATE_TO_DISK )||( iGenerationMode == CTerrainGenerator::GENERATE_DUAL_SAVE ))
	{
		CMetaboob::sComponentIO newComponentIORequest;
		//newFoliageIORequest.foliage_index = GetFoliageType( sTypeName );
		unordered_map<string,ushort>::iterator foundPoint;
		foundPoint = TerrainProp::TerrainPropFactory.component_swap.find( sTypeName );
		if ( foundPoint != TerrainProp::TerrainPropFactory.component_swap.end() ) {
			newComponentIORequest.component_index = (*foundPoint).second;
			newComponentIORequest.position	= newPos;
			newComponentIORequest.rotation	= Vector3d(0,0,0);
			newComponentIORequest.scaling	= Vector3d(1,1,1);
			newComponentIORequest.data		= 0;
			newComponentIORequest.b16index	= block.b16index;
			newComponentIORequest.b8index	= block.b8index;
			newComponentIORequest.bindex	= block.b1index;
			((CMetaboob*)pBoob)->v_componentIO.push_back( newComponentIORequest );
		}
		else {
			cout << "BAD COMPONENT REQUEST IN GENERATION of type: " << sTypeName << endl;
		}
	}
}

int	CVoxelTerrain::TerraGen_GetComponentCount ( CBoob * pBoob, const string& sTypeName )
{
	int iGenerationMode = generator->GetGenerationMode();
	if (( iGenerationMode == CTerrainGenerator::GENERATE_TO_DISK )||( iGenerationMode == CTerrainGenerator::GENERATE_DUAL_SAVE ))
	{
		CMetaboob::sComponentIO newComponentIORequest;
		//newFoliageIORequest.foliage_index = GetFoliageType( sTypeName );
		unordered_map<string,ushort>::iterator foundPoint;
		foundPoint = TerrainProp::TerrainPropFactory.component_swap.find( sTypeName );
		if ( foundPoint != TerrainProp::TerrainPropFactory.component_swap.end() ) {
			//((CMetaboob*)pBoob)->v_componentIO.cou
			//vector<CMetaboob::sComponentIO>::iterator itr = ((CMetaboob*)pBoob)->v_componentIO.begin();
			int count = 0;
			uint maxcount = ((CMetaboob*)pBoob)->v_componentIO.size();
			//while ( itr != ((CMetaboob*)pBoob)->v_componentIO.end() )
			for ( uint i = 0; i < maxcount; ++i )
			{
				//if ( itr->component_index == foundPoint->second ) {
				if ( ((CMetaboob*)pBoob)->v_componentIO[i].component_index == foundPoint->second ) {
					count += 1;
				}
			}
			return count;
		}
	}
	return 0;
}

unsigned short CVoxelTerrain::GetComponentType( CTerrainProp *pComponent )
{
	/*string tname = pComponent->GetTypeName();

	if ( tname == "CTerrainProp" )
		return 0;
	else if ( tname == "ElectricLampComponent" )
		return 1;
	else if ( tname == "ElectricSourceComponent" )
		return 2;*/

	ushort result = TerrainProp::GetId( pComponent );
	if ( result ) {
		return result;
	}

	string tname = pComponent->GetTypeName();
	cout << "Warning: Given TerrainProp '" << tname << "' has no valid Component type." << endl;

	return (unsigned short)(-1);
}

CTerrainProp* CVoxelTerrain::CreateComponent( 
		unsigned short iType,
		const Vector3d &vInPos, const Vector3d &vInRot, const Vector3d &vInScale,
		const uint64_t iInData,
		CBoob* pBoob, char const b16_index, char const b8_index, short const b1_index )
{
	CTerrainProp* result = NULL;

	BlockInfo newBlockInfo;
	newBlockInfo.pBoob	= pBoob;
	newBlockInfo.b16index	= b16_index;
	newBlockInfo.b8index	= b8_index;
	newBlockInfo.b1index	= b1_index;
	newBlockInfo.pBlock16	= &(pBoob->data[b16_index]);
	newBlockInfo.pBlock8	= &(newBlockInfo.pBlock16->data[b8_index]);
	newBlockInfo.pBlock		= &(newBlockInfo.pBlock8->data[b1_index]);
	newBlockInfo.block		= *(newBlockInfo.pBlock);

	/*if ( iType == 0 )
		cout << "Invalid component type " << iType << " with dat " << iInData << endl;
	else if ( iType == 1 )
	{
		result = new ElectricLampComponent ( newBlockInfo );
		result->transform.position = vInPos;
		result->transform.rotation = vInRot;
		result->transform.scale = vInScale;
	}
	else if ( iType == 2 )
	{
		result = new ElectricSourceComponent ( newBlockInfo );
		result->transform.position = vInPos;
		result->transform.rotation = vInRot;
		result->transform.scale = vInScale;
	}*/
	//result = TerrainProp::Instantiate( iType, newBlockInfo ); // KEY LINE
	if ( result ) {
		result->transform.position = vInPos;
		result->transform.rotation = vInRot;
		result->transform.scale = vInScale;
		result->OnTerrainPlacement();
		result->SetUserdata( iInData );
	}
	else {
		cout << "ERROR COULN'T LOAD SHIT" << endl;
	}


	return result;
}

void CVoxelTerrain::CreateQueuedComponents ( void )
{
	while ( !vComponentQueue.empty() )
	{
		TerraProp currentRequest = vComponentQueue.back();
		vComponentQueue.pop_back();

		// First need to check if boob exists, I think
		if ( BoobExists( currentRequest.m_block.pBoob ) )
		{
			//CComponent* newFoliage = CreateFoliage( currentRequest.foliage_index, currentRequest.position, currentRequest.userdata );
			//newFoliage->pBoob = currentRequest.myBoob;
			//currentRequest.myBoob->v_foliage.push_back( newFoliage );
			CreateComponent( 
				currentRequest.component_index,
				currentRequest.position, currentRequest.rotation, currentRequest.scaling,
				currentRequest.userdata,
				currentRequest.m_block.pBoob, currentRequest.m_block.b16index, currentRequest.m_block.b8index, currentRequest.m_block.b1index
				);
		}
	}
}