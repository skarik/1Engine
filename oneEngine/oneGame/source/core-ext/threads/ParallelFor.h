#ifndef CORE_EXT_THREADS_PARALLEL_FOR_H_
#define CORE_EXT_THREADS_PARALLEL_FOR_H_

#include "core/types/types.h"
#include "core-ext/threads/semaphore.h"

#include "core-ext/threads/Jobs.h"

#include <thread>
#include <functional>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <list>
#include <vector>
#include <utility>

namespace core
{
	template<typename Indexor, typename Function>
	void parallel_for ( bool threaded, const Indexor& first, const Indexor& until, Function func )
	{
		if (threaded)
		{
			//const size_t hw_threadCount = std::thread::hardware_concurrency(); // TODO: dedicated lightweight threadpool for parallel_for, maybe
			for (Indexor index = first; index < until; ++index)
			{
				core::jobs::System::Current::AddJobRequest(core::jobs::kJobTypeParallelFor, func, index);
			}
			core::jobs::System::Current::WaitForJobs(core::jobs::kJobTypeParallelFor);

#		if 0 // This is slower
			if (until > first)
			{
				std::vector<std::thread> threads;
				threads.resize(until - first);
				for (Indexor index = first; index < until; ++index)
				{
					threads[index - first] = std::thread(func, index);
				}
				for (auto& thread : threads)
				{
					thread.join();
				}
			}
#		endif
		}
		else
		{
			for (Indexor index = first; index < until; ++index)
			{
				func(index);
			}
		}
	}
}

#endif//CORE_EXT_THREADS_PARALLEL_FOR_H_