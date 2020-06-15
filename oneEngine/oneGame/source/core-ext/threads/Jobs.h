//===============================================================================================//
//
//		Jobs.h
//
// Thread pool-like job system. Has sync points for different jobs.
//
//===============================================================================================//
#ifndef CORE_EXT_THREADS_JOBS_H_
#define CORE_EXT_THREADS_JOBS_H_

#include "core/types/types.h"
#include "core-ext/threads/semaphore.h"

#include <thread>
#include <functional>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <list>
#include <vector>
#include <utility>

// This controls whether spin-locking or cv-signals are used for the worker loop.
#define _USING_JOB_SIGNALING_SYSTEM_

namespace core {
namespace jobs
{
	enum JobTypeBits : uint32_t
	{
		kJobTypeDefault		= 0x0001,
		kJobTypePhysics		= 0x0002,
		kJobTypeEngine		= 0x0004,
		kJobTypeRenderStep	= 0x0008,
		kJobTypeTerrain		= 0x0010,

		kJobTypeRendererSorting	= 0x0100,

		kJobTypeALL = 0xFF
	};

	// Identifier for a job
	struct JobId
	{
		uint32_t				value = kInvalidIndex;

		enum JobConstants : uint32_t
		{
			kInvalidIndex = uint32_t(-1)
		};
	};

	class JobRequest
	{
	public:
		JobTypeBits				type;
		JobId					id;
		std::function<void()>	function;
	};

	struct JobTask
	{
		JobTypeBits				type;
		uint32_t				worker_index;
	};

	class JobState
	{
	public:
		Threads::semaphore		signal;
		std::atomic<bool>		perform;
		std::function<void()>	function;
		JobId					id;

	public:
		JobState(void)
			: signal(), perform(false)
		{
			;
		}
		JobState(const JobState& that)
			: signal(), perform(false)
		{
			function = that.function;
		}
		JobState(const JobState&& that)
			: signal(), perform(false)
		{
			function = that.function;
		}
	};

	class System
	{
		CORE_API static System* Active;
	public:
		//	Constructor
		// Initializes the job handler thread with the given amount of threads.
		CORE_API explicit System ( const uint8_t threadCount = 0 );
		//	Destructor
		// Waits for all jobs to finish and then ends the job handler thread.
		CORE_API ~System ( void );

	public:
		struct Current
		{
		public:
			//	AddJobRequest ( jobTypeBits, Fn&& function, Args... args )
			// Adds a job request with the given type to the Current system. It may begin execution before this function returns.
			template <typename Fn, typename... Args>
			static JobId AddJobRequest ( JobTypeBits jobTypeBits, Fn&& function, Args...args )
			{
				return Active->AddJobRequest( jobTypeBits, function, args... );
			}
			//	AddJobRequest ( Fn&& function, Args... args )
			// Adds a job request to the Current system. It may begin execution before this function returns.
			// Defaults to a kJobTypeDefault type.
			template <typename Fn, typename... Args>
			static JobId AddJobRequest ( Fn&& function, Args...args )
			{
				return Active->AddJobRequest( function, args... );
			}	
			//	Perform ()
			// Calls WaitForJobs( kJobTypeALL ). Will stop until all jobs are done.
			static void Perform ( void )
			{
				Active->Perform();
			}
			//	WaitForJobs ( jobTypeBits )
			// Waits for the given jobs in the group are all finished. 
			static void WaitForJobs ( const JobTypeBits jobTypeBits )
			{
				Active->WaitForJobs( jobTypeBits );
			}
			//	WaitForJob ( JobId )
			// Waits until the given issued job is ifnished.
			static void WaitForJob ( const JobId& id )
			{
				Active->WaitForJob( id );
			}
		};

		//	AddJobRequest ( jobTypeBits, Fn&& function, Args... args )
		// Adds a job request with the given type to the internal list. It may begin execution before this function returns.
		template <typename Fn, typename... Args>
		JobId AddJobRequest ( JobTypeBits jobTypeBits, Fn&& function, Args...args )
		{
			JobRequest newJob;
			newJob.type = jobTypeBits;
			newJob.function = std::bind(function,args...);
			return this->_internal_AddJob( newJob );
		}
		//	AddJobRequest ( Fn&& function, Args... args )
		// Adds a job request to the internal list. It may begin execution before this function returns.
		// Defaults to a kJobTypeDefault type.
		template <typename Fn, typename... Args>
		JobId AddJobRequest ( Fn&& function, Args...args )
		{
			return this->AddJobRequest( kJobTypeDefault, function, args... );
		}

		//	Perform ()
		// Calls WaitForJobs( kJobTypeALL ). Will stop until all jobs are done.
		CORE_API void Perform ( void )
		{
			WaitForJobs( kJobTypeALL );
		}
		//	WaitForJobs ( jobTypeBits )
		// Waits for the given jobs in the group are all finished. 
		CORE_API void WaitForJobs ( const JobTypeBits jobTypeBits )
		{
			this->_internal_WaitForJobs( jobTypeBits ); // Why the extra call?
		}
		//	WaitForJob ( jobId )
		// Waits for the given job to finish
		CORE_API void WaitForJob ( const JobId& id )
		{
			this->_internal_WaitForJob( id ); // Why the extra call?
		}

	private:
		//	_internal_AddJob
		// Adds a job to the internal list, and may immediately start execution.
		CORE_API JobId _internal_AddJob ( const JobRequest& jobToAdd );
		//	_internal_WaitForJobs
		// Waits for the given job type
		CORE_API void _internal_WaitForJobs ( const JobTypeBits jobTypeBits );
		//	_internal_WaitForJob
		// Waits for the given job id
		CORE_API void _internal_WaitForJob ( const JobId& id );

		//	_internal_JobCycle
		// Job distribution loop.
		// Continues while m_systemEnabled is true and the current job list is not empty.
		CORE_API void _internal_JobCycle ( void );
		//	_internal_WorkerCycle
		// Job execution loop.
		// Is assigned a job by _internal_JobCycle, and then executes.
		// When not working, state.perform is set to false.
		CORE_API void _internal_WorkerCycle ( JobState* state );
	private:
		// Is the system enabled for adding jobs?
		std::atomic<bool>		m_systemEnabled;
		// Locked by the thread doing the main work
		std::mutex				m_systemMainLock;
		// Thread that sends out jobs
		std::thread				m_managerThread;
		// Threads that perform jobs
		std::vector<std::thread>
								m_jobThreads;
		std::vector<JobState>	m_jobStates;

#ifdef _USING_JOB_SIGNALING_SYSTEM_
		// Used to prevent the CPU usage from spin locking, instead executing update when a job is requested or finished.
		Threads::semaphore		m_jobsignal;
#endif

		// Flag to mask the highest priority jobs
		std::atomic<uint8_t>	m_highPriorityFlag;

		// Job storage
		std::atomic_flag		m_jobRequestLock;
		std::list<JobRequest>	m_jobRequests;
		std::atomic_flag		m_jobTaskLock;
		std::list<JobTask>		m_jobTasks;

		// Job ID tracking
		std::atomic<uint32_t>	m_nextJobId;
	};
}}

#endif//CORE_EXT_THREADS_JOBS_H_