
#ifndef _THREADS_FUTURE_VALUE_H_
#define _THREADS_FUTURE_VALUE_H_

#include <thread>
#include <mutex>

namespace Threads
{
	template <typename Type>
	class future
	{
	private:
		std::mutex		m_lock;
		Type			m_value;
		bool			m_hasValue;

	public:
		future ( void ) {
			m_hasValue = false;
		};
		
		// mark again as having no value
		void reset ( void ) {
			m_lock.lock();
			m_hasValue = false;
			m_lock.unlock();
		}

		// set a value, marking as having a value
		void set ( const Type& n_input )
		{
			m_lock.lock();
			m_value = n_input;
			m_hasValue = true;
			m_lock.unlock();
		}

		// Stop until gets a value
		void wait ( void )
		{
			bool t_keepwaiting = true;
			while ( t_keepwaiting )
			{
				m_lock.lock();
				if ( m_hasValue ) {
					t_keepwaiting = false;
				}
				m_lock.unlock();
			}
		}

		// retrieve value
		void get ( Type& o_value )
		{
			wait();
			m_lock.lock();
			o_value = m_value;
			m_lock.unlock();
		}

		// get if has value read
		bool hasValue ( void )
		{
			std::lock_guard<std::mutex> readlock( m_lock );
			return m_hasValue;
		}
	};
}

#endif//_THREADS_FUTURE_VALUE_H_