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
	static ftype deltaTime;
	static ftype smoothDeltaTime;

	static ftype fixedTime;
	static ftype targetFixedTime;

private:
	static clock_t iLastTick;
	static clock_t iCurrentTick;

	static ftype fDeltaTimes[10];

};

typedef CTime Time;

#endif

	#endif
#endif