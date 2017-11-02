//===============================================================================================//
//
//		CTime : Timer class, Win32
//
//===============================================================================================//
#if _WIN32
#ifndef C_TIME_
#define C_TIME_

#include "core/os.h"
#include "core/types/types.h"
#include "core/types/float.h"

#include <ctime>

class CTime
{
public:
	CORE_API static void Init();
	CORE_API static void Tick();

	// Timer return current CPU time in seconds
	CORE_API static Real CurrentTime ( void );
	// Timer return current CPU time in milliseconds
	CORE_API static Real_d GetCurrentCPUTime ( void );

	CORE_API static Real TrainerFactor ( Real );
	CORE_API static Real SqrtTrainerFactor ( Real );

public:
	// Time in seconds the last frame took to run
	CORE_API static Real deltaTime;
	CORE_API static Real limitedDeltaTime;
	// Low-pass filtered value of deltaTime
	CORE_API static Real smoothDeltaTime;

	CORE_API static Real fixedTime;
	CORE_API static Real targetFixedTime;
	// Time in seconds game has been running
	CORE_API static Real currentTime;

private:
	static LARGE_INTEGER iLastTick;
	static LARGE_INTEGER iCurrentTick;

	static LARGE_INTEGER iFrequency;

	static Real fDeltaTimes[10];

};

typedef CTime Time;

#endif//C_TIME_
#endif