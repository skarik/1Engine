//===============================================================================================//
//
//		core-ext/threads/spinlock.h
//
// Atomic spinlock implementation.
//
//===============================================================================================//
#ifndef CORE_EXT_THREADS_ATOMIC_SPINLOCK_H_
#define CORE_EXT_THREADS_ATOMIC_SPINLOCK_H_

#include <atomic>

namespace core {
namespace threads {

	// @brief Atomic spinlock class. Implement critical areas with a silly spinlock - for when mutexes are too heavy.
	//        Does not enforce ordering of all the threads waiting on WaitEnter.
	class Spinlock
	{
	public:
		// @brief Attempts to grab the boolean. Spinwaits if not.
		void			WaitEnter ( void )
		{
			bool expected;
			do
			{
				expected = false;
			}
			while (isLocked.compare_exchange_weak(expected, true));
		}
		// @brief Attempts to grab the boolean. Returns false if not, returns true if yes.
		bool			TryEnter ( void )
		{
			bool expected = false;
			return isLocked.compare_exchange_weak(expected, true);
		}

		// @brief Leaves the area.
		void			Exit ( void )
		{
			bool expected = true;
			isLocked.compare_exchange_weak(expected, false);
		}

	public:
		std::atomic_bool	isLocked = false;
	};

	// @brief Scoped utility for the Spinlock class
	class ScopedSpinlock
	{
	public:
		explicit		ScopedSpinlock ( Spinlock& spinlock )
			: lock(spinlock)
		{
			lock.WaitEnter();
		};

		/**/			~ScopedSpinlock ( void )
		{
			lock.Exit();
		}

	private:
		Spinlock&			lock;
	};


}}

#endif//CORE_EXT_THREADS_ATOMIC_SPINLOCK_H_