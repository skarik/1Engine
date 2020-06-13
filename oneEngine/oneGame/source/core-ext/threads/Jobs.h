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

// Uncomment this if in a C++11 compiler that supports Variadic templates.
#define _USING_VARIADIC_TEMPLATES_
// This controls whether spin-locking or cv-signals are used for the worker loop.
#define _USING_JOB_SIGNALING_SYSTEM_

namespace Jobs
{
	struct result_t
	{
		uint8_t flag;
		result_t ( void ) : flag(0)
		{
			;
		}
	};

	enum jobTypeBits : uint32_t
	{
		kJobTypeDefault		= 0x0001,
		kJobTypePhysics		= 0x0002,
		kJobTypeEngine		= 0x0004,
		kJobTypeRenderStep	= 0x0008,
		kJobTypeTerrain		= 0x0010,

		kJobTypeRendererSorting	= 0x0100,

		kJobTypeALL = 0xFF
	};

	class jobRequest_t
	{
	public:
		jobTypeBits				type;
		std::function<void()>	function;
	};
	class jobTask_t
	{
	public:
		jobTypeBits				type;
		uint32_t				worker_index;
	};
	class jobState_t
	{
	public:
		Threads::semaphore		signal;
		std::atomic<bool>		perform;
		std::function<void()>	function;

	public:
		jobState_t(void)
			: signal(), perform(false)
		{
			;
		}
		jobState_t(const jobState_t& that)
			: signal(), perform(false)
		{
			function = that.function;
		}
		jobState_t(const jobState_t&& that)
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
#ifdef _USING_VARIADIC_TEMPLATES_
		struct Current
		{
		public:
			//	AddJobRequest ( jobTypeBits, Fn&& function, Args... args )
			// Adds a job request with the given type to the Current system. It may begin execution before this function returns.
			template <typename Fn, typename... Args>
			static void AddJobRequest ( jobTypeBits jobTypeBits, Fn&& function, Args...args )
			{
				Active->AddJobRequest( jobTypeBits, function, args... );
			}
			//	AddJobRequest ( Fn&& function, Args... args )
			// Adds a job request to the Current system. It may begin execution before this function returns.
			// Defaults to a kJobTypeDefault type.
			template <typename Fn, typename... Args>
			static void AddJobRequest ( Fn&& function, Args...args )
			{
				Active->AddJobRequest( function, args... );
			}	
			//	Perform ()
			// Calls WaitForJobs( kJobTypeALL ). Will stop until all jobs are done.
			static void Perform ( void )
			{
				Active->Perform();
			}
			//	WaitForJobs ( jobTypeBits )
			// Waits for the given jobs in the group are all finished. 
			static void WaitForJobs ( const jobTypeBits jobTypeBits )
			{
				Active->WaitForJobs( jobTypeBits );
			}
		};

		//	AddJobRequest ( jobTypeBits, Fn&& function, Args... args )
		// Adds a job request with the given type to the internal list. It may begin execution before this function returns.
		template <typename Fn, typename... Args>
		void AddJobRequest ( jobTypeBits jobTypeBits, Fn&& function, Args...args )
		{
			jobRequest_t newJob;
			newJob.type = jobTypeBits;
			newJob.function = std::bind(function,args...);
			this->_internal_AddJob( newJob );
		}
		//	AddJobRequest ( Fn&& function, Args... args )
		// Adds a job request to the internal list. It may begin execution before this function returns.
		// Defaults to a kJobTypeDefault type.
		template <typename Fn, typename... Args>
		void AddJobRequest ( Fn&& function, Args...args )
		{
			this->AddJobRequest( kJobTypeDefault, function, args... );
		}
#else
		//	AddJobRequest ( jobTypeBits, Fn&& function, Args... args )
		// Adds a job request to the internal list with the given type. It may begin execution before this function returns.
		template <typename Fn>
		static void AddJobRequest ( jobTypeBits jobTypeBits, Fn&& function ) {
			jobRequest_t newJob = {jobTypeBits,std::bind(function)};
			Active->_internal_AddJob( newJob );
		}
		template <typename Fn, typename V1>
		static void AddJobRequest ( jobTypeBits jobTypeBits, Fn&& function, V1 v1 ) {
			jobRequest_t newJob = {jobTypeBits,std::bind(function,v1)};
			Active->_internal_AddJob( newJob );
		}
		template <typename Fn, typename V1, typename V2>
		static void AddJobRequest ( jobTypeBits jobTypeBits, Fn&& function, V1 v1, V2 v2 ) {
			jobRequest_t newJob = {jobTypeBits,std::bind(function,v1,v2)};
			Active->_internal_AddJob( newJob );
		}
		template <typename Fn, typename V1, typename V2, typename V3>
		static void AddJobRequest ( jobTypeBits jobTypeBits, Fn&& function, V1 v1, V2 v2, V3 v3 ) {
			jobRequest_t newJob = {jobTypeBits,std::bind(function,v1,v2,v3)};
			Active->_internal_AddJob( newJob );
		}
		template <typename Fn, typename V1, typename V2, typename V3, typename V4>
		static void AddJobRequest ( jobTypeBits jobTypeBits, Fn&& function, V1 v1, V2 v2, V3 v3, V4 v4 ) {
			jobRequest_t newJob = {jobTypeBits,std::bind(function,v1,v2,v3,v4)};
			Active->_internal_AddJob( newJob );
		}
		template <typename Fn, typename V1, typename V2, typename V3, typename V4, typename V5>
		static void AddJobRequest ( jobTypeBits jobTypeBits, Fn&& function, V1 v1, V2 v2, V3 v3, V4 v4, V5 v5 ) {
			jobRequest_t newJob = {jobTypeBits,std::bind(function,v1,v2,v3,v4,v5)};
			Active->_internal_AddJob( newJob );
		}
		template <typename Fn, typename V1, typename V2, typename V3, typename V4, typename V5, typename V6>
		static void AddJobRequest ( jobTypeBits jobTypeBits, Fn&& function, V1 v1, V2 v2, V3 v3, V4 v4, V5 v5, V6 v6 ) {
			jobRequest_t newJob = {jobTypeBits,std::bind(function,v1,v2,v3,v4,v5,v6)};
			Active->_internal_AddJob( newJob );
		}
		template <typename Fn, typename V1, typename V2, typename V3, typename V4, typename V5, typename V6, typename V7>
		static void AddJobRequest ( jobTypeBits jobTypeBits, Fn&& function, V1 v1, V2 v2, V3 v3, V4 v4, V5 v5, V6 v6, V7 v7 ) {
			jobRequest_t newJob = {jobTypeBits,std::bind(function,v1,v2,v3,v4,v5,v6,v7)};
			Active->_internal_AddJob( newJob );
		}
		template <typename Fn, typename V1, typename V2, typename V3, typename V4, typename V5, typename V6, typename V7, typename V8>
		static void AddJobRequest ( jobTypeBits jobTypeBits, Fn&& function, V1 v1, V2 v2, V3 v3, V4 v4, V5 v5, V6 v6, V7 v7, V8 v8 ) {
			jobRequest_t newJob = {jobTypeBits,std::bind(function,v1,v2,v3,v4,v5,v6,v7,v8)};
			Active->_internal_AddJob( newJob );
		}
		template <typename Fn, typename V1, typename V2, typename V3, typename V4, typename V5, typename V6, typename V7, typename V8, typename V9>
		static void AddJobRequest ( jobTypeBits jobTypeBits, Fn&& function, V1 v1, V2 v2, V3 v3, V4 v4, V5 v5, V6 v6, V7 v7, V8 v8, V9 v9 ) {
			jobRequest_t newJob = {jobTypeBits,std::bind(function,v1,v2,v3,v4,v5,v6,v7,v8,v9)};
			Active->_internal_AddJob( newJob );
		}
		template <typename Fn, typename V1, typename V2, typename V3, typename V4, typename V5, typename V6, typename V7, typename V8, typename V9, typename V10>
		static void AddJobRequest ( jobTypeBits jobTypeBits, Fn&& function, V1 v1, V2 v2, V3 v3, V4 v4, V5 v5, V6 v6, V7 v7, V8 v8, V9 v9, V10 v10 ) {
			jobRequest_t newJob = {jobTypeBits,std::bind(function,v1,v2,v3,v4,v5,v6,v7,v8,v9,v10)};
			Active->_internal_AddJob( newJob );
		}
		//	AddJobRequest ( Fn&& function, Args... args )
		// Adds a job request to the internal list. It may begin execution before this function returns.
		// Defaults to a kJobTypeDefault type.
		template <typename Fn>
		static void AddJobRequest ( Fn&& function ) {
			jobRequest_t newJob = {kJobTypeDefault,std::bind(function)};
			Active->_internal_AddJob( newJob );
		}
		template <typename Fn, typename V1>
		static void AddJobRequest ( Fn&& function, V1 v1 ) {
			jobRequest_t newJob = {kJobTypeDefault,std::bind(function,v1)};
			Active->_internal_AddJob( newJob );
		}
		template <typename Fn, typename V1, typename V2>
		static void AddJobRequest ( Fn&& function, V1 v1, V2 v2 ) {
			jobRequest_t newJob = {kJobTypeDefault,std::bind(function,v1,v2)};
			Active->_internal_AddJob( newJob );
		}
		template <typename Fn, typename V1, typename V2, typename V3>
		static void AddJobRequest ( Fn&& function, V1 v1, V2 v2, V3 v3 ) {
			jobRequest_t newJob = {kJobTypeDefault,std::bind(function,v1,v2,v3)};
			Active->_internal_AddJob( newJob );
		}
		template <typename Fn, typename V1, typename V2, typename V3, typename V4>
		static void AddJobRequest ( Fn&& function, V1 v1, V2 v2, V3 v3, V4 v4 ) {
			jobRequest_t newJob = {kJobTypeDefault,std::bind(function,v1,v2,v3,v4)};
			Active->_internal_AddJob( newJob );
		}
		template <typename Fn, typename V1, typename V2, typename V3, typename V4, typename V5>
		static void AddJobRequest ( Fn&& function, V1 v1, V2 v2, V3 v3, V4 v4, V5 v5 ) {
			jobRequest_t newJob = {kJobTypeDefault,std::bind(function,v1,v2,v3,v4,v5)};
			Active->_internal_AddJob( newJob );
		}
		template <typename Fn, typename V1, typename V2, typename V3, typename V4, typename V5, typename V6>
		static void AddJobRequest ( Fn&& function, V1 v1, V2 v2, V3 v3, V4 v4, V5 v5, V6 v6 ) {
			jobRequest_t newJob = {kJobTypeDefault,std::bind(function,v1,v2,v3,v4,v5,v6)};
			Active->_internal_AddJob( newJob );
		}
		template <typename Fn, typename V1, typename V2, typename V3, typename V4, typename V5, typename V6, typename V7>
		static void AddJobRequest ( Fn&& function, V1 v1, V2 v2, V3 v3, V4 v4, V5 v5, V6 v6, V7 v7 ) {
			jobRequest_t newJob = {kJobTypeDefault,std::bind(function,v1,v2,v3,v4,v5,v6,v7)};
			Active->_internal_AddJob( newJob );
		}
		template <typename Fn, typename V1, typename V2, typename V3, typename V4, typename V5, typename V6, typename V7, typename V8>
		static void AddJobRequest ( Fn&& function, V1 v1, V2 v2, V3 v3, V4 v4, V5 v5, V6 v6, V7 v7, V8 v8 ) {
			jobRequest_t newJob = {kJobTypeDefault,std::bind(function,v1,v2,v3,v4,v5,v6,v7,v8)};
			Active->_internal_AddJob( newJob );
		}
		template <typename Fn, typename V1, typename V2, typename V3, typename V4, typename V5, typename V6, typename V7, typename V8, typename V9>
		static void AddJobRequest ( Fn&& function, V1 v1, V2 v2, V3 v3, V4 v4, V5 v5, V6 v6, V7 v7, V8 v8, V9 v9 ) {
			jobRequest_t newJob = {kJobTypeDefault,std::bind(function,v1,v2,v3,v4,v5,v6,v7,v8,v9)};
			Active->_internal_AddJob( newJob );
		}
		template <typename Fn, typename V1, typename V2, typename V3, typename V4, typename V5, typename V6, typename V7, typename V8, typename V9, typename V10>
		static void AddJobRequest ( Fn&& function, V1 v1, V2 v2, V3 v3, V4 v4, V5 v5, V6 v6, V7 v7, V8 v8, V9 v9, V10 v10 ) {
			jobRequest_t newJob = {kJobTypeDefault,std::bind(function,v1,v2,v3,v4,v5,v6,v7,v8,v9,v10)};
			Active->_internal_AddJob( newJob );
		}
#endif

		//	Perform ()
		// Calls WaitForJobs( kJobTypeALL ). Will stop until all jobs are done.
		CORE_API void Perform ( void )
		{
			WaitForJobs( kJobTypeALL );
		}
		//	WaitForJobs ( jobTypeBits )
		// Waits for the given jobs in the group are all finished. 
		CORE_API void WaitForJobs ( const jobTypeBits jobTypeBits )
		{
			this->_internal_WaitForJobs( jobTypeBits );
		}

	private:
		//	_internal_AddJob
		// Adds a job to the internal list, and may immediately start execution.
		CORE_API void _internal_AddJob ( const jobRequest_t& jobToAdd );
		//	_internal_WaitForJobs
		// Waits for the given job type
		CORE_API void _internal_WaitForJobs ( const jobTypeBits jobTypeBits );
		
		//	_internal_JobCycle
		// Job distribution loop.
		// Continues while m_systemEnabled is true and the current job list is not empty.
		CORE_API void _internal_JobCycle ( void );
		//	_internal_WorkerCycle
		// Job execution loop.
		// Is assigned a job by _internal_JobCycle, and then executes.
		// When not working, state.perform is set to false.
		CORE_API void _internal_WorkerCycle ( jobState_t* state );
	private:
		// Is the system enabled for adding jobs?
		std::atomic<bool>	m_systemEnabled;
		// Locked by the thread doing the main work
		std::mutex			m_systemMainLock;
		// Thread that sends out jobs
		std::thread			m_managerThread;
		// Threads that perform jobs
		std::vector<std::thread>	m_jobThreads;
		std::vector<jobState_t>		m_jobStates;

#ifdef _USING_JOB_SIGNALING_SYSTEM_
		// Used to prevent the CPU usage from spin locking, instead executing update when a job is requested or finished.
		Threads::semaphore			m_jobsignal;
#endif

		// Flag to mask the highest priority jobs
		std::atomic<uint8_t>	m_highPriorityFlag;

		// Job storage
		std::atomic_flag	m_jobRequestLock;
		std::list<jobRequest_t>	m_jobRequests;
		std::atomic_flag	m_jobTaskLock;
		std::list<jobTask_t>	m_jobTasks;
	};
};

#endif//CORE_EXT_THREADS_JOBS_H_