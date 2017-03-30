#ifdef _WIN32

#ifndef _C_TIME_PROFILER_H_
#define _C_TIME_PROFILER_H_

#include "core/time/time.h"
#include "core/containers/arstring.h"

#include <string>
using std::string;
#include <map>
#include <mutex>

class CTimeProfilerUI;

namespace debug
{
	class CTimeProfiler
	{
	public:
		// Constructor
		CORE_API explicit CTimeProfiler ( void );

		// Zeros profile with given name
		CORE_API void ZeroTimeProfile ( const char* );

		// Begins profile with given name
		CORE_API void BeginTimeProfile ( const char* );

		// Returns time taken in seconds
		CORE_API double EndTimeProfile ( const char* );

		// Returns time taken in seconds, but only edits if returned time is larger
		CORE_API double EndMaxTimeProfile ( const char* );

		// Returns current aggregate time taken in seconds
		CORE_API double EndAddTimeProfile ( const char* );

		// Prints and returns time
		CORE_API double EndPrintTimeProfile ( const char* );

		std::mutex	delta_lock;
	private:
		friend CTimeProfilerUI;

		std::mutex	io_lock;

		struct sTimeProfile
		{
			LARGE_INTEGER tick1;
			LARGE_INTEGER tick2;
			double aggregate;
			double delta;
		};
		LARGE_INTEGER freq;

		typedef std::map<arstring<128>,sTimeProfile> TimeProfileMap;
		TimeProfileMap mTimeProfiles;
	};
}

CORE_API extern debug::CTimeProfiler TimeProfiler;

#endif

#endif