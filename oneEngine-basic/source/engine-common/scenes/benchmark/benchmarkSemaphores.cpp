// Include game settings
#include "core/settings/CGameSettings.h"
// Include debug console
#include "core/debug/CDebugConsole.h"
#include "engine/utils/CDeveloperConsole.h"
#include "engine-common/utils/CDeveloperConsoleUI.h"
// Needed includes
#include "benchmarkSemaphores.h"
#include "core-ext/threads/semaphore.h"
#include "core-ext/threads/counter.h"
#include <iostream>
#include <chrono>
#include <thread>

#define ITERATION_COUNT 10000000

void benchmarkSemaphores::LoadScene ( void )
{
	using namespace std;

	cout << "Beginning semaphore tests.\n" << endl;
#ifdef THREAD_CLASSES_USE_ATOMICS
	cout << "Semaphores are compiled to use atomics." << endl;
#else
	cout << "Semaphores are compiled without atomics." << endl;
#endif

	//=========================================//
	cout << "\nTest 1: single signal, single wait" << endl;
	//=========================================//
	{
		auto start_time = chrono::high_resolution_clock::now();
		{
			std::mutex sync; sync.lock();
			Threads::semaphore sem;
			auto fWaiter = [&]()
			{
				sync.lock(); sync.unlock();
				for ( uint64_t i = 0; i < ITERATION_COUNT; ++i )
				{
					sem.wait();
				}
			};
			auto fUser = [&]()
			{
				sync.lock(); sync.unlock();
				for ( uint64_t i = 0; i < ITERATION_COUNT; ++i )
				{
					sem.notify();
				}
			};

			thread threadWaiter(fWaiter);
			thread threadUser(fUser);
			sync.unlock();

			threadWaiter.join();
			threadUser.join();
		}
		auto end_time = chrono::high_resolution_clock::now();
		std::chrono::duration<double> diff = end_time-start_time;
		cout << "Time taken was " << diff.count() << " s" << endl;
	}


	//=========================================//
	cout << "\nTest 2: multiple signal, single wait" << endl;
	//=========================================//
	{
		auto start_time = chrono::high_resolution_clock::now();
		{
			std::mutex sync; sync.lock();
			Threads::semaphore sem;
			auto fWaiter = [&]()
			{
				sync.lock(); sync.unlock();
				for ( uint64_t i = 0; i < ITERATION_COUNT; ++i )
				{
					sem.wait();
				}
			};
			auto fUser = [&]()
			{
				sync.lock(); sync.unlock();
				for ( uint64_t i = 0; i < ITERATION_COUNT/4; ++i )
				{
					sem.notify();
				}
			};

			thread threadWaiter(fWaiter);
			thread threadUser0(fUser);
			thread threadUser1(fUser);
			thread threadUser2(fUser);
			thread threadUser3(fUser);
			sync.unlock();

			threadWaiter.join();
			threadUser0.join();
			threadUser1.join();
			threadUser2.join();
			threadUser3.join();
		}
		auto end_time = chrono::high_resolution_clock::now();
		std::chrono::duration<double> diff = end_time-start_time;
		cout << "Time taken was " << diff.count() << " s" << endl;
	}


	//=========================================//
	cout << "\nTest 3: single signal, multiple wait" << endl;
	//=========================================//
	{
		auto start_time = chrono::high_resolution_clock::now();
		{
			std::mutex sync; sync.lock();
			Threads::semaphore sem;
			auto fWaiter = [&]()
			{
				sync.lock(); sync.unlock();
				for ( uint64_t i = 0; i < ITERATION_COUNT/4; ++i )
				{
					sem.wait();
				}
			};
			auto fUser = [&]()
			{
				sync.lock(); sync.unlock();
				for ( uint64_t i = 0; i < ITERATION_COUNT; ++i )
				{
					sem.notify();
				}
			};

			thread threadWaiter0(fWaiter);
			thread threadWaiter1(fWaiter);
			thread threadWaiter2(fWaiter);
			thread threadWaiter3(fWaiter);
			thread threadUser0(fUser);
			sync.unlock();

			threadWaiter0.join();
			threadWaiter1.join();
			threadWaiter2.join();
			threadWaiter3.join();
			threadUser0.join();
		}
		auto end_time = chrono::high_resolution_clock::now();
		std::chrono::duration<double> diff = end_time-start_time;
		cout << "Time taken was " << diff.count() << " s" << endl;
	}


	//=========================================//
	cout << "\nTest 4: multiple signal, multiple wait" << endl;
	//=========================================//
	{
		auto start_time = chrono::high_resolution_clock::now();
		{
			std::mutex sync; sync.lock();
			Threads::semaphore sem;
			auto fWaiter = [&]()
			{
				sync.lock(); sync.unlock();
				for ( uint64_t i = 0; i < ITERATION_COUNT; ++i )
				{
					sem.wait();
				}
			};
			auto fUser = [&]()
			{
				sync.lock(); sync.unlock();
				for ( uint64_t i = 0; i < ITERATION_COUNT; ++i )
				{
					sem.notify();
				}
			};

			thread threadWaiter0(fWaiter);
			thread threadWaiter1(fWaiter);
			thread threadWaiter2(fWaiter);
			thread threadWaiter3(fWaiter);
			thread threadUser0(fUser);
			thread threadUser1(fUser);
			thread threadUser2(fUser);
			thread threadUser3(fUser);
			sync.unlock();

			threadWaiter0.join();
			threadWaiter1.join();
			threadWaiter2.join();
			threadWaiter3.join();
			threadUser0.join();
			threadUser1.join();
			threadUser2.join();
			threadUser3.join();
		}
		auto end_time = chrono::high_resolution_clock::now();
		std::chrono::duration<double> diff = end_time-start_time;
		cout << "Time taken was " << diff.count() << " s" << endl;
	}
}