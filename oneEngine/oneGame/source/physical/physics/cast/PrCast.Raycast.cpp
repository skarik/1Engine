#include "PrCast.h"

PrCast::PrCast( const prRaycastQuery& query )
{
	// Perform raycast
	m_hits = NULL;
	m_hits_count = 0;
}