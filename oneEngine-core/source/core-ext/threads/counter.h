
#ifndef _THREADS_COUNT_H_
#define _THREADS_COUNT_H_

#include <mutex>
#include <condition_variable>

namespace Threads
{
	class counter
	{
	private:
		std::mutex mtx;
		std::condition_variable cv;
		int count;
	public:
		counter (int count_ = 0)
			: count(count_) {}

		inline void add()
		{
			std::unique_lock<std::mutex> lock(mtx);
			count++;
		}
		inline void sub()
		{
			std::unique_lock<std::mutex> lock(mtx);
			count--;
			if ( count <= 0 )
			{
				cv.notify_all();
			}
		}

		inline void wait_for_0()
		{
			std::unique_lock<std::mutex> lock(mtx);
			while ( count > 0 ) {
				cv.wait(lock);
			}
		}
	};
}

#endif//_THREADS_COUNT_H_