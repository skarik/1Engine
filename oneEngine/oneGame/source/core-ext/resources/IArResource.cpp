#include "IArResource.h"
#include "core/math/hash.h"

//	virtual InitResourceHash() : Recalculates the resource hash.
void IArResource::InitResourceHash ( void )
{
	m_resourceHashValue = math::hash::fnv1a_32(ResourceName());
}