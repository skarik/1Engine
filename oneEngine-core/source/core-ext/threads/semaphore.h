
#ifndef _THREADS_SEMAPHORE_H_
#define _THREADS_SEMAPHORE_H_

#include <mutex>
#include <condition_variable>

namespace Threads
{
	class semaphore
	{
	private:
		std::mutex mtx;
		std::condition_variable cv;
		int count;
	public:
		semaphore (int count_ = 0)
			: count(count_) {}

		inline void notify()
		{
			std::unique_lock<std::mutex> lock(mtx);
			count++;
			cv.notify_one();
		}

		inline void wait()
		{
			std::unique_lock<std::mutex> lock(mtx);
			while ( count <= 0 ) {
				cv.wait(lock);
			}
			count--;
		}
	};
}

#endif//_THREADS_SEMAPHORE_H_