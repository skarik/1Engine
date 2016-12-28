
#ifndef _THREADS_SEMAPHORE_H_
#define _THREADS_SEMAPHORE_H_

#include <mutex>
#include <atomic>
#include <condition_variable>
#include "compile_mode.h"
//#include "core/os.h"

namespace Threads
{
/*#ifdef _WIN32
	class semaphore
	{
	private:
		HANDLE ghSemaphore;

	public:
		semaphore (int count_ = 0)
		{
			ghSemaphore = CreateSemaphore(
				NULL,
				64,
				64,
				NULL);
		}
		~semaphore ()
		{
			CloseHandle(ghSemaphore);
		}

		inline void notify()
		{
			ReleaseSemaphore(ghSemaphore, 1, NULL);
		}

		inline void wait()
		{
			WaitForSingleObject(ghSemaphore,INFINITE);
		}

		inline bool try_wait()
		{
			DWORD result = WaitForSingleObject(ghSemaphore,0);
			if ( result == WAIT_TIMEOUT )
				return false;
			return true;
		}
	};
#else*/
#ifndef THREAD_CLASSES_USE_ATOMICS

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
			++count;
			cv.notify_one();
		}

		inline void wait()
		{
			std::unique_lock<std::mutex> lock(mtx);
			while ( count <= 0 ) {
				cv.wait(lock);
			}
			--count;
		}
	};

#else //THREAD_CLASSES_USE_ATOMICS

	class semaphore
	{
	private:
		std::mutex mtx;
		std::condition_variable cv;
		std::atomic_int count;

	public:
		semaphore (int count_ = 0)
			: count(count_) {}

		inline void notify()
		{
			std::unique_lock<std::mutex> lock(mtx);
			++count;
			cv.notify_one();
		}

		inline void wait()
		{
			std::unique_lock<std::mutex> lock(mtx);
			while ( count <= 0 ) {
				cv.wait(lock);
			}
			--count;
		}

		inline bool try_wait()
		{
			std::unique_lock<std::mutex> lock(mtx);
			if ( count > 0 ) {
				--count;
				return true;
			}
			return false;
		}
	};

#endif//THREAD_CLASSES_USE_ATOMICS
//#endif
}

#endif//_THREADS_SEMAPHORE_H_