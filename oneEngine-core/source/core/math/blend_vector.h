
#ifndef _BLEND_VECTOR_H_
#define _BLEND_VECTOR_H_

#include <vector>
#include <stdexcept> 

template <class Type>
class blend_vector {

public:
	blend_vector ( void ) : loop(false) {
		;
	}
	blend_vector ( const blend_vector<Type> & v ) : m_entries(v.m_entries), loop(v.loop) {
		;
	}

	// pushes a new value onto the entry list
	void push_back ( const Type & _value )
	{
		m_entries.push_back( _value );
	}

	// accesses an element
	Type& operator [] ( const int index ) {
		return m_entries[index];
	}
	const Type& operator [] ( const int index ) const {
		return m_entries[index];
	}

	// throws a casted value into the list
	Type get_target_value ( const double _cast ) const
	{
		double _tempcast = _cast;
		if ( _tempcast < 0 ) _tempcast = 0;
		if ( _tempcast > 1 ) _tempcast = 1;

		// Expand the cast to the size of the list
		_tempcast *= m_entries.size() - (loop?0:1);
		
		// Get the first sample target
		int sampleTarget1 = (int)floor(_tempcast);
		// Do proper wrapping on the first
		if ( sampleTarget1 >= (int)m_entries.size() ) {
			if ( loop ) {
				_tempcast -= sampleTarget1;
				sampleTarget1 = 0;
			}
			else {
				return m_entries.back();
			}
		}

		// The next item to blend is the next one
		int sampleTarget2 = sampleTarget1+1;
		// If choosing last entry, then return the last entry when not looping
		// go to first entry if looping
		if ( sampleTarget2 >= (int)m_entries.size() ) {
			if ( loop ) {
				sampleTarget2 = 0;
			}
			else {
				return m_entries.back();
			}
		}
		
		// Otherwise, linearly blend between the two targets
		_tempcast = _tempcast - sampleTarget1;
		return (Type)(m_entries[sampleTarget1]*(1-_tempcast) + m_entries[sampleTarget2]*_tempcast);

		throw std::out_of_range("Weight was out of calculated range.");
	}
public:
	bool loop;
private:
	std::vector<Type> m_entries;
};


#endif//_BLEND_VECTOR_H_