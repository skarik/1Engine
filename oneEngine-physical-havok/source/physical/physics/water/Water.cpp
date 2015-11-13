
#include "core/types/types.h"
#include "Water.h"

CBaseWaterTester* CBaseWaterTester::Active = NULL;
CBaseWaterTester* CBaseWaterTester::Get ( void )
{
	return Active;
}
CBaseWaterTester::CBaseWaterTester ( void )
{
	Active = this;
}