
#ifndef _IO_FILE_UTILS_H_
#define _IO_FILE_UTILS_H_

#include <string>

namespace IO
{
	// File check
	bool FileExists ( const char* n_filename );
	bool FileExists ( const std::string& n_filename );

	// Name edit
	std::string FilenameStandardize ( const std::string& n_filename );

	// File edit
	bool ClearFile ( const char* n_filename );

	// Append edit files
	bool AppendFile ( const char* n_targetfile, const char* n_sourcefile );
	bool AppendStringToFile ( const char* n_targetfile, const char* n_sourcestring );

	// File reading
	bool ReadLine ( FILE* n_file, char* n_buffer );

	// Big fat tiles
	std::string ReadFileToString ( const char* n_filename );
};

#endif//_IO_FILE_UTILS_H_