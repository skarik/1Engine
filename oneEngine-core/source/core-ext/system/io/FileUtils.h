
#ifndef _IO_FILE_UTILS_EXT_H_
#define _IO_FILE_UTILS_EXT_H_

#include <string>

#include "core/types/types.h"
#include "core/system/io/FileUtils.h"

namespace IO
{
	// File checking
	CORE_API bool ModelExists ( const std::string& n_filename );
}

#endif//_IO_FILE_UTILS_EXT_H_