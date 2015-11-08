
#include "ItemFloraLifedrop.h"

#include "renderer/logic/model/CModel.h"

ItemFloraLifedrop::ItemFloraLifedrop ( void )
	: CWeaponItem( ItemData() )
{
	pModel = new CModel ( string(".res/models/items/flora_lifedrop.FBX") );

	holdType = Item::Default;
}

ItemFloraLifedrop::~ItemFloraLifedrop ( void )
{
	;
}
