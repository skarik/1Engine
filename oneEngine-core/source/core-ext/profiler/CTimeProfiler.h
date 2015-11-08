#ifdef _WIN32

#ifndef _C_TIME_PROFILER_H_
#define _C_TIME_PROFILER_H_

#include "core/time/time.h"

#include <string>
using std::string;
#include <map>
#include <mutex>

class CTimeProfilerUI;

namespace Debug
{
	class CTimeProfiler
	{
	public:
		// Constructor
		CORE_API explicit CTimeProfiler ( void );

		// Zeros profile with given name
		CORE_API void ZeroTimeProfile ( string const& );

		// Begins profile with given name
		CORE_API void BeginTimeProfile ( string const& );

		// Returns time taken in seconds
		CORE_API double EndTimeProfile ( string const& );

		// Returns time taken in seconds, but only edits if returned time is larger
		CORE_API double EndMaxTimeProfile ( string const& );

		// Returns current aggregate time taken in seconds
		CORE_API double EndAddTimeProfile ( string const& );

		// Prints and returns time
		CORE_API double EndPrintTimeProfile ( string const& );

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

		typedef std::map<string,sTimeProfile> TimeProfileMap;
		TimeProfileMap mTimeProfiles;
	};
}

CORE_API extern Debug::CTimeProfiler TimeProfiler;

#endif

#endif