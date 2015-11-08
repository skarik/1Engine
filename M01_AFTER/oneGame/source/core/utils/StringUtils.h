
#ifndef _STRING_HELPER_H_
#define	_STRING_HELPER_H_

#include <vector>
#include <algorithm>
#include <string>

namespace StringUtils
{
		using std::string;

	string					GetFileExtension( const string& FileName );
	string					ToLower( const string& input );

	string					TrimRight( const string &t, const string &ws = "\0\t\f\v\n\r " );
	string					TrimLeft ( const string &t, const string &ws = "\0\t\f\v\n\r " );

	string					FullTrim ( const string &t );

	std::vector<string>		Split  (const string& s, const string& delim, const bool keep_empty = true );


	template<class object>
	object					ToObject ( const char* t );

};

#endif