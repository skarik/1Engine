//===============================================================================================//
//
//		core/utils/string.h - Common string utilities.
//
//===============================================================================================//
#ifndef CORE_STRING_HELPER_H_
#define	CORE_STRING_HELPER_H_

#include "core/types/types.h"
#include "core/containers/arstring.h"

#include <vector>
#include <algorithm>
#include <string>

namespace core
{
	namespace utils
	{
		namespace string
		{
			static constexpr const char* kWhitespace = "\t\f\v\n\r \0";

			//===============================================================================================//
			// std::string & arstring
			//===============================================================================================//

			//	GetFileStemLeaf(filename) : Returns the file without the extension.
			// Not guaranteed to work with a missing extension.
			static std::string			GetFileStemLeaf( const std::string& filename );
			static arstring256			GetFileStemLeaf( const char* filename, size_t len = 0 );
			//	GetFileExtension(filename) : Returns the file's last extension.
			static std::string			GetFileExtension( const std::string& filename );
			static arstring256			GetFileExtension( const char* filename, size_t len = 0 );
			//	GetPathStandard(filename) : Returns a standardized version of the path.
			static std::string			GetPathStandard( const std::string& filename );
			static arstring256			GetPathStandard( const char* filename, size_t len = 0 );

			//	GetLower(input) : Returns lowercase version of the string. Will work with Unicode.
			static std::string			GetLower( const std::string& input );
			//	GetUpper(input) : Returns uppercase version of the string. Will work with Unicode.
			static std::string			GetUpper( const std::string& input );

			static std::string&			ToLower( std::string& inout );
			static std::string&			ToUpper( std::string& inout );

			//	TrimLeft(t, whitespace) : Returns string with whitespace removed from the right edge (end).
			static std::string			TrimRight( const std::string &t, const std::string &ws = kWhitespace );
			//	TrimLeft(t, whitespace) : Returns string with whitespace removed from the left edge (beginning).
			static std::string			TrimLeft ( const std::string &t, const std::string &ws = kWhitespace );
			//	FullTrim(t) : Returns string with left and right sides both trimmed.
			static std::string			FullTrim ( const std::string &t );

			//	Split(string, delimiters, keepEmpty) : Splits the input string across the given string of delimeters.
			// The delimeters are not included in the result strings.
			// Arguments:
			//	s:			The string to be split.
			//	delim:		List of delimiters to split the string on.
			//	keep_empty:	If empty spaces between delimeters should have an entry in the returned vector. True by default.
			// Returns:
			//	std::vector of all the std::strings that were split.
			static std::vector<std::string>
										Split (const std::string& s, const std::string& delim, const bool keep_empty = true );

			//	LargestCommonSubstringLength(string1, string2) : Counts the length of the largest common substring between the two.
			// Order of the arguments does not matter: "rapped & wrap" will return 3 just as "wrap & rapped" would, the common substring being "rap".
			static size_t				LargestCommonSubstringLength ( const std::string& str1, const std::string& str2 );

			//	CamelCaseToReadable(string) : Makes a human-readable spaced string from a camel-cased string
			static arstring256			CamelCaseToReadable( const char* camelCased, size_t len = 0 );

			//===============================================================================================//
			// C-string
			//===============================================================================================//

			//	ToFileStemLeaf(str, len) : Edits the input filename string to the leaf. The extension's '.' is removed.
			static void					ToFileStemLeaf( char* inout_string, size_t len = 0 );
			//	ToFileExtension(str, len) : Edits the input filename string to the extension. It will not start with the '.' character.
			static void					ToFileExtension( char* inout_string, size_t len = 0 );

			static void					ToLower( char* inout_string, size_t len = 0 );
			static void					ToUpper( char* inout_string, size_t len = 0 );

			static void					ToPathStandard( char* inout_string, size_t len = 0 );
			static void					ToResourceName( char* inout_string, size_t len = 0 );

			//===============================================================================================//
			// misc
			//===============================================================================================//

			template<class object>
			object						ToObject ( const char* t );
			template<class object>
			std::string					ToString ( const object& o );
		}
	}
}

#include "core/utils/string.hpp"

#endif//CORE_STRING_HELPER_H_