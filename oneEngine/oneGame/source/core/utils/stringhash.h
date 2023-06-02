#ifndef CORE_UTILS_STRING_HASH_H_
#define CORE_UTILS_STRING_HASH_H_

#include "core/common.h"

#pragma warning(push)
#pragma warning(disable: 4307) // Integer overflow

//	arHashString(str) : Returns a hash for a null-terminated string.
constexpr size_t arHashString ( const char* str )
{
	// http://www.cse.yorku.ca/~oz/hash.html
	const char* s = str;
	size_t hash = 5381;
	int ch = 0;
	while (ch = *s++)
	{
		hash = ((hash << 5) + hash) + ch;
	}
	return hash;
}

//	arHashStringRecursive(str) : Returns a hash for a null-terminated string. Calculated recursively to allow for constexpr usage on dumbass compilers.
constexpr size_t arHashStringRecursive ( const char* str, const size_t hash = 5381, const int n = 0 )
{	
	const int ch = str[n];
	const size_t next_hash = ((hash << 5) + hash) + ch;
	return ch ? arHashStringRecursive( str, next_hash, n + 1 ) : hash;
}

#pragma warning(pop)


#endif//CORE_UTILS_STRING_HASH_H_