
#ifndef _THREADS_SEMAPHORE_H_
#define _THREADS_SEMAPHORE_H_

#include <mutex>
#include <atomic>
#include <condition_variable>
#include "compile_mode.h"

namespace Threads
{
	class semaphore
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
		semaphore (int count_ = 0)
#	ifdef THREAD_CLASSES_USE_ATOMICS
		{
			count.store(count_);
		}
#	else
			: count(count_) {}
#	endif

		inline void notify()
		{
#		ifndef THREAD_CLASSES_USE_ATOMICS
			std::unique_lock<std::mutex> lock(mtx);
#		endif
			count++;
			cv.notify_one();
		}

		inline void wait()
		{
#		ifndef THREAD_CLASSES_USE_ATOMICS
			std::unique_lock<std::mutex> lock(mtx);
#		endif
			while ( count <= 0 ) {
#			ifdef THREAD_CLASSES_USE_ATOMICS
				std::unique_lock<std::mutex> lock(mtx);
#			endif
				cv.wait(lock);
			}
			count--;
		}
	};
}

#endif//_THREADS_SEMAPHORE_H_