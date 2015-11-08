
#include "AmmoCrossbowBolt.h"

#include "renderer/logic/model/CModel.h"

AmmoCrossbowBolt::AmmoCrossbowBolt ( void )
	: CAmmoBase( ItemData() )
{
	pModel = new CModel( string(".res\\models\\weapons\\cb_bolt_beta.FBX") );
}

bool AmmoCrossbowBolt::Use ( int x )
{
	return false;
}