
#ifndef _T_LONG_INT_POSITION_H_
#define _T_LONG_INT_POSITION_H_

#include "core/types/types.h"
#include <functional>

// Structure Definition
typedef int32_t rangeint;
struct WorldVector
{
	rangeint x;
	rangeint y;
	rangeint z;

	WorldVector ( rangeint const& inx, rangeint const& iny, rangeint const& inz )
	{
		x = inx;
		y = iny;
		z = inz;
	};
	WorldVector ( void )
	{
		x = 0;
		y = 0;
		z = 0;
	};
	bool operator== (const WorldVector& b) const
	{
		return (( b.x == x )&&( b.y == y )&&( b.z == z ));
	}
	bool operator!= (const WorldVector& b) const
	{
		return (( b.x != x )||( b.y != y )||( b.z != z ));
	}
	bool operator< (const WorldVector& b) const
	{
		if ( x < b.x ) {
			return true;
		}
		else if ( x == b.x )
		{
			if ( y < b.y ) {
				return true;
			}
			else if ( y == b.y )
			{
				if ( z < b.z ) {
					return true;
				}
			}
		}
		return false;
	}

	bool operator<= ( const WorldVector& b ) const
	{
		if ( (*this) < b ) {
			return true;
		}
		if ( (*this) == b ) {
			return true;
		}
		return false;
	}
	bool operator> ( const WorldVector& b ) const
	{
		if ( x > b.x ) {
			return true;
		}
		else if ( x == b.x )
		{
			if ( y > b.y ) {
				return true;
			}
			else if ( y == b.y )
			{
				if ( z > b.z ) {
					return true;
				}
			}
		}
		return false;
	}

	// Performs operator<() but ignores Z coordinate
	bool CompareXY ( const WorldVector& b ) const
	{
		if ( x < b.x ) {
			return true;
		}
		else if ( x == b.x )
		{
			if ( y < b.y ) {
				return true;
			}
		}
		return false;
	}

	// size_t cast : used for indexing RangeVectors into hash tables
	operator size_t () const
	{
		// Following implementation based on: http://www.azillionmonkeys.com/qed/hash.html
#if !defined (get16bits)
#define get16bits(d) ((((uint32_t)(((const uint8_t *)(d))[1])) << 8)\
                       +(uint32_t)(((const uint8_t *)(d))[0]) )
#endif
		int len = 4*3;
		char* data = (char*)&x;
		
		uint32_t hash = len, tmp;
		int rem;

		rem = len & 3;
		len >>= 2;

		// Main loop
		for (;len > 0; len--) {
			hash  += get16bits (data);
			tmp    = (get16bits (data+2) << 11) ^ hash;
			hash   = (hash << 16) ^ tmp;
			data  += 2*sizeof (uint16_t);
			hash  += hash >> 11;
		}

		// Handle end cases
		switch (rem) {
			case 3: hash += get16bits (data);
					hash ^= hash << 16;
					hash ^= ((signed char)data[sizeof (uint16_t)]) << 18;
					hash += hash >> 11;
					break;
			case 2: hash += get16bits (data);
					hash ^= hash << 11;
					hash += hash >> 17;
					break;
			case 1: hash += (signed char)*data;
					hash ^= hash << 10;
					hash += hash >> 1;
		}

		// Force "avalanching" of final 127 bits
		hash ^= hash << 3;
		hash += hash >> 5;
		hash ^= hash << 4;
		hash += hash >> 17;
		hash ^= hash << 25;
		hash += hash >> 6;

		return hash;
	}

	// Addition shortcut
	WorldVector operator+ (WorldVector const& right) const
	{
		return WorldVector( x+right.x,y+right.y,z+right.z );
	}

};

// Definition for C++11 hash function
template<> struct std::hash <WorldVector> {
	size_t operator() ( const WorldVector& arg ) const {
		return (size_t)(arg);
	}
};

// Backwards compatibility typedef
typedef WorldVector RangeVector;

#endif