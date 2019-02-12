//===============================================================================================//
//
//		hash.h
//
// Provides string and etc hashing within the math::hash namespace.
// Both 32 and 64 bit variants are strived to be made available.
//
//===============================================================================================//
#ifndef CORE_MATH_HASH_H_
#define CORE_MATH_HASH_H_

#include "core/types.h"

namespace math
{
	namespace hash
	{
		// todo: add murmur2

		FORCE_INLINE
		uint32_t fnv1a_32 ( const char* buf, const int len )
		{
			uint32_t hash = 0x811c9dc5;
			for (int i = 0; i < len; ++i)
			{
				hash = hash ^ buf[i];
				hash = hash * 0x1000193;
			}
			return hash;
		}

		FORCE_INLINE
		uint32_t fnv1a_32 ( const char* buf )
		{
			uint32_t hash = 0x811c9dc5;
			do
			{
				hash = hash ^ *buf;
				hash = hash * 0x1000193;
			}
			while (*(++buf) != 0);
			return hash;
		}

		FORCE_INLINE
		uint64_t fnv1a_64 ( const char* buf, const int len )
		{
			uint64_t hash = 0xcbf29ce484222325;
			for (int i = 0; i < len; ++i)
			{
				hash = hash ^ buf[i];
				hash = hash * 0x100000001b3;
			}
			return hash;
		}

		FORCE_INLINE
		uint64_t fnv1a_64 ( const char* buf )
		{
			uint64_t hash = 0xcbf29ce484222325;
			do
			{
				hash = hash ^ *buf;
				hash = hash * 0x100000001b3;
			}
			while (*(++buf) != 0);
			return hash;
		}
	}
}

#endif//CORE_MATH_HASH_H_