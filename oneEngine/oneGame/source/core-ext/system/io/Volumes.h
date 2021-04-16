//===============================================================================================//
//
//		core-ext/system/io/Volumes.h
//
// Contains OS-specific features dealing with logical disk volumes
//
//===============================================================================================//
#ifndef CORE_EXT_SYSTEM_IO_VOLUMES_H_
#define CORE_EXT_SYSTEM_IO_VOLUMES_H_

#include <vector>
#include <string>

#include "core/types/types.h"

namespace core
{
	namespace io
	{
		//	EnumerateVolumes() : Returns a list of all the volumes connected to the system.
		CORE_API void EnumerateVolumes ( std::vector<std::string>& outVolumeList );
	}
}

#endif//CORE_EXT_SYSTEM_IO_VOLUMES_H_