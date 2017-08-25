
#include "CVoxelTerrain.h"

// Include all the component types
#include "CTerraComponent.h"

#include "ElectricLampComponent.h"
#include "ElectricSourceComponent.h"

#include <string>
using std::string;

unsigned short CVoxelTerrain::GetComponentType( CTerraComponent *pComponent )
{
	string tname = pComponent->GetTypeName();

	if ( tname == "CTerraComponent" )
		return 0;
	else if ( tname == "ElectricLampComponent" )
		return 1;
	else if ( tname == "ElectricSourceComponent" )
		return 2;

	cout << "Warning: Given TerraComponent '" << tname << "' has no valid Component type." << endl;

	return (unsigned short)(-1);
}

CTerraComponent* CVoxelTerrain::CreateComponent( 
		unsigned short iType,
		const Vector3d &vInPos, const Vector3d &vInRot, const Vector3d &vInScale,
		const uint64_t iInData,
		CBoob* pBoob, char const b16_index, char const b8_index, short const b1_index )
{
	CTerraComponent* result = NULL;

	BlockInfo newBlockInfo;
	newBlockInfo.pBoob	= pBoob;
	newBlockInfo.b16index	= b16_index;
	newBlockInfo.b8index	= b8_index;
	newBlockInfo.b1index	= b1_index;
	newBlockInfo.pBlock16	= &(pBoob->data[b16_index]);
	newBlockInfo.pBlock8	= &(newBlockInfo.pBlock16->data[b8_index]);
	newBlockInfo.pBlock		= &(newBlockInfo.pBlock8->data[b1_index]);
	newBlockInfo.block		= *(newBlockInfo.pBlock);

	if ( iType == 0 )
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
	}
	/*else if ( iType == 1 )
	{
		result = new CTreeBase();
		result->transform.position = vInPos;
		((CTreeBase*)(result))->Generate();
	}*/

	return result;
}