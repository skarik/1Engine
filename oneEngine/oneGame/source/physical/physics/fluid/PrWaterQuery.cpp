#include "PrWaterQuery.h"

PrWaterQuery::~PrWaterQuery(void)
{
	; // Nothing
}

PrWaterQuery::operator bool() const
{
	return hit;
}
