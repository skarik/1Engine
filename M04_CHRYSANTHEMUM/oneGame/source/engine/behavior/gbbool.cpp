
#include "gbbool.h"
#include "CGameBehavior.h"

gbbool& gbbool::operator= ( bool const &rhs )
{
	if ( rhs != value )
	{
		value = rhs;
		if ( value )
		{
			target->OnEnable();
		}
		else
		{
			target->OnDisable();
		}
	}
	return *this;
}