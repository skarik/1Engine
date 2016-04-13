
#ifndef _CORE_STRING_EXTENSION_H_
#define _CORE_STRING_EXTENSION_H_

#include <cstring>

FORCE_INLINE static bool streq ( const char* str1, const char* str2 )
{
	return strcmp( str1, str2 ) == 0;
}
FORCE_INLINE static bool strneq ( const char* str1, const char* str2, size_t maxcount )
{
	return strncmp( str1, str2, maxcount ) == 0;
}

#endif//_CORE_STRING_EXTENSION_H_