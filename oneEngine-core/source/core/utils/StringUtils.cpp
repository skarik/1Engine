
#include "StringUtils.h"

#include <vector>
#include <iostream>
#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>
//#include <boost/algorithm/string.hpp>

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

namespace StringUtils
{
	string GetFileExtension( const string& FileName )
	{
		if ( FileName.find_last_of(".") != std::string::npos )
			return FileName.substr(FileName.find_last_of(".")+1);
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
		/*std::string str = t;
		size_t found;
		found = str.find_last_not_of(ws);
		if (found != std::string::npos) {
			str.erase(found+1);
		}
		else { // str is all whitespace
			str.clear();  
		}
		return str;*/
		std::string s = t;
		s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
		return s;
	}
	//http://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring
	string TrimLeft ( const string &t, const string &ws )
	{
		/*std::string str = t;
		boost::trim( t );
		size_t found;
		//found = str.find_first_not_of(ws);
		//while (found != std::string::npos) {
		//	str = str.substr(found+1);
		//	found = str.find_first_not_of(ws);
		//}
		found = str.find_first_of(ws);
		while (found != std::string::npos) {
			str = str.substr(found+1);
			found = str.find_first_of(ws);
		}
		std::cout << "result: " << str << std::endl;*/
		std::string s = t;
		s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
		return s;
	}

	string FullTrim ( const string &t )
	{
		// Trim input string name
		std::string trimmedMatname = t;
		//boost::trim( trimmedMatname );
		trimmedMatname = TrimLeft(TrimRight(trimmedMatname));
		char str [128];
		char*tok;
		strcpy( str, trimmedMatname.c_str() );
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
		result.x = (ftype)strtod( t, &pos );
		result.y = (ftype)strtod( pos, NULL );
		return result;
	}
	template<> inline Vector3d ToObject ( const char* t )
	{
		Vector3d result;
		char* pos;
		result.x = (ftype)strtod( t, &pos );
		result.y = (ftype)strtod( pos, &pos );
		result.z = (ftype)strtod( pos, NULL );
		return result;
	}
	template<> inline Vector4d ToObject ( const char* t )
	{
		Vector4d result;
		char* pos;
		result.x = (ftype)strtod( t, &pos );
		result.y = (ftype)strtod( pos, &pos );
		result.z = (ftype)strtod( pos, &pos );
		result.w = (ftype)strtod( pos, NULL );
		return result;
	}
	template<> inline Color ToObject ( const char* t )
	{
		Color result;
		char* pos;
		result.red = (ftype)strtod( t, &pos );
		result.green = (ftype)strtod( pos, &pos );
		result.blue = (ftype)strtod( pos, &pos );
		result.alpha = (ftype)strtod( pos, NULL );
		return result;
	}
	template<> inline Rect ToObject ( const char* t )
	{
		Rect result;
		char* pos;
		result.pos.x = (ftype)strtod( t, &pos );
		result.pos.y = (ftype)strtod( pos, &pos );
		result.size.x = (ftype)strtod( pos, &pos );
		result.size.y = (ftype)strtod( pos, NULL );
		return result;
	}

}