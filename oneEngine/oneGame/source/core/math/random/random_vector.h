
#ifndef _RANDOM_VECTOR_H_
#define _RANDOM_VECTOR_H_

#include <vector>
#include <stdexcept> 

#include "core/types/types.h"
#include "core/math/Math.h"
#include "core/math/random/Random.h"

template <class myType>
class random_vector
{
public:
	random_vector ( void ) : m_maxweight(0) {
		;
	}
	random_vector ( const random_vector<myType> & v ) : m_maxweight(v.m_maxweight), m_entries(v.m_entries) {
		;
	}

	// throws a random value into the list
	myType get_random_value ( void ) const
	{
		double _tempcast;
		_tempcast = random_range( 0.0, m_maxweight );
		unsigned int _index;
		for ( _index = 0; _index < m_entries.size(); ++_index ) {
			_tempcast -= m_entries[_index].weight;
			if ( _tempcast <= 0 ) {
				return m_entries[_index].value;
			}
		}
		throw std::out_of_range("Weight was out of calculated range.");
	}

	// throws a casted value into the list
	myType get_target_value ( const double _cast ) const
	{
		//cout << _cast << endl;
		double _tempcast = saturate(_cast)*m_maxweight;
		unsigned int _index;
		for ( _index = 0; _index < m_entries.size(); ++_index ) {
			_tempcast -= m_entries[_index].weight;
			if ( _tempcast <= 0 ) {
				return m_entries[_index].value;
			}
		}
		if ( _tempcast < m_maxweight*FTYPE_PRECISION ) {
			return m_entries[m_entries.size()-1].value;
		}
		/*if ( _tempcast < m_maxweight*0.03f ) {
			return m_entries[m_entries.size()-1].value;
		}
		char _error [256];
		sprintf( _error, "Value out of calculated range: %lf over %lf out of %lf", _tempcast,_cast,m_maxweight );
		cout << _error << endl;
		throw std::out_of_range( _error );*/
		throw std::out_of_range("Weight was out of calculated range.");
	}


	// vector-like interface

	// pushes a new value onto the entry list
	void push_back ( const myType & _value, const double _weight )
	{
		_entry_t new_entry;
		new_entry.value = _value;
		new_entry.weight = _weight;
		m_entries.push_back( new_entry );
		m_maxweight += _weight;
	}
	// erases a value from the entry list
	void find_and_erase ( const myType & _value )
	{
		for ( auto it = m_entries.begin(); it != m_entries.end(); ++it )
		{
			if ( it->value == _value )
			{
				m_maxweight -= it->weight;
				m_entries.erase( it );
			}
		}
	}
	// return size of vector
	FORCE_INLINE uint32_t size() const
	{
		return m_entries.size();
	}
	// return empty
	FORCE_INLINE bool empty() const
	{
		return m_entries.empty();
	}


private:
	struct _entry_t {
		myType value;
		double weight;
	};
	std::vector<_entry_t> m_entries;
	double m_maxweight;
};


#endif