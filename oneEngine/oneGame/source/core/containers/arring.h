//===============================================================================================//
//
//		arring.h
//
// Implements a simple ring-buffer style template class. Values are compile-time constants.
//
//===============================================================================================//
#ifndef CORE_RING_H_
#define CORE_RING_H_

#include "core/types/types.h"

template <typename OBJECT, unsigned short Ln>
class arring
{
public:
	explicit	arring ( void )
	{
		memset(data, 0, sizeof(OBJECT) * Ln);
		current = 0;
	}

	OBJECT& at ( const unsigned short index )
	{
		return data[at];
	}
	const OBJECT& at ( const unsigned short index ) const
	{
		return data[at];
	}

	const OBJECT& get ( void )
	{
		return data[current];
	}
	void increment ( void )
	{
		current = (current + 1) % Ln;
	}
	const OBJECT& get_increment ( void )
	{
		unsigned short prev = current;
		current = (current + 1) % Ln;
		return data[prev];
	}
	OBJECT set_increment ( OBJECT& value )
	{
		OBJECT prev = data[current];
		data[current] = value;
		current = (current + 1) % Ln;
		return prev;
	}

	void fill ( OBJECT value )
	{
		for (int i = 0; i < Ln; ++i)
		{
			data[Ln] = value;
		}
	}

private:
	OBJECT data [Ln];
	unsigned short current;
};

#endif//CORE_RING_H_