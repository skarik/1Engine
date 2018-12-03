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

			static std::string			GetLower( const std::string& input );
			static std::string			GetUpper( const std::string& input );

			static std::string			TrimRight( const std::string &t, const std::string &ws = "\0\t\f\v\n\r " );
			static std::string			TrimLeft ( const std::string &t, const std::string &ws = "\0\t\f\v\n\r " );
			static std::string			FullTrim ( const std::string &t );

			static std::vector<std::string> Split  (const std::string& s, const std::string& delim, const bool keep_empty = true );

			static size_t				LargestCommonSubstringLength ( const std::string& str1, const std::string& str2 );

			//===============================================================================================//
			// C-string
			//===============================================================================================//

			static void					ToFileStemLeaf( char* inout_string, size_t len = 0 );
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
		}
	}
}

#include "core/utils/string.hpp"

#endif//CORE_STRING_HELPER_H_