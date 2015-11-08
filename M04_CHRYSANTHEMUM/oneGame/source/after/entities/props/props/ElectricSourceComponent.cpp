
#include "ElectricSourceComponent.h"
#include "after/entities/item/props/ItemElectricSource.h"
#include "after/terrain/VoxelTerrain.h"
#include "renderer/material/glMaterial.h"

ElectricSourceComponent::ElectricSourceComponent ( BlockTrackInfo const& inInfo )
	: CTerrainProp( inInfo )
{
	glMaterial* newMat = new glMaterial();
	newMat->m_diffuse = Color( 0.6f,0.3f,0.3f );
	newMat->removeReference();

	m_model = new CModel( string(".res\\models\\geosphere.FBX") );
	m_model->transform.scale = Vector3d( 0.5f,0.5f,0.5f );
	m_model->SetMaterial( newMat );
}


void ElectricSourceComponent::Update ( void )
{
	// Shiz
}

// Lookats create some outlines
void	ElectricSourceComponent::OnInteractLookAt	( CActor* interactingActor )
{
	if ( interactingActor->GetTypeName() == "CPlayer" )
	{
		//pModel->renderSettings.cOutlineColor = Color( 1.0f,1,1 );
		//pModel->renderSettings.fOutlineWidth = 0.08f;
	}
}
void	ElectricSourceComponent::OnInteractLookAway	( CActor* interactingActor )
{
	if ( interactingActor->GetTypeName() == "CPlayer" )
	{
		//pModel->renderSettings.fOutlineWidth = -1.0f;
	}
}


void ElectricSourceComponent::PreSimulation ( void )
{
	// This is the code for a power source
	//SetWireBit( mBlockInfo.pBlock8->data[mBlockInfo.b1index], 0 );
///	mBlockInfo.pBlock8->data[mBlockInfo.b1index].wire = 0;
//	mBlockInfo.block->wire = 0;
	//SetPowerBit( mBlockInfo.pBlock8->data[mBlockInfo.b1index], 1 );
///	mBlockInfo.pBlock8->data[mBlockInfo.b1index].power = 1;
//	mBlockInfo.block->power = 1;
	//if ( GetSource( (uint32_t)(mBlockInfo.pBlock8->temp[mBlockInfo.b1index]) ) )
	//if ( GetPowerBit( (uint32_t)(mBlockInfo.pBlock8->temp[mBlockInfo.b1index]) ) )
	//	cout << "Presim" << endl;

	mBlockInfo.pBlock->wire = 0;
	mBlockInfo.pBlock->voltage = 3;
}


void ElectricSourceComponent::PostSimulation ( void )
{
	// And I don't think anything goes here.
}

// On Punched. Is executed when an ItemBlockPuncher or derived hits this object.
void ElectricSourceComponent::OnPunched ( RaycastHit const& hitInfo )
{
	ItemElectricSource* pNewItem = new ItemElectricSource();
	pNewItem->transform.Get( transform );

	DeleteObject( this );
}