
#include "core/types/types.h"
#include "Wind.h"

CBaseWindTester* CBaseWindTester::Active = NULL;
CBaseWindTester* CBaseWindTester::Get ( void )
{
	return Active;
}
CBaseWindTester::CBaseWindTester ( void )
{
	Active = this;
}