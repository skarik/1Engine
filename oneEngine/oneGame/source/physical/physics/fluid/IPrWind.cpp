#include "IPrWind.h"

IPrWind::IPrWind(void)
	: m_active(true)
{
	;
}

IPrWind::~IPrWind(void)
{
	m_active = false;
}
