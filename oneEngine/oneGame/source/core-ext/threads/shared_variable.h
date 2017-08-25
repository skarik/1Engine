
#ifndef _THREADS_SHARED_VARIABLE_H_
#define _THREADS_SHARED_VARIABLE_H_

#include "boost/thread.hpp"

namespace Threads
{
	template <typename Type>
	class shared_variable
	{
	private:
		boost::mutex	m_lock;
		Type			m_value;

	public:
		// constructor taking value
		shared_variable ( const Type& n_value )
			: m_value(n_value)
		{
			;
		}
		shared_variable ( void )
		{
			;
		}

		// get value
		void get ( Type& o_value )
		{
			m_lock.lock();
			o_value = m_value;
			m_lock.unlock();
		}
		// get value
		const Type& get ( void )
		{
			boost::mutex::scoped_lock readlock ( m_lock );
			return m_value;
		}

		// set value
		void set ( const Type& i_value )
		{
			m_lock.lock();
			m_value = i_value;
			m_lock.unlock();
		}

		// checks if same as first argument. if same, then sets value to second argument and returns true.
		// if not same, returns false. has no race condition
		bool set_if_same_as ( const Type& i_compare_value, const Type& i_set_to_value )
		{
			boost::mutex::scoped_lock rwlock ( m_lock );
			if ( m_value == i_compare_value ) {
				m_value = i_set_to_value;
				return true;
			}
			return false;
		}
	};
};

#endif//_THREADS_SHARED_VARIABLE_H_