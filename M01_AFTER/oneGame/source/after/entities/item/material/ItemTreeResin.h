#ifndef _ITEM_TREE_RESIN_H_
#define _ITEM_TREE_RESIN_H_

#include "after/entities/item/CWeaponItem.h"
#include "renderer/logic/model/CModel.h"

class ItemTreeResin : public CWeaponItem
{
	ClassName( "ItemTreeResin" );
protected:
	WITEM_DATA_DEFINE
	{ WITEMDATA_DEFAULTS;

		wpdata.iItemId = 6;

		wpdata.sInfo = "Resin";
		wpdata.bCanStack = true;
		wpdata.iMaxStack = 50;
		wpdata.fWeight = 1.0f;

		return wpdata;
	}
public:
	ItemTreeResin::ItemTreeResin ( void )
		: CWeaponItem( ItemData() )
	{
		pModel = new CModel( string(".res/models/items/tree_resin.FBX") );
		pModel->transform.scale = Vector3d( 0.7f,0.7f,0.7f );
		glMaterial* sapMaterial;
		sapMaterial = new glMaterial;
		sapMaterial->m_emissive = Color( 0,0,0.0f );
		sapMaterial->m_diffuse = Color( 1,1,1.0f );
		sapMaterial->setTexture( 0, new CTexture("null") );
		sapMaterial->passinfo.push_back( glPass() );
		sapMaterial->passinfo[0].shader = new glShader( ".res/shaders/e/blob.glsl" );
		sapMaterial->removeReference();
		pModel->SetMaterial( sapMaterial );
	}

	bool ItemTreeResin::Use ( int x )
	{
		return false;
	}

private:
	
};

#endif