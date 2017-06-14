
#include "AmmoArrow.h"

#include "renderer/logic/model/CModel.h"

AmmoArrow::AmmoArrow ( void )
	: CAmmoBase( ItemData() )
{
	pModel = new CModel( "models/weapons/cb_arrow_beta.FBX" );
}

bool AmmoArrow::Use ( int x )
{
	return false;
}