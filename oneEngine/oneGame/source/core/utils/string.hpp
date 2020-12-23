#ifndef CORE_STRING_HELPER_HPP_
#define CORE_STRING_HELPER_HPP_

#include "core/utils/string.h"
#include "core/types/types.h"
#include "core/debug.h"

#include <vector>
#include <iostream>
#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>

namespace core {
namespace utils {
namespace string
{
	//===============================================================================================//
	// std::string & arstring
	//===============================================================================================//

	//	GetFileStemLeaf(filename) : Returns the file without the extension.
	// Not guaranteed to work with a missing extension.
	std::string GetFileStemLeaf( const std::string& filename )
	{
		return filename.substr(0, filename.find_last_of("."));
	}
	arstring256 GetFileStemLeaf( const char* filename, size_t len )
	{
		arstring256 result (filename);
		if (len == 0)
			len = result.length();
		for (size_t pos = len - 1; pos > 0; --pos) {
			if (result[pos] == '.') {
				result[pos] = 0;
				break;
			}
			else if (result[pos] == '/' || result[pos] == '\\') {
				break;
			}
		}
		return result;
	}

	//	GetFileExtension ( filename ) : Returns the file's last extension.
	std::string GetFileExtension( const std::string& filename )
	{
		if ( filename.find_last_of(".") != std::string::npos )
			return filename.substr(filename.find_last_of(".")+1);
		return "";
	}
	arstring256 GetFileExtension( const char* filename, size_t len )
	{
		if (len == 0)
			len = strlen(filename);
		size_t pos = len;
		do {
			pos -= 1;
			if (filename[pos] == '.') {
				return arstring256(filename + pos + 1);
			}
		}
		while (pos != 0);
		return arstring256();
	}

	//	GetPathStandard(filename) : Returns a standardized version of the path.
	std::string GetPathStandard( const std::string& filename )
	{
		std::string result = GetLower(filename);
		for ( unsigned int i = 0; i < result.length(); ++i ) {
			if ( result[i] == '\\' ) {
				result[i] = '/';
			}
		}
		return result;
	}
	arstring256 GetPathStandard( const char* filename, size_t len )
	{
		if (len == 0)
			len = strlen(filename);
		arstring256 result (filename);
		ToLower(result, len);
		for ( unsigned int i = 0; i < len; ++i ) {
			if ( result[i] == '\\' ) {
				result[i] = '/';
			}
		}
		return result;
	}

	std::string GetLower( const std::string& input )
	{
		std::string result = input;
		std::transform( result.begin(), result.end(), result.begin(), ::tolower );
		return result;
	}
	std::string GetUpper( const std::string& input )
	{
		std::string result = input;
		std::transform( result.begin(), result.end(), result.begin(), ::toupper );
		return result;
	}

	void ToLower( std::string& inout )
	{
		std::transform( inout.begin(), inout.end(), inout.begin(), ::tolower );
	}
	void ToUpper( std::string& inout )
	{
		std::transform( inout.begin(), inout.end(), inout.begin(), ::toupper );
	}


	std::string TrimRight( const std::string &t, const std::string &ws )
	{
		std::string s = t;
		s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
		return s;
	}
	std::string TrimLeft ( const std::string &t, const std::string &ws )
	{
		// Referenced from:
		//http://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring
		
		std::string s = t;
		s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
		return s;
	}

	std::string FullTrim ( const std::string &t )
	{
		// Trim input string name
		std::string trimmedMatname = TrimLeft(TrimRight(t));

		char str [128];
		char*tok;
		memcpy( str, trimmedMatname.c_str(), 128 );
		str[127] = 0;
		tok = strtok( str, "\0 ");
		trimmedMatname.clear();
		trimmedMatname = tok;

		return trimmedMatname;
	}

	std::vector<std::string> Split( const std::string& s, const std::string& delim, const bool keep_empty )
	{
		// Referenced from: 
		//http://stackoverflow.com/questions/236129/how-to-split-a-string-in-c

		std::vector<std::string> result;
		if (delim.empty()) {
			result.push_back(s);
			return result;
		}
		std::string::const_iterator substart = s.begin(), subtemp, subend;
		while ( true )
		{
			// Search string for first min delim
			subend = s.end();
			for ( auto delims = delim.begin(); delims != delim.end(); ++delims )
			{
				subtemp = std::find( substart, s.end(), *delims );
				if ( subtemp != s.end() && subtemp < subend )
				{
					subend = subtemp;
				}
			}
			// Make string from that found position
			std::string temp(substart, subend);
			if (keep_empty || !temp.empty()) {
				result.push_back(temp);
			}
			// End if at end
			if ( subend == s.end() ) {
				break;
			}
			// Go to next search state
			substart = subend + 1;
			// End if at end
			if ( substart == s.end() ) {
				break;
			}
		}
		return result;
	}

	size_t LargestCommonSubstringLength(const std::string& str1, const std::string& str2)
	{
		// Make sure str1 is smaller
		std::string s1 = str1;
		std::string s2 = str2;
		if ( s1.length() > s2.length() )
		{
			std::swap( s1, s2 );
		}

		// Loop through str2
		size_t maxsize, checksize, checklen;
		maxsize = 0;
		for ( size_t j = 0; j < s2.length(); ++j )
		{
			checksize = 0;
			checklen = std::min( s1.length(), s2.length() - j );

			if ( maxsize >= checklen )
			{
				return maxsize;
			}

			for ( size_t i = 0; i < checklen; ++i )
			{
				if ( s1[i] == s2[i+j] )
				{
					checksize += 1;
				}
				else
				{
					maxsize = std::max( maxsize, checksize );
					checksize = 0;
				}
			}
			maxsize = std::max( maxsize, checksize );
		}
		return maxsize;
	}

	//	CamelCaseToReadable(string) : Makes a human-readable spaced string from a camel-cased string
	arstring256 CamelCaseToReadable( const char* camelCased, size_t len )
	{
		arstring256 result = camelCased;

		size_t result_itr = 1;
		for (size_t i = 1; i < len; ++i)
		{
			ARCORE_ASSERT(result_itr < sizeof(arstring256) - 2);

			// Is this an upper case character
			if (::isupper(camelCased[i]) && ::islower(camelCased[i - 1]))
			{
				//result_itr
				result[result_itr++] = ' ';
				result[result_itr++] = camelCased[i];
			}
			else
			{
				result[result_itr++] = camelCased[i];
			}
		}
		// Put null character at the end
		result[result_itr] = 0;

		return result;
	}

	//===============================================================================================//
	// C-string
	//===============================================================================================//

	void ToFileStemLeaf( char* inout_string, size_t len )
	{
		if (len == 0)
			len = strlen(inout_string);
		for (size_t pos = len - 1; pos > 0; --pos)
		{
			if (inout_string[pos] == '.')
			{
				inout_string[pos] = 0;
				break;
			}
			else if (inout_string[pos] == '/' || inout_string[pos] == '\\')
			{
				break;
			}
		}
	}
	void ToFileExtension( char* inout_string, size_t len )
	{
		if (len == 0)
			len = strlen(inout_string);
		size_t pos = len;
		do
		{
			pos -= 1;
			if (inout_string[pos] == '.')
			{
				pos += 1;
				size_t ext_len = len - pos;
				for (size_t i = 0; i < ext_len; ++i)
				{
					inout_string[i] = inout_string[i + pos];
				}
				inout_string[ext_len] = 0;
				break;
			}
		}
		while (pos != 0);
	}

	void ToLower( char* inout_string, size_t len )
	{
		if (len == 0)
			len = strlen(inout_string);
		for (size_t pos = 0; pos < len; ++pos) {
			inout_string[pos] = ::tolower(inout_string[pos]);
		}
	}
	void ToUpper( char* inout_string, size_t len )
	{
		if (len == 0)
			len = strlen(inout_string);
		for (size_t pos = 0; pos < len; ++pos) {
			inout_string[pos] = ::toupper(inout_string[pos]);
		}
	}

	void ToPathStandard( char* inout_string, size_t len )
	{
		if (len == 0)
			len = strlen(inout_string);
		ToLower(inout_string, len);
		for (size_t pos = 0; pos < len; ++pos) {
			if (inout_string[pos] == '\\') {
				inout_string[pos] = '/';
			}
		}
	}
	void ToResourceName( char* inout_string, size_t len )
	{
		if (len == 0)
			len = strlen(inout_string);
		ToPathStandard(inout_string, len);
		ToFileStemLeaf(inout_string, len);
	}

	//===============================================================================================//
	// misc
	//===============================================================================================//

	/*template<> inline Vector2f ToObject ( const char* t )
	{
		Vector2f result;
		char* pos;
		result.x = (Real)strtod( t, &pos );
		result.y = (Real)strtod( pos, NULL );
		return result;
	}
	template<> inline Vector3f ToObject ( const char* t )
	{
		Vector3f result;
		char* pos;
		result.x = (Real)strtod( t, &pos );
		result.y = (Real)strtod( pos, &pos );
		result.z = (Real)strtod( pos, NULL );
		return result;
	}
	template<> inline Vector4f ToObject ( const char* t )
	{
		Vector4f result;
		char* pos;
		result.x = (Real)strtod( t, &pos );
		result.y = (Real)strtod( pos, &pos );
		result.z = (Real)strtod( pos, &pos );
		result.w = (Real)strtod( pos, NULL );
		return result;
	}
	template<> inline Color ToObject ( const char* t )
	{
		Color result;
		char* pos;
		result.red = (Real)strtod( t, &pos );
		result.green = (Real)strtod( pos, &pos );
		result.blue = (Real)strtod( pos, &pos );
		result.alpha = (Real)strtod( pos, NULL );
		return result;
	}
	template<> inline Rect ToObject ( const char* t )
	{
		Rect result;
		char* pos;
		result.pos.x = (Real)strtod( t, &pos );
		result.pos.y = (Real)strtod( pos, &pos );
		result.size.x = (Real)strtod( pos, &pos );
		result.size.y = (Real)strtod( pos, NULL );
		return result;
	}*/

}}}

#endif//CORE_STRING_HELPER_HPP_