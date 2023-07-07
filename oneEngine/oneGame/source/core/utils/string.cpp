#include "core/utils/string.h"

#include "core/types/types.h"
#include "core/math/Vector2.h"
#include "core/math/Vector3.h"
#include "core/math/Vector4.h"
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

namespace core {
namespace utils {
namespace string
{
	//===============================================================================================//
	// misc
	//===============================================================================================//

	template<> Vector2f ToObject ( const char* t )
	{
		Vector2f result;
		char* pos;
		result.x = (Real)strtod( t, &pos );
		result.y = (Real)strtod( pos, NULL );
		return result;
	}
	template<> Vector3f ToObject ( const char* t )
	{
		Vector3f result;
		char* pos;
		result.x = (Real)strtod( t, &pos );
		result.y = (Real)strtod( pos, &pos );
		result.z = (Real)strtod( pos, NULL );
		return result;
	}
	template<> Vector4f ToObject ( const char* t )
	{
		Vector4f result;
		char* pos;
		result.x = (Real)strtod( t, &pos );
		result.y = (Real)strtod( pos, &pos );
		result.z = (Real)strtod( pos, &pos );
		result.w = (Real)strtod( pos, NULL );
		return result;
	}
	template<> Color ToObject ( const char* t )
	{
		Color result;
		char* pos;
		result.red = (Real)strtod( t, &pos );
		result.green = (Real)strtod( pos, &pos );
		result.blue = (Real)strtod( pos, &pos );
		result.alpha = (Real)strtod( pos, NULL );
		return result;
	}
	template<> ColorRGBA16 ToObject ( const char* t )
	{
		ColorRGBA16 result(0, 0, 0, 0);
		char* pos;
		result.r = (int16_t)strtol( t, &pos, 0 );
		result.g = (int16_t)strtol( pos, &pos, 0  );
		result.b = (int16_t)strtol( pos, &pos, 0  );
		long alpha = strtol( pos, NULL, 0  );
		result.a = (alpha != LONG_MIN && alpha != LONG_MAX) ? (int16_t)alpha : 255;
		return result;
	}
	template<> Rect ToObject ( const char* t )
	{
		Rect result;
		char* pos;
		result.pos.x = (Real)strtod( t, &pos );
		result.pos.y = (Real)strtod( pos, &pos );
		result.size.x = (Real)strtod( pos, &pos );
		result.size.y = (Real)strtod( pos, NULL );
		return result;
	}

	template<> std::string ToString ( const Vector2f& o )
	{
		return std::to_string(o.x) + " " + std::to_string(o.y);
	}
	template<> std::string ToString ( const Vector3f& o )
	{
		return std::to_string(o.x) + " " + std::to_string(o.y) + " " + std::to_string(o.z);
	}
	template<> std::string ToString ( const Vector4f& o )
	{
		return std::to_string(o.x) + " " + std::to_string(o.y) + " " + std::to_string(o.z) + " " + std::to_string(o.w);
	}
	template<> std::string ToString ( const Color& o )
	{
		return std::to_string(o.red) + " " + std::to_string(o.green) + " " + std::to_string(o.blue) + " " + std::to_string(o.alpha);
	}
	template<> std::string ToString ( const ColorRGBA16& o )
	{
		return std::to_string(o.r) + " " + std::to_string(o.g) + " " + std::to_string(o.b) + " " + std::to_string(o.a);
	}
	template<> std::string ToString ( const Rect& o )
	{
		return std::to_string(o.pos.x) + " " + std::to_string(o.pos.y) + " " + std::to_string(o.size.x) + " " + std::to_string(o.size.y);
	}

}}}

