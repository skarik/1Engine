
#ifndef _THREADS_COUNT_H_
#define _THREADS_COUNT_H_

#include <mutex>
#include <atomic>
#include <condition_variable>
#include "compile_mode.h"

namespace Threads
{
	class counter
	{
	private:
		std::mutex mtx;
		std::condition_variable cv;
#	ifdef THREAD_CLASSES_USE_ATOMICS
		std::atomic_int count;
#	else
		int count;
#	endif
	public:
		counter (int count_ = 0)
#	ifdef THREAD_CLASSES_USE_ATOMICS
		{
			count.store(count_);
		}
#	else
			: count(count_) {}
#	endif

		inline void add()
		{
#		ifndef THREAD_CLASSES_USE_ATOMICS
			std::unique_lock<std::mutex> lock(mtx);
#		endif
			count++;
		}
		inline void sub()
		{
#		ifndef THREAD_CLASSES_USE_ATOMICS
			std::unique_lock<std::mutex> lock(mtx);
#		endif
			count--;
			if ( count <= 0 )
			{
				cv.notify_all();
			}
		}

		inline void wait_for_0()
		{
#		ifndef THREAD_CLASSES_USE_ATOMICS
			std::unique_lock<std::mutex> lock(mtx);
#		endif
			while ( count > 0 ) {
#			ifdef THREAD_CLASSES_USE_ATOMICS
				std::unique_lock<std::mutex> lock(mtx);
#			endif
				cv.wait(lock);
			}
		}
	};
}

#endif//_THREADS_COUNT_H_