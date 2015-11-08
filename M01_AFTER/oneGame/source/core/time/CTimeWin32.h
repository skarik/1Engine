// CTime class
// controls shiz

#ifndef _C_TIME_
#define _C_TIME_

// Include
#include "core/types/types.h"
#include "core/types/float.h"

#include <ctime>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 1
#endif
#include <windows.h>

// Namespace for the lulz

// Class Definition
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
	CORE_API static Real deltaTime;
	CORE_API static Real limitedDeltaTime;
	CORE_API static Real smoothDeltaTime;

	CORE_API static Real fixedTime;
	CORE_API static Real targetFixedTime;

	CORE_API static Real currentTime;

private:
	static LARGE_INTEGER iLastTick;
	static LARGE_INTEGER iCurrentTick;

	static LARGE_INTEGER iFrequency;

	static Real fDeltaTimes[10];

};

typedef CTime Time;

#endif