
#ifndef _STRING_HELPER_H_
#define	_STRING_HELPER_H_

#include "core/types/types.h"

#include <vector>
#include <algorithm>
#include <string>

namespace StringUtils
{
	using std::string;

	//	GetFileStemLeaf ( filename )
	// Returns the file without the extension. Not gaurenteed to work with a missing extension.
	string					GetFileStemLeaf( const string& filename );
	//	GetFileExtension ( filename )
	// Returns the file's last extension.
	string					GetFileExtension( const string& filename );

	string					ToLower( const string& input );

	string					TrimRight( const string &t, const string &ws = "\0\t\f\v\n\r " );
	string					TrimLeft ( const string &t, const string &ws = "\0\t\f\v\n\r " );

	string					FullTrim ( const string &t );

	std::vector<string>		Split  (const string& s, const string& delim, const bool keep_empty = true );

	size_t					LargestCommonSubstringLength ( const string& str1, const string& str2 );

	template<class object>
	object					ToObject ( const char* t );

};

#endif