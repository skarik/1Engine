#ifndef CORE_EXT_SYSTEM_IO_STD_FILESYSTEM_H_
#define CORE_EXT_SYSTEM_IO_STD_FILESYSTEM_H_

#include <filesystem>

#ifdef _MSC_VER
#	if _MSC_VER<=1900 // VS2015
namespace fs = std::tr2::sys;
#	elif _MSC_VER<=19016 // VS2015
namespace fs = std::experimental::filesystem;
#	else // VS2019+
namespace fs = std::filesystem;
#	endif
#else
namespace fs = std::filesystem;
#endif

#endif//CORE_EXT_SYSTEM_IO_STD_FILESYSTEM_H_