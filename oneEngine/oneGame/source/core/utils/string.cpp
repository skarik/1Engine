#include "core/utils/string.h"

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

namespace core {
namespace utils {
namespace string
{
	//===============================================================================================//
	// misc
	//===============================================================================================//

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

}}}

