//===============================================================================================//
//
//		core-ext/system/io/Files.h
//
// Contains OS-specific features dealing with filesystems.
//
//===============================================================================================//
#ifndef CORE_EXT_SYSTEM_IO_FILES_H_
#define CORE_EXT_SYSTEM_IO_FILES_H_

#include <vector>
#include <string>

#include "core/types/types.h"

namespace core
{
	namespace io
	{
		namespace file
		{
			//	GetSize(filename) : Returns size of file in bytes.
			CORE_API uint64_t GetSize ( const char* filename );
			//	GetLastWriteTime(filename) : Returns time since epoch the file was last edited.
			CORE_API int64_t GetLastWriteTime ( const char* filename );
		}
	}
}

#endif//CORE_EXT_SYSTEM_IO_VOLUMES_H_