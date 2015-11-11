// arstring.h
// Explicitly sized string class.
// A string class that's a POD type, and uses built-in C++ operators to do most of its work.
// Holds a null-terminated string to represent its data.

#ifndef _C_AR_STRING_H_
#define _C_AR_STRING_H_

#include <cstring>

template <unsigned short Ln>
class arstring
{
public:
	explicit		arstring ( const char* str ) {
		if ( str ) {
			strncpy( data, str, Ln );
		}
		else {
			data[0] = 0;
		}
	}
	explicit		arstring ( void ) {
		strcpy( data, "" );
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


#endif//_C_AR_STRING_H_