// CTime class
// controls shiz

#ifdef __APPLE__
	#ifdef TARGET_OS_MAC

#ifndef _C_TIME_
#define _C_TIME_

// Include
#include <ctime>
#include "floattype.h"

// Namespace for the lulz

// Class Definition
class CTime
{
public:
	static void Init();
	static void Tick();

public:
	static Real deltaTime;
	static Real smoothDeltaTime;

	static Real fixedTime;
	static Real targetFixedTime;

private:
	static clock_t iLastTick;
	static clock_t iCurrentTick;

	static Real fDeltaTimes[10];

};

typedef CTime Time;

#endif

	#endif
#endif