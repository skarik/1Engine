#include "PrCast.h"

PrCast::~PrCast ( void )
{
	delete[] m_hits;
	m_hits = NULL;

	m_hits_count = 0;
}


bool PrCast::Hit ( void ) const
{
	return m_hits_count > 0;
}
