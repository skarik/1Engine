#include "Files.h"
#include "core/os.h"
#include "core/debug.h"
#include "core-ext/std/filesystem.h"

uint64_t core::io::file::GetSize ( const char* filename )
{
	std::error_code error;
	return (uint64_t)fs::file_size(filename, error);
}
int64_t core::io::file::GetLastWriteTime ( const char* filename )
{
	std::error_code error;
	auto timestamp = fs::last_write_time(filename, error);

	return timestamp.time_since_epoch().count();
}