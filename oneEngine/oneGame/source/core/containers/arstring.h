//===============================================================================================//
//
//		arstring.h
//
// Explicitly sized string class.
// A string class that's a POD type, and uses built-in C++ operators to do most of its work.
// Holds a statically sized null-terminated string (C string) to represent its data.
//
//===============================================================================================//
#ifndef CORE_AR_STRING_H_
#define CORE_AR_STRING_H_

#include "core/types/types.h"
#include <cstring>

template <unsigned short Ln>
class arstring
{
public:
	arstring ( const char* str ) {
		if ( str ) {
			strncpy( data, str, Ln );
		}
		else {
			data[0] = 0;
		}
	}
	arstring ( void ) {
		strcpy( data, "" );
	}
	arstring ( arstring<Ln>&& other ) {
		strncpy( data, other.data, Ln );
	}
	arstring( const arstring<Ln>& other ) {
		strncpy( data, other.data, Ln );
	}

	operator char* ( void ) {
		return (char*)data;
	}
	operator const char* ( void ) const {
		return (const char*)data;
	}
	const char* c_str ( void ) const {
		return (char*)data;
	}

	arstring<Ln> & operator= ( const char* str ) {
		if ( str ) {
			strncpy( data, str, Ln );
		}
		else {
			data[0] = 0;
		}
		return *this;
	}

	arstring<Ln> & operator= ( const arstring<Ln> & str ) {
		strncpy( data, str, Ln );
		return *this;
	}

	template <unsigned short Lm>
	arstring<Ln> & operator= ( const arstring<Lm> & str ) {
		strncpy( data, str, Ln );
		return *this;
	}
	template <unsigned short Lm>
	bool operator== ( const arstring<Lm> & str ) const {
		return strcmp( data, str.data )==0;
	}
	/*bool operator== ( const char* str ) {
		return strcmp( data, str )==0;
	}*/
	bool compare( const char* str ) const {
		if ( str ) {
			return strcmp( data, str )==0;
		}
		else {
			return false;
		}
	}
	template <unsigned short Lm>
	bool operator!= ( const arstring<Lm> & str ) const {
		return strcmp( data, str.data )!=0;
	}

	arstring<Ln> operator+ ( const char* str ) {
		if ( str ) {
			arstring<Ln> resultant ( *this );
			strcat( resultant.data, str );
			return resultant;
		}
		return *this;
	}

	template <unsigned short Lm>
	arstring<Ln> operator+ ( const arstring<Lm> & str ) {
		arstring<Ln> resultant ( *this );
		strcat( resultant.data, str.data );
		return resultant;
	}
	void operator+= ( const char* str ) {
		if ( str ) {
			strcat( data, str );
		}
	}
	template <unsigned short Lm>
	void operator+= ( const arstring<Lm> & str ) {
		strcat( data, str.data );
	}


	size_t length ( void ) const {
		return strlen( data );
	}

public:
	char data [Ln];

private:
	bool operator== ( const char* str ) const {
		return strcmp( data,str )==0;
	};
};

// Inject hash-table (std::unordered_map) functionality for arstring into the STD namespace
#include <functional>
namespace std
{
	template <>
	struct hash<arstring<128>>
	{
		size_t operator()(const arstring<128>& str) const
		{	// http://www.cse.yorku.ca/~oz/hash.html
			char* s = (char*)str.c_str();
			size_t h = 5381;
			int c;
			while ((c = *s++))
				h = ((h << 5) + h) + c;
			return h;
		}
	};
	template <>
	struct equal_to<arstring<128>>
	{
		bool operator()(const arstring<128> &lhs, const arstring<128> &rhs) const 
		{
			return lhs.compare(rhs);
		}
	};
	template <>
	struct less<arstring<128>>
	{
		bool operator()(const arstring<128> &lhs, const arstring<128> &rhs) const 
		{
			return strcmp(lhs.data,rhs.data) < 0;
		}
	};
}

// Ease of use defines
typedef arstring<64>	arstring64;
typedef arstring<128>	arstring128;
typedef arstring<256>	arstring256;

#endif//CORE_AR_STRING_H_