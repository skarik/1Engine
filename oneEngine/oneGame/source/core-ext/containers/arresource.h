//===============================================================================================//
//
//		arresource
//
// Templated resource class for wrapping a type. Stores time of last access.
// Used by the resource manager system
//
//===============================================================================================//
#ifndef CORE_CONTAINER_AR_RESOURCE_H_
#define CORE_CONTAINER_AR_RESOURCE_H_

#include "core/types/float.h"
#include "core/time/Time.h"

//	class arresource
// Wraps a resource.
template <typename Type>
class arresource
{
public:
	Type	m_value;
	Real	m_time_last_accessed;
};

#endif//CORE_CONTAINER_AR_RESOURCE_H_