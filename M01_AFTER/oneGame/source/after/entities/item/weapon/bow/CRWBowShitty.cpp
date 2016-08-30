
#include "CRWBowShitty.h"
#include "core-ext/animation/CAnimation.h"
#include "renderer/logic/model/CSkinnedModel.h"

CRWBowShitty::CRWBowShitty ( void )
	: CBaseRandomBow( tBowWeaponProperties(), ItemData() )
{
	holdType = Item::Bow;

	vHoldingOffset = Vector3d( -0.18f,0,0 );
}
CRWBowShitty::CRWBowShitty ( tBowWeaponProperties& inProps )
	: CBaseRandomBow( inProps, ItemData() )
{
	holdType = Item::Bow;
	
	vHoldingOffset = Vector3d( -0.18f,0,0 );
}

CRWBowShitty::~CRWBowShitty ( void )
{
	;
}

void CRWBowShitty::Generate ( void )
{
	CBaseRandomItem::Generate();

	string bowModel = "models/weapons/bow_beta.FBX";

	//pModel = new CSkinnedModel( bowModel );
	pModel = new CModel( bowModel.c_str() );
	/*if (pModel->GetAnimation()) {
		pModel->GetAnimation()->Play("ref");
	}*/
}