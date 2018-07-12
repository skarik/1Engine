#ifndef CORE_IO_FILE_UTILS_H_
#define CORE_IO_FILE_UTILS_H_

#include "core/types.h"
#include "core/common.h"
#include <string>

namespace IO
{
	// File check
	bool FileExists ( const char* n_filename );
	bool FileExists ( const std::string& n_filename );

	// Name edit.
	//DEPRECATED("Use core::utils::string::ToPathStandard instead")
	//std::string FilenameStandardize ( const std::string& n_filename );

	// File edit
	bool ClearFile ( const char* n_filename );

	// Append edit files
	bool AppendFile ( const char* n_targetfile, const char* n_sourcefile );
	bool AppendStringToFile ( const char* n_targetfile, const char* n_sourcestring );

	// File reading
	bool ReadLine ( FILE* n_file, char* n_buffer );

	// Big fat tiles
	std::string ReadFileToString ( const char* n_filename );

	// Copy files
	bool CopyFileTo ( const char* n_destination, const char* n_source );

	// Remove files
	bool RemoveFile ( const char* n_filename );
};

#endif//CORE_IO_FILE_UTILS_H_