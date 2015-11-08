
#ifndef _JOBS_H_
#define _JOBS_H_

#include "core/types/types.h"

#include <thread>
#include <functional>
#include <atomic>
#include <list>
#include <vector>
#include <utility>

// Uncomment this if in a C++11 compiler that supports Variadic templates.
//#define _USING_VARIADIC_TEMPLATES_

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

	enum jobType_t : uint8_t
	{
		JOBTYPE_DEFAULT		= 0x01,
		JOBTYPE_PHYSICS		= 0x02,
		JOBTYPE_ENGINE		= 0x04,
		JOBTYPE_RENDERSTEP	= 0x08,
		JOBTYPE_TERRAIN		= 0x10,

		JOBTYPE_ALL = 0xFF
	};

	class jobRequest_t
	{
	public:
		jobType_t				type;
		std::function<void()>	function;
	};
	class jobTask_t
	{
	public:
		jobType_t				type;
		uint8_t					worker_index;
	};
	class jobState_t
	{
	public:
		std::atomic<bool>		perform;
		std::function<void()>	function;

	public:
		jobState_t(void)
			: perform(false)
		{
			;
		}
		jobState_t(const jobState_t& that)
			: perform(false)
		{
			function = that.function;
		}
		jobState_t(const jobState_t&& that)
			: perform(false)
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
		CORE_API explicit System ( const uint8_t threadCount );
		//	Destructor
		// Waits for all jobs to finish and then ends the job handler thread.
		CORE_API ~System ( void );

	public:
#ifdef _USING_VARIADIC_TEMPLATES_
		//	AddJobRequest ( jobType, Fn&& function, Args... args )
		// Adds a job request to the internal list with the given type. It may begin execution before this function returns.
		template <typename Fn, typename... Args>
		CORE_API static void AddJobRequest ( jobType_t jobType, Fn&& function, Args...args )
		{
			jobRequest_t newJob;
			newJob.type = jobType;
			newJob.function = std::bind(function,args...);
			Active->_internal_AddJob( newJob );
		}
		//	AddJobRequest ( Fn&& function, Args... args )
		// Adds a job request to the internal list. It may begin execution before this function returns.
		// Defaults to a JOBTYPE_DEFAULT type.
		template <typename Fn, typename... Args>
		CORE_API static void AddJobRequest ( Fn&& function, Args...args )
		{
			jobRequest_t newJob;
			newJob.type = JOBTYPE_DEFAULT;
			newJob.function = std::bind(function,args...);
			Active->_internal_AddJob( newJob );
		}
#else
		//	AddJobRequest ( jobType, Fn&& function, Args... args )
		// Adds a job request to the internal list with the given type. It may begin execution before this function returns.
		template <typename Fn>
		static void AddJobRequest ( jobType_t jobType, Fn&& function ) {
			jobRequest_t newJob = {jobType,std::bind(function)};
			Active->_internal_AddJob( newJob );
		}
		template <typename Fn, typename V1>
		static void AddJobRequest ( jobType_t jobType, Fn&& function, V1 v1 ) {
			jobRequest_t newJob = {jobType,std::bind(function,v1)};
			Active->_internal_AddJob( newJob );
		}
		template <typename Fn, typename V1, typename V2>
		static void AddJobRequest ( jobType_t jobType, Fn&& function, V1 v1, V2 v2 ) {
			jobRequest_t newJob = {jobType,std::bind(function,v1,v2)};
			Active->_internal_AddJob( newJob );
		}
		template <typename Fn, typename V1, typename V2, typename V3>
		static void AddJobRequest ( jobType_t jobType, Fn&& function, V1 v1, V2 v2, V3 v3 ) {
			jobRequest_t newJob = {jobType,std::bind(function,v1,v2,v3)};
			Active->_internal_AddJob( newJob );
		}
		template <typename Fn, typename V1, typename V2, typename V3, typename V4>
		static void AddJobRequest ( jobType_t jobType, Fn&& function, V1 v1, V2 v2, V3 v3, V4 v4 ) {
			jobRequest_t newJob = {jobType,std::bind(function,v1,v2,v3,v4)};
			Active->_internal_AddJob( newJob );
		}
		template <typename Fn, typename V1, typename V2, typename V3, typename V4, typename V5>
		static void AddJobRequest ( jobType_t jobType, Fn&& function, V1 v1, V2 v2, V3 v3, V4 v4, V5 v5 ) {
			jobRequest_t newJob = {jobType,std::bind(function,v1,v2,v3,v4,v5)};
			Active->_internal_AddJob( newJob );
		}
		template <typename Fn, typename V1, typename V2, typename V3, typename V4, typename V5, typename V6>
		static void AddJobRequest ( jobType_t jobType, Fn&& function, V1 v1, V2 v2, V3 v3, V4 v4, V5 v5, V6 v6 ) {
			jobRequest_t newJob = {jobType,std::bind(function,v1,v2,v3,v4,v5,v6)};
			Active->_internal_AddJob( newJob );
		}
		template <typename Fn, typename V1, typename V2, typename V3, typename V4, typename V5, typename V6, typename V7>
		static void AddJobRequest ( jobType_t jobType, Fn&& function, V1 v1, V2 v2, V3 v3, V4 v4, V5 v5, V6 v6, V7 v7 ) {
			jobRequest_t newJob = {jobType,std::bind(function,v1,v2,v3,v4,v5,v6,v7)};
			Active->_internal_AddJob( newJob );
		}
		template <typename Fn, typename V1, typename V2, typename V3, typename V4, typename V5, typename V6, typename V7, typename V8>
		static void AddJobRequest ( jobType_t jobType, Fn&& function, V1 v1, V2 v2, V3 v3, V4 v4, V5 v5, V6 v6, V7 v7, V8 v8 ) {
			jobRequest_t newJob = {jobType,std::bind(function,v1,v2,v3,v4,v5,v6,v7,v8)};
			Active->_internal_AddJob( newJob );
		}
		template <typename Fn, typename V1, typename V2, typename V3, typename V4, typename V5, typename V6, typename V7, typename V8, typename V9>
		static void AddJobRequest ( jobType_t jobType, Fn&& function, V1 v1, V2 v2, V3 v3, V4 v4, V5 v5, V6 v6, V7 v7, V8 v8, V9 v9 ) {
			jobRequest_t newJob = {jobType,std::bind(function,v1,v2,v3,v4,v5,v6,v7,v8,v9)};
			Active->_internal_AddJob( newJob );
		}
		template <typename Fn, typename V1, typename V2, typename V3, typename V4, typename V5, typename V6, typename V7, typename V8, typename V9, typename V10>
		static void AddJobRequest ( jobType_t jobType, Fn&& function, V1 v1, V2 v2, V3 v3, V4 v4, V5 v5, V6 v6, V7 v7, V8 v8, V9 v9, V10 v10 ) {
			jobRequest_t newJob = {jobType,std::bind(function,v1,v2,v3,v4,v5,v6,v7,v8,v9,v10)};
			Active->_internal_AddJob( newJob );
		}
		//	AddJobRequest ( Fn&& function, Args... args )
		// Adds a job request to the internal list. It may begin execution before this function returns.
		// Defaults to a JOBTYPE_DEFAULT type.
		template <typename Fn>
		static void AddJobRequest ( Fn&& function ) {
			jobRequest_t newJob = {JOBTYPE_DEFAULT,std::bind(function)};
			Active->_internal_AddJob( newJob );
		}
		template <typename Fn, typename V1>
		static void AddJobRequest ( Fn&& function, V1 v1 ) {
			jobRequest_t newJob = {JOBTYPE_DEFAULT,std::bind(function,v1)};
			Active->_internal_AddJob( newJob );
		}
		template <typename Fn, typename V1, typename V2>
		static void AddJobRequest ( Fn&& function, V1 v1, V2 v2 ) {
			jobRequest_t newJob = {JOBTYPE_DEFAULT,std::bind(function,v1,v2)};
			Active->_internal_AddJob( newJob );
		}
		template <typename Fn, typename V1, typename V2, typename V3>
		static void AddJobRequest ( Fn&& function, V1 v1, V2 v2, V3 v3 ) {
			jobRequest_t newJob = {JOBTYPE_DEFAULT,std::bind(function,v1,v2,v3)};
			Active->_internal_AddJob( newJob );
		}
		template <typename Fn, typename V1, typename V2, typename V3, typename V4>
		static void AddJobRequest ( Fn&& function, V1 v1, V2 v2, V3 v3, V4 v4 ) {
			jobRequest_t newJob = {JOBTYPE_DEFAULT,std::bind(function,v1,v2,v3,v4)};
			Active->_internal_AddJob( newJob );
		}
		template <typename Fn, typename V1, typename V2, typename V3, typename V4, typename V5>
		static void AddJobRequest ( Fn&& function, V1 v1, V2 v2, V3 v3, V4 v4, V5 v5 ) {
			jobRequest_t newJob = {JOBTYPE_DEFAULT,std::bind(function,v1,v2,v3,v4,v5)};
			Active->_internal_AddJob( newJob );
		}
		template <typename Fn, typename V1, typename V2, typename V3, typename V4, typename V5, typename V6>
		static void AddJobRequest ( Fn&& function, V1 v1, V2 v2, V3 v3, V4 v4, V5 v5, V6 v6 ) {
			jobRequest_t newJob = {JOBTYPE_DEFAULT,std::bind(function,v1,v2,v3,v4,v5,v6)};
			Active->_internal_AddJob( newJob );
		}
		template <typename Fn, typename V1, typename V2, typename V3, typename V4, typename V5, typename V6, typename V7>
		static void AddJobRequest ( Fn&& function, V1 v1, V2 v2, V3 v3, V4 v4, V5 v5, V6 v6, V7 v7 ) {
			jobRequest_t newJob = {JOBTYPE_DEFAULT,std::bind(function,v1,v2,v3,v4,v5,v6,v7)};
			Active->_internal_AddJob( newJob );
		}
		template <typename Fn, typename V1, typename V2, typename V3, typename V4, typename V5, typename V6, typename V7, typename V8>
		static void AddJobRequest ( Fn&& function, V1 v1, V2 v2, V3 v3, V4 v4, V5 v5, V6 v6, V7 v7, V8 v8 ) {
			jobRequest_t newJob = {JOBTYPE_DEFAULT,std::bind(function,v1,v2,v3,v4,v5,v6,v7,v8)};
			Active->_internal_AddJob( newJob );
		}
		template <typename Fn, typename V1, typename V2, typename V3, typename V4, typename V5, typename V6, typename V7, typename V8, typename V9>
		static void AddJobRequest ( Fn&& function, V1 v1, V2 v2, V3 v3, V4 v4, V5 v5, V6 v6, V7 v7, V8 v8, V9 v9 ) {
			jobRequest_t newJob = {JOBTYPE_DEFAULT,std::bind(function,v1,v2,v3,v4,v5,v6,v7,v8,v9)};
			Active->_internal_AddJob( newJob );
		}
		template <typename Fn, typename V1, typename V2, typename V3, typename V4, typename V5, typename V6, typename V7, typename V8, typename V9, typename V10>
		static void AddJobRequest ( Fn&& function, V1 v1, V2 v2, V3 v3, V4 v4, V5 v5, V6 v6, V7 v7, V8 v8, V9 v9, V10 v10 ) {
			jobRequest_t newJob = {JOBTYPE_DEFAULT,std::bind(function,v1,v2,v3,v4,v5,v6,v7,v8,v9,v10)};
			Active->_internal_AddJob( newJob );
		}
#endif

		//	Perform ()
		// Calls WaitForJobs( JOBTYPE_ALL ). Will stop until all jobs are done.
		CORE_API static void Perform ( void )
		{
			WaitForJobs( JOBTYPE_ALL );
		}
		//	WaitForJobs ( jobType )
		// Waits for the given jobs in the group are all finished. 
		CORE_API static void WaitForJobs ( const jobType_t jobType )
		{
			Active->_internal_WaitForJobs( jobType );
		}

	private:
		//	_internal_AddJob
		// Adds a job to the internal list, and may immediately start execution.
		CORE_API void _internal_AddJob ( const jobRequest_t& jobToAdd );
		//	_internal_WaitForJobs
		// Waits for the given job type
		void _internal_WaitForJobs ( const jobType_t jobType );
		
		//	_internal_JobCycle
		// Job distribution loop.
		// Continues while m_systemEnabled is true and the current job list is not empty.
		void _internal_JobCycle ( void );
		//	_internal_WorkerCycle
		// Job execution loop.
		// Is assigned a job by _internal_JobCycle, and then executes.
		// When not working, state.perform is set to false.
		void _internal_WorkerCycle ( jobState_t* state );
	private:
		// Is the system enabled for adding jobs?
		std::atomic<bool>	m_systemEnabled;
		// Thread that sends out jobs
		std::thread			m_managerThread;
		// Threads that perform jobs
		std::vector<std::thread>	m_jobThreads;
		std::vector<jobState_t>		m_jobStates;

		// Flag to mask the highest priority jobs
		std::atomic<uint8_t>	m_highPriorityFlag;

		// Job storage
		std::atomic_flag	m_jobRequestLock;
		std::list<jobRequest_t>	m_jobRequests;
		std::atomic_flag	m_jobTaskLock;
		std::list<jobTask_t>	m_jobTasks;
	};
};

#endif//_JOBS_H_