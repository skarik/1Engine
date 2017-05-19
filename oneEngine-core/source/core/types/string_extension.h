//===============================================================================================//
//
//		string_extension.h
//
// Contains quick one-off string utilities that don't belong anywhere else (probably).
//
//===============================================================================================//
#ifndef CORE_STRING_EXTENSION_H_
#define CORE_STRING_EXTENSION_H_

#include <cstring>

// Returns true if strings are equal. Uses strcmp internally.
FORCE_INLINE static bool streq ( const char* str1, const char* str2 )
{
	return strcmp( str1, str2 ) == 0;
}
// Returns true if strings are equal, comparing N characters. Uses strncmp internally.
FORCE_INLINE static bool strneq ( const char* str1, const char* str2, size_t maxcount )
{
	return strncmp( str1, str2, maxcount ) == 0;
}

#endif//CORE_STRING_EXTENSION_H_