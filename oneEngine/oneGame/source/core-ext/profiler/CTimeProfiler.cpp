
#ifdef _WIN32

#include "CTimeProfiler.h"
#include "core/debug/Console.h"
#include "core/types/float.h"

#include <iostream>
using std::cout;
using std::endl;

using namespace debug;

CTimeProfiler TimeProfiler;


CTimeProfiler::CTimeProfiler ( void )
{
	QueryPerformanceFrequency( &freq );
}

void CTimeProfiler::ZeroTimeProfile ( const char* sName )
{
	std::lock_guard<std::mutex> lock( io_lock );

	arstring<128> cname (sName);
	auto profile = mTimeProfiles.find(cname);
	if ( profile == mTimeProfiles.end() ) {
		sTimeProfile newProfile;
		newProfile.aggregate = 0;
		newProfile.delta = 0;
		mTimeProfiles[cname] = newProfile;
	}
	else {
		profile->second.delta = profile->second.aggregate;
		profile->second.aggregate = 0;
	}
}

void CTimeProfiler::BeginTimeProfile ( const char* sName )
{
	std::lock_guard<std::mutex> lock( io_lock );

	arstring<128> cname (sName);
	auto profile = mTimeProfiles.find(cname);
	if ( profile == mTimeProfiles.end() ) {
		sTimeProfile newProfile;
		newProfile.aggregate = 0;
		newProfile.delta = 0;
		QueryPerformanceCounter( &(newProfile.tick1) );
		mTimeProfiles[cname] = newProfile;
	}
	else {
		QueryPerformanceCounter( &(profile->second.tick1) );
	}
}


double CTimeProfiler::EndTimeProfile ( const char* sName )
{
	std::lock_guard<std::mutex> lock( io_lock );

	arstring<128> cname (sName);
	sTimeProfile& currentProfile = mTimeProfiles[cname];
	QueryPerformanceCounter( &(currentProfile.tick2) );

	{
		std::lock_guard<std::mutex> lock( delta_lock );
		if ( currentProfile.aggregate > FTYPE_PRECISION ) {
			currentProfile.delta = currentProfile.aggregate;
			currentProfile.aggregate = 0;
		}
		else {
			currentProfile.delta = (((double)(currentProfile.tick2.QuadPart-currentProfile.tick1.QuadPart))/(freq.QuadPart))*1000.0f;
		}
	}

	return currentProfile.delta;
}

double CTimeProfiler::EndMaxTimeProfile ( const char* sName )
{
	std::lock_guard<std::mutex> lock( io_lock );

	arstring<128> cname (sName);
	sTimeProfile& currentProfile = mTimeProfiles[cname];
	QueryPerformanceCounter( &(currentProfile.tick2) );

	double time;
	{
		std::lock_guard<std::mutex> lock( delta_lock );
		time = (((double)(currentProfile.tick2.QuadPart-currentProfile.tick1.QuadPart))/(freq.QuadPart))*1000.0f;
		if ( time > currentProfile.delta ) {
			currentProfile.delta = time;
		}
	}

	return time;
}

double CTimeProfiler::EndAddTimeProfile ( const char* sName )
{
	std::lock_guard<std::mutex> lock( io_lock );

	arstring<128> cname (sName);
	sTimeProfile& currentProfile = mTimeProfiles[cname];
	QueryPerformanceCounter( &(currentProfile.tick2) );

	{
		std::lock_guard<std::mutex> lock( delta_lock );
		currentProfile.aggregate += (((double)(currentProfile.tick2.QuadPart-currentProfile.tick1.QuadPart))/(freq.QuadPart))*1000.0f;
	}

	return currentProfile.aggregate;
}

double CTimeProfiler::EndPrintTimeProfile ( const char* sName )
{
	std::lock_guard<std::mutex> lock( io_lock );

	double result = EndTimeProfile( sName );
	cout << "[Profiler] " << sName << " took " << result << " ms" << endl;

	return result;
}

#endif