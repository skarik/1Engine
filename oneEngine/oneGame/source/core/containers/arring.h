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
#include "core/debug.h"

template <typename OBJECT, unsigned short Ln>
class arring
{
public:
	explicit	arring ( bool memsetData = false )
	{
		if (memsetData)
			memset(data, 0, sizeof(OBJECT) * Ln);
		current = 0;
	}

	// at(index) : Returns object at the given index
	OBJECT& at ( const unsigned short index )
	{
		ARCORE_ASSERT(index < Ln);
		return data[index];
	}
	// at(index) : Returns object at the given index
	const OBJECT& at ( const unsigned short index ) const
	{
		ARCORE_ASSERT(index < Ln);
		return data[index];
	}

	// get() : Returns currently selected object
	OBJECT& get ( void )
	{
		return data[current];
	}
	// get() : Returns currently selected object
	const OBJECT& get ( void ) const
	{
		return data[current];
	}
	//	increment() : Increments the counter. Will loop around.
	void increment ( void )
	{
		current = (current + 1) % Ln;
	}

	//	get_incrememnt() : Increments the counter, then returns the previously selected item.
	const OBJECT& get_increment ( void )
	{
		unsigned short prev = current;
		this->increment();
		return data[prev];
	}
	//	set_increment(value) : Sets the current selected item, then increments the counter.
	OBJECT set_increment ( OBJECT& value )
	{
		OBJECT prev = data[current];
		data[current] = value;
		this->increment();
		return prev;
	}

	//	fill(value) : Fills all elements with the given value
	void fill ( OBJECT value )
	{
		for (int i = 0; i < Ln; ++i)
		{
			data[Ln] = value;
		}
	}

	//	size() : Returns the length of the ring
	size_t size ( void ) const 
	{
		return Ln;
	}

private:
	OBJECT data [Ln];
	unsigned short current;
};

#endif//CORE_RING_H_