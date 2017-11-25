
#include "StringUtils.h"

#include "core/types/types.h"
#include "core/math/Vector2d.h"
#include "core/math/Vector3d.h"
#include "core/math/Vector4d.h"
#include "core/math/Quaternion.h"
#include "core/math/matrix/CMatrix.h"
#include "core/math/Rotator.h"
#include "core/math/XTransform.h"
#include "core/math/Color.h"
#include "core/math/Rect.h"

#include <vector>
#include <iostream>
#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>

namespace StringUtils
{
	//	GetFileStemLeaf ( filename ) : Returns the file without the extension. Not gauranteed to work with a missing extension.
	string GetFileStemLeaf( const string& filename )
	{
		return filename.substr(0, filename.find_last_of("."));
	}

	//	GetFileExtension ( filename ) : Returns the file's last extension.
	string GetFileExtension( const string& filename )
	{
		if ( filename.find_last_of(".") != std::string::npos )
			return filename.substr(filename.find_last_of(".")+1);
		return "";
	}

	string ToLower( const string& input )
	{
		std::string result = input;
		std::transform( result.begin(), result.end(), result.begin(), ::tolower );
		return result;
	}

	string TrimRight( const string &t, const string &ws )
	{
		std::string s = t;
		s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
		return s;
	}
	//http://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring
	string TrimLeft ( const string &t, const string &ws )
	{
		std::string s = t;
		s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
		return s;
	}

	string FullTrim ( const string &t )
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

	//http://stackoverflow.com/questions/236129/how-to-split-a-string-in-c
	std::vector<string> Split( const string& s, const string& delim, const bool keep_empty )
	{
		std::vector<string> result;
		if (delim.empty()) {
			result.push_back(s);
			return result;
		}
		/*string::const_iterator substart = s.begin(), subtemp, subend;
		while (true)
		{
			// Search string for first min delim
			for ( auto delims = delim.begin(); delims != delim.end(); ++delims )
			{
				//subend = search(substart, s.end(), delim.begin(), delim.end());
				subend = s
			}
			string temp(substart, subend);
			if (keep_empty || !temp.empty()) {
				result.push_back(temp);
			}
			if (subend == s.end()) {
				break;
			}
			substart = subend + delim.size();
		}*/
		string::const_iterator substart = s.begin(), subtemp, subend;
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
			string temp(substart, subend);
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

	size_t LargestCommonSubstringLength(const string& str1, const string& str2)
	{
		// Make sure str1 is smaller
		string s1 = str1;
		string s2 = str2;
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


	//template<typename object>
	//object					ToObject ( const char* t )

	/*template<Vector3d>
	inline Vector3d	ToObject<Vector3d> ( const char* t )
	{
		return Vector3d(0,0,0);
	}*/
	template<> inline Vector2d ToObject ( const char* t )
	{
		Vector2d result;
		char* pos;
		result.x = (Real)strtod( t, &pos );
		result.y = (Real)strtod( pos, NULL );
		return result;
	}
	template<> inline Vector3d ToObject ( const char* t )
	{
		Vector3d result;
		char* pos;
		result.x = (Real)strtod( t, &pos );
		result.y = (Real)strtod( pos, &pos );
		result.z = (Real)strtod( pos, NULL );
		return result;
	}
	template<> inline Vector4d ToObject ( const char* t )
	{
		Vector4d result;
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
	}

}